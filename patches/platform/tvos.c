#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "patchfinder.h"

int _tvos_internal_platform = 0;

bool platform_check_callback_tvos(uint32_t *stream) {
    stream[3] = 0x52800001 | (_tvos_internal_platform << 5);

    printf("%s: Patched tvOS platform check (mov: 0x%x)\n", __FUNCTION__, 0x52800001 | (_tvos_internal_platform << 5));

    return true;
}

void patch_platform_check_tvos(void *dyld_buf, size_t dyld_len, uint32_t platform) {
    _tvos_internal_platform = platform;

    uint32_t tvos_matches[] = {
        0xf9400260, // ldr x0, [x*, 0x20]
        0x29400000, // ldp
        0xf9400000, // ldr x*, [x0, 0x10]
        0x52800001, // mov w1, *
        0x14000000  // b 
    };

    uint32_t tvos_masks[] = {
        0xffc003e0,
        0xffc00000,
        0xffc003e0,
        0xffe0001f,
        0xfc000000
    };
    pf_find_maskmatch(dyld_buf, dyld_len, tvos_matches, tvos_masks, sizeof(tvos_matches) / sizeof(uint32_t), (void *)platform_check_callback_tvos);

    tvos_matches[4] = 0xd63f0000; // blr x*
    tvos_masks[4] = 0xfffffc1f;

    pf_find_maskmatch(dyld_buf, dyld_len, tvos_matches, tvos_masks, sizeof(tvos_matches) / sizeof(uint32_t), (void *)platform_check_callback_tvos);

    // this codegen SUCKS
    uint32_t tvos_matches2[] = {
        0x1a800000, // csel w*, w*, w*, eq
        0xf9400260, // ldr x0, [x*, 0x20]
        0xf9400000, // ldr x*, [x0, 0x10]
        0x52800001, // mov w1, *
        0x14000000  // b 
    };

    uint32_t tvos_masks2[] = {
        0xffe0fc00,
        0xffc003e0,
        0xffc003e0,
        0xffe0001f,
        0xfc000000
    };

    pf_find_maskmatch(dyld_buf, dyld_len, tvos_matches2, tvos_masks2, sizeof(tvos_matches2) / sizeof(uint32_t), (void *)platform_check_callback_tvos);
}