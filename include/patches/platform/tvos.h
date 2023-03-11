#ifndef _PLATFORM_TVOS_H
#define _PLATFORM_TVOS_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void patch_platform_check_tvos(void *dyld_buf, size_t dyld_len, uint32_t platform);

#endif