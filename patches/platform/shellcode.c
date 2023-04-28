#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "plooshfinder.h"
#include "plooshfinder32.h"
#include "macho.h"
#include "patches/platform/shellcode.h"

uint32_t *shc_loc;
int shc_copied = 0;

uint32_t *get_shc_region(void *buf) {
    // we use a zero region in the the __TEXT segment of dyld
    // this is done so our shellcode is in an executable region
    // __unwind_info is the last section in __TEXT,
    // so we just go into the zero region and
    // place our shellcode 0x1000 bytes in
    
    struct segment_command_64 *segment = macho_get_segment(buf, "__TEXT");
    if (!segment) return 0;
    struct section_64 *section = macho_get_section(buf, segment, "__unwind_info");
    if (!section) return 0;

    void *section_addr = buf + section->offset;
    uint64_t section_len = section->size;

    void *shc_base = section_addr + section_len;
    uint64_t shc_off = (segment->filesize - section->offset) / 2;

    void *shc_addr = shc_base + shc_off;

    shc_loc = (uint32_t *) shc_addr;

    if (!shc_loc) {
        printf("%s: no shellcode location!\n", __FUNCTION__);
    }

    return shc_loc;
}

uint32_t *copy_shc(int platform, uint32_t jmp) {
    if (!shc_loc) {
        printf("%s: No shellcode location!\n", __FUNCTION__);
        return 0;
    }

    bool with_link = true;
    if ((jmp & 0xfffffc1f) == 0xd61f0000) {
        with_link = false;
    }

    uint32_t shellcode[] = {
        0xf100283f, // cmp x1, 10
        0x54000040, // b.eq 0x8
        0xd2800001, // mov x1, {plat}
        jmp,        // b{l}r {reg}
        ret
    };

    shellcode[2] |= platform << 5;

    uint32_t shc_size = sizeof(shellcode) / sizeof(uint32_t);

    if ((jmp & 0xfffffc1f) == 0xd61f0000) {
        // this is the old style, don't use a ret
        shc_size = shc_size - 1;
        shellcode[4] = 0;
    }

    uint32_t shc_off = shc_copied * shc_size;

    for (int i = 0; i < shc_size; i++) {
        shc_loc[shc_off + i] = shellcode[i];
    }

    shc_copied += 1;
    return shc_loc + shc_off;
}