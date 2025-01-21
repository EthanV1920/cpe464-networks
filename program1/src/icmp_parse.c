/* Filename: icmp_parse.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              ICMP header.
 */

#include "../inc/icmp_parse.h"
#include "stdio.h"

void get_icmp_info(const unsigned char *packet) {
  struct icmp_t *icmp_head = (struct icmp_t *)packet;

  printf("\tICMP Header\n");
  switch (icmp_head->type) {
  case 0x0:
    printf("\t\tType: Reply\n");
    break;
  case 0x8:
    printf("\t\tType: Request\n");
    break;
  default:
    printf("\t\tType: %d\n", icmp_head->type);
    break;
  }
}
