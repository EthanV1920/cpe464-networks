/* Filename: ethernet_parse.c
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file has the code for the etherent parsing
 */

#include "../inc/ethernet_parse.h"
#include "../inc/arp_parse.h"
#include "../inc/ip_parse.h"
#include "stdlib.h"
#include "string.h"
#include "net/ethernet.h"

struct ethernet_info_t get_eth_info(const unsigned char *packet) {
    struct ethernet_t *eth_head = (struct ethernet_t *)packet;
    struct ethernet_info_t ret_info;

    struct ether_addr mac;

    printf("\tEthernet Header\n");
    memcpy(&mac, &eth_head->dest_addr, sizeof(eth_head->dest_addr));
    printf("\t\tDest MAC: %s\n", ether_ntoa(&mac));

    memcpy(&mac, &eth_head->src_addr, sizeof(eth_head->src_addr));
    printf("\t\tSource MAC: %s\n", ether_ntoa(&mac));

    switch (ntohs(eth_head->type)) {
        case 0x0800:
            sprintf(ret_info.type, "IP");
            printf("\t\tType: IP\n");
            printf("\n");
            get_ip_info(packet + sizeof(struct ethernet_t));
            break;
        case 0x0806:
            sprintf(ret_info.type, "ARP");
            printf("\t\tType: ARP\n");
            printf("\n");
            get_arp_info(packet + sizeof(struct ethernet_t));
            break;
        default:
            sprintf(ret_info.type, "NONE");
            printf("\t\tType: Unknown\n");
            break;
    }

    return ret_info;
}
