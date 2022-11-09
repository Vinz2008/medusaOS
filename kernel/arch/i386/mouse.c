#include <kernel/mouse.h>
#include <types.h>
#include <stdio.h>
#include <kernel/ps2.h>
#include <kernel/irq_handlers.h>


uint32_t device;
mouse_t mouse_state;


uint32_t current_byte = 0;
int32 bytes_per_packet = 3;
uint8_t packet[4] = { 0 };

void sys_mouse_handler(x86_iframe_t* frame){
    uint8_t byte = ps2_read(PS2_DATA);
    if (current_byte == 0 && !(byte & MOUSE_ALWAYS_SET)) {
        return;
    }
    packet[current_byte] = byte;
    current_byte = (current_byte + 1) % bytes_per_packet;
    if (current_byte == 0) {
        mouse_handle_packet();
    }
}


void mouse_handle_packet(){
    mouse_t old_state = mouse_state;
    uint8_t flags = packet[0];
    int32 delta_x = (int32) packet[1];
    int32 delta_y = (int32) packet[2];
    uint8_t extra = 0;
    if (bytes_per_packet == 4) {
        extra = packet[3];
        if (extra & MOUSE_UNUSED_A || extra & MOUSE_UNUSED_B) {
            return;
        }
    }
    if (flags & MOUSE_X_OVERFLOW || flags & MOUSE_Y_OVERFLOW) {
        return;
    }
    if (flags & MOUSE_X_NEG) {
        delta_x |= 0xFFFFFF00;
    }

    if (flags & MOUSE_Y_NEG) {
        delta_y |= 0xFFFFFF00;
    }

    mouse_state.left_pressed = flags & MOUSE_LEFT;
    mouse_state.right_pressed = flags & MOUSE_RIGHT;
    mouse_state.middle_pressed = flags & MOUSE_MIDDLE;

    mouse_state.x += delta_x;
    mouse_state.y -= delta_y; // Point the y-axis downward
}

void mouse_set_sample_rate(uint8_t rate) {
    ps2_write_device(device, MOUSE_SET_SAMPLE);
    ps2_expect_ack();
    ps2_write_device(device, rate);
    ps2_expect_ack();
}

void mouse_set_resolution(uint8_t level) {
    ps2_write_device(device, MOUSE_SET_RESOLUTION);
    ps2_expect_ack();
    ps2_write_device(device, level);
    ps2_expect_ack();
}

void mouse_set_scaling(bool enabled) {
    uint8_t cmd = enabled ? MOUSE_ENABLE_SCALING : MOUSE_DISABLE_SCALING;

    ps2_write_device(device, cmd);
    ps2_expect_ack();
}

void mouse_enable_scroll_wheel() {
    mouse_set_sample_rate(200);
    mouse_set_sample_rate(100);
    mouse_set_sample_rate(80);
    uint32_t type = ps2_identify_device(device);
    if (type == PS2_MOUSE_SCROLL_WHEEL) {
        bytes_per_packet = 4;
        log(LOG_SERIAL, false,"enabled scroll wheel\n");
    } else {
        log(LOG_SERIAL, false,"unable to enable scroll wheel\n");
    }
}

void mouse_enable_five_buttons() {
    if (bytes_per_packet != 4) {
        return;
    }

    mouse_set_sample_rate(200);
    mouse_set_sample_rate(200);
    mouse_set_sample_rate(80);

    uint32_t type = ps2_identify_device(device);

    if (type != PS2_MOUSE_FIVE_BUTTONS) {
        log(LOG_SERIAL, false,"mouse has fewer than five buttons\n");
    } else {
        log(LOG_SERIAL, false,"five buttons enabled\n");
    }
}

void init_mouse(uint32_t dev){
    device = dev;
    irq_register_handler(12, sys_mouse_handler);
    mouse_enable_scroll_wheel();
    mouse_enable_five_buttons();
    mouse_set_sample_rate(80);
    mouse_set_resolution(0x00);
    mouse_set_scaling(false);
    ps2_write_device(device, PS2_DEV_ENABLE_SCAN);
    ps2_expect_ack();
}