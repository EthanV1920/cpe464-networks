/* Filename: tcp_parse.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              TCP header.
 */

#include "../inc/tcp_parse.h"
#include "../inc/checksum.h"
#include "arpa/inet.h"
#include "stdio.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void get_tcp_info(const unsigned char *packet, struct tcp_ps_head ps_head) {
  struct tcp_t *tcp_head = (struct tcp_t *)packet;

  printf("\tTCP Header\n");
  printf("\t\tSource Port:  %d\n", ntohs(tcp_head->src_port));
  printf("\t\tDest Port:  %d\n", ntohs(tcp_head->dest_port));
  printf("\t\tSequence Number: %ld\n", ntohl(tcp_head->seq_num));
  printf("\t\tACK Number: %ld\n", ntohl(tcp_head->ack));
  printf("\t\tData Offset (bytes): %d\n", (tcp_head->data_ofst) * 4);
  if (tcp_head->syn_flag) {
    printf("\t\tSYN Flag: Yes\n");
  } else {
    printf("\t\tSYN Flag: No\n");
  }

  if (tcp_head->rst_flag) {
    printf("\t\tRST Flag: Yes\n");
  } else {
    printf("\t\tRST Flag: No\n");
  }

  if (tcp_head->fin_flag) {
    printf("\t\tFIN Flag: Yes\n");
  } else {
    printf("\t\tFIN Flag: No\n");
  }

  if (tcp_head->ack_flag) {
    printf("\t\tACK Flag: Yes\n");
  } else {
    printf("\t\tACK Flag: No\n");
  }

  printf("\t\tWindow Size: %ld\n", ntohs(tcp_head->window));

  int buf_size = 12 + (ps_head.tcp_len);
  // ps_head.tcp_len = ntohs(ps_head.tcp_len);
  // ps_head.tcp_len = 48;

  printf("INFO: buf_size = %d\n", buf_size);

  unsigned char *ps_head_buffer = (unsigned char *)malloc(12);

  memcpy(ps_head_buffer, &ps_head.src_addr, 4);
  memcpy(ps_head_buffer + 4, &ps_head.dest_addr, 4);
  memcpy(ps_head_buffer + 8, &ps_head.zeros, 1);
  memcpy(ps_head_buffer + 9, &ps_head.proto, 1);
  memcpy(ps_head_buffer + 10, &ps_head.tcp_len, 2);
  // TODO: Fix TCP Checksum
  //
  // printf("INFO: ps_head_buffer: ");
  // for(int i = 0; i < 12; i++) {
  //   printf("0x%02x ", ps_head_buffer[i]);
  // }
  // printf("\n");

  // printf("INFO: packet: ");
  // for(int i = 0; i < 36; i++) {
  //   printf("0x%02x ", packet[i]);
  // }
  // printf("\n");



  unsigned char *buf = malloc(buf_size);
  memcpy(buf, ps_head_buffer, 12);
  memcpy(buf + 12, packet, buf_size - 12);

  printf("\t\tChecksum: 0x%x\n", in_cksum((uint16_t *)buf, buf_size));
  // printf("\t\tPH: 0x%lx\n", &ps_head_buffer);

  printf("INFO: buf: ");
  for(int i = 0; i < buf_size; i++) {
    printf("0x%02x ", buf[i]);
  }
  printf("\n");

  int check;
  check = in_cksum((uint16_t *)buf, buf_size);
  if (check == 0) {
    printf("\t\tChecksum: Correct (0x%0.4x)\n", ntohs(tcp_head->checksum));
  } else {
    printf("\t\tChecksum: Incorrect (0x%0.4x)\n", ntohs(tcp_head->checksum));
  }
  free(buf);
}
