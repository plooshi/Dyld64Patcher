#ifndef _PLATFORM_IOS16_H
#define _PLATFORM_IOS16_H
#include <stdint.h>
#include <stdlib.h>

void patch_platform_check16(void *real_buf, void *dyld_buf, size_t dyld_len, uint32_t platform);

#endif