/* Filename: ip_parse.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              ip header.
 */

#include "../inc/ip_parse.h"
#include "../inc/udp_parse.h"
#include "../inc/checksum.h"
#include "../inc/icmp_parse.h"
#include "../inc/tcp_parse.h"
#include "arpa/inet.h"
#include "net/ethernet.h"
#include "stdio.h"
#include <stdint.h>

void get_ip_info(const unsigned char *packet) {
  struct ip_t *ip_head = (struct ip_t *)packet;

  printf("\tIP Header\n");
  printf("\t\tIP Version: %d\n", ip_head->version);
  // switch ((ip_head->version)) {
  // case 0x4:
  //   printf("\t\tIP Version: 4\n");
  //   break;
  // case 0x6:
  //   printf("\t\tIP Version: 11\n");
  //   break;
  // default:
  //   break;
  // }
  printf("\t\tHeader Len (bytes): %d\n", ip_head->ihl * 4);

  printf("\t\tTOS subfields:\n");
  printf("\t\t   Diffserv bits: %d\n", (ip_head->dscp));
  printf("\t\t   ECN bits: %d\n", ip_head->ecn);

  printf("\t\tTTL: %d\n", ip_head->ttl);

  switch (ip_head->proto) {
  case 0x1:
    printf("\t\tProtocol: ICMP\n");
    break;
  case 0x6:
    printf("\t\tProtocol: TCP\n");
    break;
  case 0x11:
    printf("\t\tProtocol: UDP\n");
    break;
  default:
    printf("\t\tProtocol: Unknown\n");
    break;
  }

  int check;
  check = in_cksum((unsigned short *)(packet), ip_head->ihl * 4);
  if (check == 0) {
    printf("\t\tChecksum: Correct (0x%0.4x)\n", ntohs(ip_head->hdr_check));
  } else {
    printf("\t\tChecksum: Incorrect (0x%0.4x)\n", ntohs(ip_head->hdr_check));
  }

  char send_ip[32] = {0};
  inet_ntop(AF_INET, &ip_head->src_addr, send_ip, 32);
  printf("\t\tSender IP: %s\n", send_ip);

  char dest_ip[32] = {0};
  inet_ntop(AF_INET, &ip_head->dest_addr, dest_ip, 32);
  printf("\t\tDest IP: %s\n", dest_ip);

  switch (ip_head->proto) {
  case 0x1:
    // printf("\t\tProtocol: ICMP\n");
    printf("\n");
    get_icmp_info(packet + (ip_head->ihl * 4));
    break;
  case 0x6:
    // printf("\t\tProtocol: TCP\n");
    printf("\n");

    struct tcp_ps_head ps_head;

    ps_head.src_addr = ip_head->src_addr;
    ps_head.dest_addr = ip_head->dest_addr;
    ps_head.zeros = 0x00000000;
    ps_head.proto = ip_head->proto;
    ps_head.tcp_len = ntohs(ip_head->total_len) - (ip_head->ihl * 4);
    printf("%d   %d\n", ntohs(ip_head->total_len), ip_head->ihl);

    get_tcp_info(packet + (ip_head->ihl * 4), ps_head);
    break; 
  case 0x11:
    // printf("\t\tProtocol: UDP\n");
    printf("\n");
    get_udp_info(packet + (ip_head->ihl * 4));
    break;
  default:
    break;
  }
}
