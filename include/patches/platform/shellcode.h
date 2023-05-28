#ifndef _PLATFORM_SHC_H
#define _PLATFORM_SHC_H
#include <stdint.h>

void set_shc_region(void *buf);
uint32_t *copy_shc(int platform, uint32_t jmp);

#endif