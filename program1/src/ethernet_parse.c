/* Filename: ethernet_parse.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file has the code for the etherent parsing
 */

#include "../inc/ethernet_parse.h"
#include "string.h"

struct ethernet_info_t get_eth_info(const unsigned char *packet) {
  // struct ethernet_t *eth_head = (struct ethernet_t *)packet;
  struct ethernet_info_t ret_info;

  char input_name[] = "HELLO";
  strncpy(ret_info.type, input_name, strlen(input_name));
  // strncpy(ret_info.type, input_name, sizeof(ret_info.type) - 1);
  // ret_info.type[sizeof(ret_info.type) - 1] = '\0';

  char *mac_addr = "11:22:33:44:55:66";
  strncpy(ret_info.dest_addr_s, mac_addr, strlen(mac_addr));
  strncpy(ret_info.src_addr_s, mac_addr, strlen(mac_addr));

  struct ethernet_t *eth_head = (struct ethernet_t *)packet;

  int len = 6;

  printf("INFO: destination addr: ");
  while (len--) {
    printf("%x:", (eth_head->dest_addr[5 - len]));
  }
  printf("\b \n");

  len = 6;
  printf("INFO: source addr: ");
  while (len--) {
    printf("%x:", (eth_head->src_addr[5 - len]));
  }
  printf("\b \n");

  printf("INFO: Type: %X\n", (eth_head->type));

  if(eth_head->type == 0x8){
      printf("INFO: IP\n");

  }

  return ret_info;
}
