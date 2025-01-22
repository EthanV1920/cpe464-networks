/* Filename: ethernet_parse.h
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              TCP header.
 */

#include "stdio.h"
#include <stdint.h>

struct tcp_t {
  uint16_t src_port;
  uint16_t dest_port;
  uint32_t seq_num;
  uint32_t ack;
  uint8_t res : 4;
  uint8_t data_ofst : 4;
  uint8_t fin_flag : 1;
  uint8_t syn_flag : 1;
  uint8_t rst_flag : 1;
  uint8_t psh_flag : 1;
  uint8_t ack_flag : 1;
  uint8_t urg_flag : 1;
  uint8_t ece_flag : 1;
  uint8_t cwr_flag : 1;
  uint16_t window;
  uint16_t checksum;
  uint16_t ugnt_point;
} __attribute__((packed));

struct tcp_ps_head {
  uint32_t src_addr;
  uint32_t dest_addr;
  uint8_t zeros;
  uint8_t proto;
  uint16_t tcp_len;
} __attribute__((packed));
// struct tcp_ps_head {
//   uint16_t tcp_len;
//   uint8_t proto;
//   uint8_t zeros;
//   uint32_t dest_addr;
//   uint32_t src_addr;
// } __attribute__((packed));


void get_tcp_info(const unsigned char *packet, struct tcp_ps_head ip_check);
