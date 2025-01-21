/* Filename: ip_parse.h
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              ip header.
 */

#include "stdio.h"
#include <stdint.h>

struct ip_t {
    uint8_t ihl : 4;
    uint8_t version : 4;
    uint8_t ecn : 2;
    uint8_t dscp : 6;
    uint16_t total_len;
    uint16_t id;
    uint8_t flags : 3;
    uint16_t frag_ofst : 13;
    uint8_t ttl;
    uint8_t proto;
    uint16_t hdr_check;
    uint32_t src_addr;
    uint32_t dest_addr;
} __attribute__((packed));

// struct ip_info_t {
//     char dest_addr_s[18];
//     char src_addr_s[18];
//     char type[5];
// };

void get_ip_info(const unsigned char *packet);
