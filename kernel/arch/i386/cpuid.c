#include <kernel/cpuid.h>
#include <cpuid.h>
#include <stdio.h>
#include <types.h>


/* Example: Get CPU's model number */
static int get_model(void)
{
    int ebx, unused;
    __cpuid(0, unused, ebx, unused, unused);
    return ebx;
}

/* Example: Check for builtin local APIC. */
static int check_apic(void)
{
    unsigned int eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & CPUID_FEAT_EDX_APIC;
}

int get_model_name(uint32_t* buf){
    //uint32_t brand[12];
    unsigned long ebx, unused;
    __cpuid(0, unused, ebx, unused, unused);
    switch(ebx){
        case 0x756e6547: // intel magic code
            unsigned long max_eax;
            __cpuid(0x80000000, max_eax, unused, unused, unused);
            if (!(max_eax >= 0x80000004)){
                return 1;
            }
            break;
        case 0x68747541: // amd magic code
            unsigned long extended;
            __cpuid(0x80000000, extended, unused, unused, unused);
            if (extended == 0){
                return 1;
            }
            if(!(extended >= 0x80000002)){
                return 1;
            }
            break;
        default:
            //printf("Unknown cpu detected\n");
            break;
    }
    if (!__get_cpuid_max(0x80000004, NULL)) {
        fprintf(LOG_STDERR, "Feature not implemented.\n");
        return 1;
    }
    __get_cpuid(0x80000002, buf+0x0, buf+0x1, buf+0x2, buf+0x3);
    __get_cpuid(0x80000003, buf+0x4, buf+0x5, buf+0x6, buf+0x7);
    __get_cpuid(0x80000004, buf+0x8, buf+0x9, buf+0xa, buf+0xb);
    return 0;
}

void intel_cpuid(){
    printf("Intel cpu\n");
    unsigned long eax, ebx, ecx, edx, max_eax, signature, unused;
    int model, family, type, brand, stepping, reserved;
	int extended_family = -1;
    __cpuid(1, eax, ebx, unused, unused);
    model = (eax >> 4) & 0xf;
	family = (eax >> 8) & 0xf;
	type = (eax >> 12) & 0x3;
	brand = ebx & 0xff;
	stepping = eax & 0xf;
	reserved = eax >> 14;
	signature = eax;
    printf("Type %d - ", type);
    switch(type) {
		case 0:
		    printf("Original OEM");
		    break;
		case 1:
		    printf("Overdrive");
		    break;
		case 2:
		    printf("Dual-capable");
		    break;
		case 3:
		    printf("Reserved");
		break;
	}
    printf("\n");
    printf("Family %d - ", family);
	switch(family) {
		case 3:
		    printf("i386");
		    break;
		case 4:
		    printf("i486");
		    break;
		case 5:
		    printf("Pentium");
		    break;
		case 6:
		    printf("Pentium Pro");
		    break;
		case 15:
		    printf("Pentium 4");
	}
    printf("\n");
    if(family == 15) {
		extended_family = (eax >> 20) & 0xff;
		printf("Extended family %d\n", extended_family);
	}
    printf("Model %d - ", model);
    switch(family) {
		case 3:
		break;
		case 4:
		switch(model) {
			case 0:
			case 1:
			printf("DX");
			break;
			case 2:
			printf("SX");
			break;
			case 3:
			printf("487/DX2");
			break;
			case 4:
			printf("SL");
			break;
			case 5:
			printf("SX2");
			break;
			case 7:
			printf("Write-back enhanced DX2");
			break;
			case 8:
			printf("DX4");
			break;
		}
		break;
		case 5:
		switch(model) {
			case 1:
			printf("60/66");
			break;
			case 2:
			printf("75-200");
			break;
			case 3:
			printf("for 486 system");
			break;
			case 4:
			printf("MMX");
			break;
		}
		break;
		case 6:
		switch(model) {
			case 1:
			printf("Pentium Pro");
			break;
			case 3:
			printf("Pentium II Model 3");
			break;
			case 5:
			printf("Pentium II Model 5/Xeon/Celeron");
			break;
			case 6:
			printf("Celeron");
			break;
			case 7:
			printf("Pentium III/Pentium III Xeon - external L2 cache");
			break;
			case 8:
			printf("Pentium III/Pentium III Xeon - internal L2 cache");
			break;
		}
		break;
		case 15:
		break;
	}
    printf("\n");
    __cpuid(0x80000000, max_eax, unused, unused, unused);
}

void amd_cpuid(){
    printf("AMD cpu\n");
    unsigned long extended, eax, ebx, ecx, edx, unused;
	int family, model, stepping, reserved;
	__cpuid(1, eax, unused, unused, unused);
	model = (eax >> 4) & 0xf;
	family = (eax >> 8) & 0xf;
	stepping = eax & 0xf;
	reserved = eax >> 12;
	printf("Family: %d Model: %d [", family, model);
	switch(family) {
		case 4:
		printf("486 Model %d", model);
		break;
		case 5:
		switch(model) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 6:
			case 7:
			printf("K6 Model %d", model);
			break;
			case 8:
			printf("K6-2 Model 8");
			break;
			case 9:
			printf("K6-III Model 9");
			break;
			default:
			printf("K5/K6 Model %d", model);
			break;
		}
		break;
		case 6:
		switch(model) {
			case 1:
			case 2:
			case 4:
			printf("Athlon Model %d", model);
			break;
			case 3:
			printf("Duron Model 3");
			break;
			case 6:
			printf("Athlon MP/Mobile Athlon Model 6");
			break;
			case 7:
			printf("Mobile Duron Model 7");
			break;
			default:
			printf("Duron/Athlon Model %d", model);
			break;
		}
		break;
	}
	printf("]\n");
}

void detect_cpu(){
    uint32_t model_name[12];
  	if (get_model_name(model_name) == 0){
    printf("Model name : %s\n", model_name);
    }
    unsigned long ebx, unused;
    __cpuid(0, unused, ebx, unused, unused);
    switch(ebx){
        case 0x756e6547: // intel magic code
            intel_cpuid();
            break;
        case 0x68747541: // amd magic code
            amd_cpuid();
            break;
        default:
            printf("Unknown cpu detected\n");
            break;
    }
}