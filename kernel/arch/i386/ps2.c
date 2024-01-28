#include <kernel/io.h>
#include <kernel/keyboard.h>
#include <kernel/mouse.h>
#include <kernel/ps2.h>
#include <kernel/x86.h>
#include <stdio.h>
#include <types.h>

bool controllers[] = {true, true};

static int ps2_wait_input(void) {
  uint64_t timeout = 100000UL;
  while (--timeout) {
    if (!(inb(PS2_STATUS) & (1 << 1))) {
      return 0;
    }
  }
  return 1;
}

void ps2_command(uint8_t cmdbyte) {
  ps2_wait_input();
  outb(PS2_CMD, cmdbyte);
}

bool ps2_wait_write() {
  int timer = PS2_TIMEOUT;
  while ((inb(0x64) & 2) && timer-- > 0) {
    asm("pause");
  }
  return timer != 0;
}

bool ps2_wait_read() {
  int timer = PS2_TIMEOUT;
  while (!(inb(0x64) & 1) && timer-- >= 0) {
    asm("pause");
  }
  return timer != 0;
}

uint8_t ps2_read(uint32_t port) {
  if (ps2_wait_read()) {
    return inb(port);
  }
  log(LOG_SERIAL, false, "read failed");
  return -1;
}

bool ps2_write(uint32_t port, uint8_t b) {
  if (ps2_wait_write()) {
    outb(port, b);
    return true;
  }
  log(LOG_SERIAL, false, "write failed");
  return false;
}

bool ps2_write_device(uint32_t device, uint8_t b) {
  if (device != 0) {
    if (!ps2_write(PS2_CMD, PS2_WRITE_SECOND)) {
      return false;
    }
  }
  return ps2_write(PS2_DATA, b);
}

bool ps2_expect_ack() {
  uint8_t ret = ps2_read(PS2_DATA);
  if (ret != PS2_DEV_ACK) {
    log(LOG_SERIAL, false, "device failed to acknowledge command, sent %x",
        ret);
    return false;
  }
  return true;
}

void ps2_enable_port(uint32_t num, bool enable) {
  if (!controllers[num]) {
    return;
  }
  if (!enable) {
    ps2_write(PS2_CMD, num == 0 ? PS2_DISABLE_FIRST : PS2_DISABLE_SECOND);
  } else {
    ps2_write(PS2_CMD, num == 0 ? PS2_ENABLE_FIRST : PS2_ENABLE_SECOND);
  }
  ps2_read(PS2_DATA);
}

void ps2_wait_ms(uint32_t ms) {
  uint32_t i = 0;
  while (i++ < 10000 * ms) {
    asm volatile("pause");
  }
}

uint32_t ps2_identity_bytes_to_type(uint8_t first, uint8_t second) {
  if (first == 0x00 || first == 0x03 || first == 0x04) {
    return first;
  } else if (first == 0xAB) {
    if (second == 0x41 || second == 0xC1) {
      return PS2_KEYBOARD_TRANSLATED;
    } else if (second == 0x83) {
      return PS2_KEYBOARD;
    }
  }
  return PS2_DEVICE_UNKNOWN;
}

uint32_t ps2_identify_device(uint32_t num) {
  log(LOG_SERIAL, false, "identifying device %d...\n", num);
  ps2_write_device(num, PS2_DEV_DISABLE_SCAN); // Disables scanning
  ps2_expect_ack();
  ps2_write_device(num, PS2_DEV_IDENTIFY); // Identify
  ps2_expect_ack();
  uint32_t first_id_byte = ps2_read(PS2_DATA);
  uint32_t second_id_byte = ps2_read(PS2_DATA);
  return ps2_identity_bytes_to_type(first_id_byte, second_id_byte);
}

