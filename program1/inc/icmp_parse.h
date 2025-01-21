/* Filename: icmp_parse.h
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              ICMP header.
 */

#include "stdio.h"
#include <stdint.h>

struct icmp_t {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  uint32_t roh;
} __attribute__((packed));

struct ethernet_info_t {
    char dest_addr_s[18];
    char src_addr_s[18];
    char type[5];
};

void get_icmp_info(const unsigned char *packet);

