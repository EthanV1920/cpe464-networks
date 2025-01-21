/* Filename: trace.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This is the main file for the trace file.
 */

// Includes 
#include <stdio.h>
#include "../inc/checksum.h"
#include "../inc/net_structs.h"
#include "../inc/ethernet_parse.h"
#include "pcap.h"

// Define
#define debug 1
#define db(msg) debug ? printf("INFO: %s\n", msg) : printf("")

int main(void) {
  char errbuf[PCAP_ERRBUF_SIZE];
  struct pcap_pkthdr header;

  // Path is relitive to the root since that is where we run from
  pcap_t *handle = pcap_open_offline("test_files/PingTest.pcap", errbuf);
  db(errbuf);

  const u_char *packet = pcap_next(handle, &header);

  struct ethernet_info_t eth_info;
  db("succeeded");
  eth_info = get_eth_info(packet);
  printf("INFO: ret_info: %s\n", eth_info.type);
  printf("INFO: ret_info: %s\n", eth_info.dest_addr_s);
  printf("INFO: ret_info: %s\n", eth_info.src_addr_s);

  return 0;
}
