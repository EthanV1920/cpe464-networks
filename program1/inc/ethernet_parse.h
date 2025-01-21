/* Filename: ethernet_parse.h
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              ethernet header.
 */

#include "stdio.h"
#include <stdint.h>

struct ethernet_t {
  uint8_t dest_addr[6];
  uint8_t src_addr[6];
  uint16_t type;
} __attribute__((packed));

struct ethernet_info_t {
    char dest_addr_s[18];
    char src_addr_s[18];
    char type[8];
};

struct ethernet_info_t get_eth_info(const unsigned char *packet);

