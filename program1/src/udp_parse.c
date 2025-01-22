/* Filename: udp_parse.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              udp header
 */

#include "../inc/udp_parse.h"
#include "net/ethernet.h"
#include "stdio.h"
#include <stdint.h>

void get_udp_info(const unsigned char *packet) {
  struct udp_t *udp_head = (struct udp_t *)packet;

  printf("\tUDP Header\n");
  int src_port = ntohs(udp_head->src_port);
  switch (src_port) {
  case 53:
    printf("\t\tSource Port:  DNS\n");
    break;
  case 23:
    printf("\t\tSource Port:  Telnet\n");
    break;
  case 80:
    printf("\t\tSource Port:  HTTP\n");
    break;
  case 20:
    printf("\t\tSource Port:  FTP\n");
    break;
  case 110:
    printf("\t\tSource Port:  POP3\n");
    break;
  case 25:
    printf("\t\tSource Port:  SMTP\n");
    break;
  default:
    printf("\t\tSource Port:  %d\n", ntohs(udp_head->src_port));
    break;
  }

  int dest_port = ntohs(udp_head->dest_port);
  switch (dest_port) {
  case 53:
    printf("\t\tDest Port:  DNS\n");
    break;
  case 23:
    printf("\t\tDest Port:  Telnet\n");
    break;
  case 80:
    printf("\t\tDest Port:  HTTP\n");
    break;
  case 20:
    printf("\t\tDest Port:  FTP\n");
    break;
  case 110:
    printf("\t\tDest Port:  POP3\n");
    break;
  case 25:
    printf("\t\tDest Port:  SMTP\n");
    break;
  default:
    printf("\t\tDest Port:  %d\n", ntohs(udp_head->dest_port));
    break;
  }
}
