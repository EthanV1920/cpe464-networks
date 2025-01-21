/* Filename: ethernet_parse.h
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              ethernet header.
 */

#include "stdio.h"
#include <stdint.h>

struct arp_t {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t hw_len;
    uint8_t proto_len;
    uint16_t op;
    uint8_t sha[6];
    uint32_t spa;
    uint8_t tha[6];
    uint32_t tpa;
} __attribute__((packed));

// struct arp_info_t {
//     char dest_addr_s[18];
//     char src_addr_s[18];
//     char type[5];
// };

// struct arp_info_t get_arp_info(const unsigned char *packet);
void get_arp_info(const unsigned char *packet);

