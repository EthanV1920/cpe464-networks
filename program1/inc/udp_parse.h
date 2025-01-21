/* Filename: udp_parse.h
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              udp header.
 */

#include "stdio.h"
#include <stdint.h>

struct udp_t {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t len;
    uint16_t checksum;
} __attribute__((packed));


void get_udp_info(const unsigned char *packet);

