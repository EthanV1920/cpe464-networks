/* Filename: trace.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This is the main file for the trace file.
 */

// Includes
#include "../inc/checksum.h"
#include "../inc/ethernet_parse.h"
#include "../inc/net_structs.h"
#include "pcap.h"
#include <stdio.h>

// Define
#define debug 1
#define db(msg) debug ? printf("INFO: %s\n", msg) : printf("")

int main(int argc, char *argv[]) {
  char errbuf[PCAP_ERRBUF_SIZE];
  struct pcap_pkthdr *header;
  const unsigned char *data;

  // Path is relitive to the root since that is where we run from
  pcap_t *handle = pcap_open_offline(argv[1], errbuf);
  // db(errbuf);

  int stat;

  struct ethernet_info_t eth_info;
  int i = 1;
  printf("\n");

  while ((stat = pcap_next_ex(handle, &header, &data)) >= 0) {
    printf("Packet number: %d  Packet Len: %d\n", i++, header->len);
    printf("\n");
    eth_info = get_eth_info(data);
    printf("\n");
    }
}

