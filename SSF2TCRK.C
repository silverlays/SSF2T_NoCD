#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TARGET_FILE "SF2TURBO.EXE"
#define BUFFER_SIZE 4096

unsigned char pattern[] = {
		0x1B, 0x22, 0x00, 0x01, 0x00, 0x00, 0x00, 0x33,
		0xC0, 0xA3, 0x7C, 0x06, 0x23, 0x00, 0xA0, 0x20
};
#define PATTERN_LEN 16

unsigned char patch[] = { 0xE9, 0xB4, 0x00, 0x00, 0x00 };
#define PATCH_LEN 5
#define PATCH_OFFSET_IN_PATTERN 9

int main() {
    FILE *f;
    unsigned char *buf;
    long file_pos = 0;
    size_t bytes_read;
    int i, j;
    int found = 0;

    printf("Super Street Fighter II Turbo -- Universal NoCD Patcher\n");
		printf("-------------------------------------------------------\n");

    buf = (unsigned char *)malloc(BUFFER_SIZE);
    if (buf == NULL) {
				printf("Error:Memory allocation failed.\n");
        return 1;
    }

    f = fopen(TARGET_FILE, "rb+");
    if (f == NULL) {
				printf("Error: Cannot open %s\n", TARGET_FILE);
				printf("Make sure the patcher is in the game folder.\n");
        free(buf);
        return 1;
    }

		printf("Scanning %s...\n", TARGET_FILE);

    while (!feof(f)) {
        file_pos = ftell(f);

        bytes_read = fread(buf, 1, BUFFER_SIZE, f);
        if (bytes_read < PATTERN_LEN) break;

        for (i = 0; i <= (int)(bytes_read - PATTERN_LEN); i++) {
            found = 1;
            for (j = 0; j < PATTERN_LEN; j++) {
                if (buf[i + j] != pattern[j]) {
                    found = 0;
                    break;
                }
            }

            if (found) {
                long patch_pos = file_pos + i + PATCH_OFFSET_IN_PATTERN;
								printf("[+] Protection code found at physical offset: 0x%lX\n", patch_pos);

                fseek(f, patch_pos, SEEK_SET);

                fwrite(patch, 1, PATCH_LEN, f);

								printf("[+] Version identified and successfully patched!\n");
                break;
            }
        }

        if (found) break;

        if (bytes_read == BUFFER_SIZE) {
            fseek(f, file_pos + BUFFER_SIZE - PATTERN_LEN, SEEK_SET);
        }
    }

    if (!found) {
				printf("[-] Error: Signature not found. Unknown version or already patched.\n");
    }

    fclose(f);
    free(buf);
    return 0;
}