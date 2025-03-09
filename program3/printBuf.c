/* Filename: printBuf.c
 * Author: Ethan Vosburg
 * Date: March 9, 2025
 * Version: 1.0
 * Description: This program takes a buffer and prints it in hex with text on
 *              the side if there is text.
 *
 */

#include "printBuf.h"

void printBuf(uint8_t *buf, uint16_t len) {
    printf("Length: %d\n", len);

    // Print in groups of 8 bytes per row
    for (int i = 0; i < len; i += 8) {
        // Print row offset
        printf("%04x | ", i);

        // Print hex values
        for (int j = 0; j < 8; j++) {
            if (i + j < len) {
                if (buf[i + j] >= 32 && buf[i + j] <= 127) {
                    printf("\e[38;5;196m%02x\e[0m  ", buf[i + j]);
                } else {
                    printf("%02x  ", buf[i + j]);
                }
            } else {
                printf("    "); // Padding for incomplete final row
            }
        }

        // Print ASCII representation
        printf("| ");
        for (int j = 0; j < 8; j++) {
            if (i + j < len) {
                if (buf[i + j] >= 32 && buf[i + j] <= 127) {
                    printf("\e[38;5;196m%c\e[0m", buf[i + j]);
                } else {
                    printf(".");
                }
            }
        }
        printf("\n");
    }
}
