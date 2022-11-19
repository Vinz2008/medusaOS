#include <kernel/ahci.h>
#include <stdio.h>
#include <kernel/pci.h>

void ahci_init(){
    pci_device_t device = pci_find_any(SATA_CONTROLLER_PCI_CLASS, SATA_CONTROLLER_PCI_SUBCLSS, -1);
    if (!(device.valid)) {
        log(LOG_SERIAL, false, "No SATA controller attached!\n");
        return;
    }
    log(LOG_SERIAL, false, "SATA controller attached\n");
}