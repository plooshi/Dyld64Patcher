#ifndef _PLATFORM_IOS15_H
#define _PLATFORM_IOS15_H
#include <stdint.h>
#include <stdlib.h>

void patch_platform_check_new(void *real_buf, void *dyld_buf, size_t dyld_len, uint32_t platform);

#endif