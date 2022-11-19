#include <types.h>
#include <stdint.h>

#define CONFIG_ADDR 0xCF8
#define CONFIG_DATA 0xCFC

typedef struct {
  uint8_t irq_line;
  uint8_t valid : 1;
  uint32_t bar0;
  uint32_t bar4;
  uint32_t bar5;
  uint8_t bus;
  uint8_t slot;
  uint8_t func;
} pci_device_t;

typedef struct {
  /* PCI information */
  uint32_t vendor_id;
  uint32_t device_id;
  
  /* Information about the device */
  uint16_t device_class;
  uint16_t device_type; 
} pci_dev_descriptor_t;

pci_device_t pci_find(unsigned int vendor_id, unsigned int device_id);
pci_device_t pci_find_any(uint8_t class_code, uint8_t subclass_code, int8_t interface_value);