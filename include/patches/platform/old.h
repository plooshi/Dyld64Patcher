#ifndef _PLATFORM_OLD_H
#define _PLATFORM_OLD_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void patch_platform_check_old(void *dyld_buf, size_t dyld_len, uint32_t platform);

#endif