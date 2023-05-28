#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "formats/macho.h"
#include "plooshfinder.h"
#include "patches/platform/patch.h"

#define symbol_to_patch "____ZNK5dyld39MachOFile24forEachSupportedPlatformEU13block_pointerFvNS_8PlatformEjjE_block_invoke"

void platform_check_patch(void *buf, int platform) {
    // this patch tricks dyld into thinking everything is for the current platform
    struct nlist_64 *forEachSupportedPlatform = macho_find_symbol(buf, symbol_to_patch);
    if (!forEachSupportedPlatform) return;

    void *func_addr = buf + forEachSupportedPlatform->offset;
    uint64_t func_len = macho_get_symbol_size(forEachSupportedPlatform);
    if (!func_len) return;

    patch_platform_check(buf, func_addr, func_len, platform);
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
    size_t dyld_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    void *dyld_buf = (void *) malloc(dyld_len);
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

    int platform = macho_get_platform(dyld_buf);
    if (platform == 0) {
        free(orig_dyld_buf);
        return 1;
    }

    platform_check_patch(dyld_buf, platform);

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