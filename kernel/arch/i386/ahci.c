#include <kernel/ahci.h>
#include <stdio.h>
#include <kernel/pci.h>

static int check_type(hba_port* port){
	uint32_t ssts = port->ssts;
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

void port_probe(hba_mem* abar){
    uint32_t pi = abar->pi;
    int i = 0;
    while (i < 32){
        if (pi & 1){
            int dt = check_type(&abar->ports[i]);
            if (dt == AHCI_DEV_SATA){
				log(LOG_SERIAL, false, "SATA drive found at port %d\n", i);
			} else if (dt == AHCI_DEV_SATAPI){
				log(LOG_SERIAL, false, "SATAPI drive found at port %d\n", i);
			} else if (dt == AHCI_DEV_SEMB){
				log(LOG_SERIAL, false, "SEMB drive found at port %d\n", i);
			} else if (dt == AHCI_DEV_PM){
				log(LOG_SERIAL, false, "PM drive found at port %d\n", i);
			} else {
				log(LOG_SERIAL, false, "No drive found at port %d\n", i);
			}
        }
        pi >>= 1;
        i++;
    }
}

void ahci_init(){
    pci_device_t device = pci_find_any(SATA_CONTROLLER_PCI_CLASS, SATA_CONTROLLER_PCI_SUBCLSS, -1);
    if (!(device.valid)) {
        log(LOG_SERIAL, false, "No SATA controller attached!\n");
        return;
    }
    log(LOG_SERIAL, false, "SATA controller attached\n");
}
