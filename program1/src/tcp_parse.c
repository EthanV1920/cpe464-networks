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

  int buf_size = sizeof(struct tcp_ps_head) + (ps_head.tcp_len) * 4;
  printf("INFO: ps_head.len = %x\n",ps_head.tcp_len);

  // ps_head.tcp_len = htons(ps_head.tcp_len);

  unsigned short *buf = malloc(buf_size);
  memcpy(buf, &ps_head, sizeof(struct tcp_ps_head));
  memcpy(buf + 12, packet, ps_head.tcp_len * 4);

  printf("\t\tChecksum: 0x%x\n", in_cksum(buf, buf_size));

  int check;
  check = in_cksum(buf, buf_size);
  if (check == 0) {
    printf("\t\tChecksum: Correct (0x%0.4x)\n", ntohs(tcp_head->checksum));
  } else {
    printf("\t\tChecksum: Incorrect (0x%0.4x)\n", ntohs(tcp_head->checksum));
  }
  free(buf);
}