void init_ps2() {
  log(LOG_SERIAL, false, "Initializing PS/2 devices\n");
  bool dual_channel = true;
  x86_disable_int();
  ps2_write(PS2_CMD, PS2_DISABLE_FIRST);
  ps2_write(PS2_CMD, PS2_DISABLE_SECOND);
  inb(PS2_DATA);
  ps2_write(PS2_CMD, PS2_READ_CONFIG);
  uint8_t config = ps2_read(PS2_DATA);
  config |= PS2_CFG_SYSTEM_FLAG;
  if (config & PS2_CFG_MUST_BE_ZERO) {
    log(LOG_SERIAL, false, "invalid bit set in configuration byte\n");
  }
  config &= ~(PS2_CFG_FIRST_PORT | PS2_CFG_SECOND_PORT | PS2_CFG_TRANSLATION);
  ps2_write(PS2_CMD, PS2_WRITE_CONFIG);
  ps2_write(PS2_DATA, config);
  ps2_write(PS2_CMD, PS2_SELF_TEST);
  if (ps2_read(PS2_DATA) != PS2_SELF_TEST_OK) {
    log(LOG_SERIAL, false, "controller failed self-test\n");
    controllers[0] = false;
    controllers[1] = false;
    return;
  }
  ps2_write(PS2_CMD, PS2_WRITE_CONFIG);
  ps2_write(PS2_DATA, config);
  ps2_write(PS2_CMD, PS2_ENABLE_SECOND);
  ps2_write(PS2_CMD, PS2_READ_CONFIG);
  config = ps2_read(PS2_DATA);
  if (config & PS2_CFG_SECOND_CLOCK) {
    log(LOG_SERIAL, false, "only one PS/2 controller\n");
    dual_channel = false;
    controllers[1] = false;
  } else {
    ps2_write(PS2_CMD, PS2_DISABLE_SECOND);
  }
  ps2_write(PS2_CMD, PS2_TEST_FIRST);

  if (ps2_read(PS2_DATA) != PS2_TEST_OK) {
    log(LOG_SERIAL, false, "first PS/2 port failed testing\n");
    controllers[0] = false;
  }

  if (dual_channel) {
    ps2_write(PS2_CMD, PS2_TEST_SECOND);
    if (ps2_read(PS2_DATA) != PS2_TEST_OK) {
      log(LOG_SERIAL, false, "second PS/2 port failed testing\n");
      controllers[1] = false;
    }
  }
  for (uint32_t i = 0; i < 2; i++) {
    if (!controllers[i]) {
      continue;
    }
    ps2_enable_port(i, true);
    log(LOG_SERIAL, false, "resetting devices %d...\n", i);
    ps2_write_device(i, PS2_DEV_RESET);
    ps2_wait_ms(500);
    ps2_expect_ack();
    ps2_wait_ms(100);
    uint8_t ret = ps2_read(PS2_DATA);
    if (i == 0 && ret != PS2_DEV_RESET_ACK) {
      log(LOG_SERIAL, false, "keyboard failed to acknowledge reset, sent %x\n",
          ret);
      goto error;
    } else if (i == 1) {
      uint8_t ret2 = ps2_read(PS2_DATA);

      /* Mice are a complete mess, cut them some slack */
      if ((ret == PS2_DEV_RESET_ACK && (ret2 == PS2_DEV_ACK || ret2 == 0x00)) ||
          ((ret == PS2_DEV_ACK || ret == 0x00) && ret2 == PS2_DEV_RESET_ACK)) {
        /* Wrong if for readability */
      } else {
        log(LOG_SERIAL, false,
            "mice failed to acknowledge reset, sent %x, %x\n", ret, ret2);
        goto error;
      }
    }
    if (i == 0) {
      ps2_read(PS2_DATA);
      ps2_enable_port(i, false);
    }
    continue;
  error:
    ps2_enable_port(i, false);
    controllers[i] = false;
  }
  if (controllers[0]) {
    ps2_enable_port(0, true);
  }
  for (uint32_t i = 0; i < 2; i++) {
    if (controllers[i]) {
      uint32_t type = ps2_identify_device(i);
      bool driver_called = true;
      switch (type) {
      case PS2_KEYBOARD:
      case PS2_KEYBOARD_TRANSLATED:
        log(LOG_SERIAL, false, "keyboard\n");
        // init_kbd(i);
        init_keyboard(i);
        break;
      case PS2_MOUSE:
      case PS2_MOUSE_SCROLL_WHEEL:
      case PS2_MOUSE_FIVE_BUTTONS:
        log(LOG_SERIAL, false, "mouse\n");
        // init_mouse(i);
        break;
      default:
        driver_called = false;
        break;
      }
      if (driver_called) {
        config |= i == 0 ? PS2_CFG_FIRST_PORT : PS2_CFG_SECOND_PORT;
        config &= ~(i == 0 ? PS2_CFG_FIRST_CLOCK : PS2_CFG_SECOND_CLOCK);
      }
    }
  }
  ps2_write(PS2_CMD, PS2_WRITE_CONFIG);
  ps2_write(PS2_DATA, config);
  x86_enable_int();
}

void ps2_initialize() {
  ps2_command(PS2_DISABLE_PORT1);
  ps2_command(PS2_DISABLE_PORT2);
  size_t timeout = 1024;
  while ((inb(PS2_STATUS) & 1) && timeout > 0) {
    timeout--;
    inb(PS2_DATA);
  }
  if (timeout == 0) {
    log(LOG_SERIAL, false, "ps2: probably don't actually have PS/2.\n");
    return;
  }
  ps2_command(PS2_ENABLE_PORT1);
  ps2_command(PS2_ENABLE_PORT2);
  mouse_write(MOUSE_SET_DEFAULTS);
  mouse_write(MOUSE_DATA_ON);
}