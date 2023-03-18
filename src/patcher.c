#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "macho.h"
#include "plooshfinder.h"
#include "patches/platform/ios15.h"
#include "patches/platform/ios16.h"
#include "patches/platform/old.h"

void *dyld_buf;
size_t dyld_len;
int platform = 0;

int get_platform() {
    void *after_header = (char *)dyld_buf + 0x20;
    void *before_platform = after_header;

    while (*(uint32_t *)before_platform != 0x32) {
        before_platform += 4;
    }

    if (*(uint8_t *)before_platform == 0x32) {
        uint32_t *platform_ptr = (uint32_t *)before_platform + 2;
        platform = *platform_ptr;
    }

    if (platform > 5) {
        printf("Unknown platform!\n");
        return 1;
    }
    return 0;
}

void patch_platform_check() {
    // this patch tricks dyld into thinking everything is for the current platform
    struct section_64 *text_section = macho_find_section(dyld_buf, "__TEXT", "__text");
    if (!text_section) return;

    void *section_addr = dyld_buf + text_section->offset;
    uint64_t section_len = text_section->size;

    patch_platform_check15(section_addr, section_len, platform);
    patch_platform_check16(section_addr, section_len, platform);
    patch_platform_check_old(section_addr, section_len, platform);
}

int main(int argc, char **argv) {
    FILE *fp = NULL;

    if (argc < 3) {
        printf("Usage: %s <input dyld> <patched dyld>\n", argv[0]);
        return 0;
    }

    fp = fopen(argv[1], "rb");
    if (!fp) {
        printf("Failed to open dyld!\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    dyld_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    dyld_buf = (void *)malloc(dyld_len);
    if (!dyld_buf) {
        printf("Out of memory while allocating region for dyld!\n");
        fclose(fp);
        return -1;
    }

    fread(dyld_buf, 1, dyld_len, fp);
    fclose(fp);

    uint32_t magic = macho_get_magic(dyld_buf);

    if (!magic) {
        free(dyld_buf);
        return 1;
    }

    void *orig_dyld_buf = dyld_buf;
    if (magic == 0xbebafeca) {
        dyld_buf = macho_find_arch(dyld_buf, CPU_TYPE_ARM64);
        if (!dyld_buf) {
            free(orig_dyld_buf);
            return 1;
        }
    }

    if (get_platform() != 0) {
        printf("Failed to get platform!\n");
        free(orig_dyld_buf);
        return 1;
    }

    patch_platform_check();

    fp = fopen(argv[2], "wb");
    if(!fp) {
        printf("Failed to open output file!\n");
        free(orig_dyld_buf);
        return -1;
    }
    
    fwrite(orig_dyld_buf, 1, dyld_len, fp);
    fflush(fp);
    fclose(fp);

    free(orig_dyld_buf);

    return 0;
}