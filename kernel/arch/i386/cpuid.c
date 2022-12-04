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

void get_brand(uint32_t* buf){
    //uint32_t brand[12];
    if (!__get_cpuid_max(0x80000004, NULL)) {
        fprintf(LOG_STDERR, "Feature not implemented.");
        return 2;
    }
    __get_cpuid(0x80000002, buf+0x0, buf+0x1, buf+0x2, buf+0x3);
    __get_cpuid(0x80000003, buf+0x4, buf+0x5, buf+0x6, buf+0x7);
    __get_cpuid(0x80000004, buf+0x8, buf+0x9, buf+0xa, buf+0xb);
    return buf;
}