/* Filename: udp_parse.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              udp header
 */

#include "../inc/udp_parse.h"
#include "stdio.h"
#include <stdint.h>
#include "net/ethernet.h"

void get_udp_info(const unsigned char *packet) {
  struct udp_t *udp_head = (struct udp_t *)packet;

  printf("\tUDP Header\n");
  int src_port = ntohs(udp_head->src_port);
  if (src_port == 53) {
      printf("\t\tSource Port:  DNS\n");

  }else{
      printf("\t\tSource Port:  %d\n", ntohs(udp_head->src_port));
  }

  int dest_port = ntohs(udp_head->dest_port);
  if (dest_port == 53) {
      printf("\t\tDest Port:  DNS\n");

  }else{
      printf("\t\tDest Port:  %d\n", ntohs(udp_head->dest_port));
  }

}
