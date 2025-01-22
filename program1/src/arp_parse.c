/* Filename: arp_parse.h
 * Author: Ethan Vosburg
 * Date: January 17, 2025
 * Version: 1.0
 * Description: This file defines all of the functions needed to deal with an
 *              arp header.
 */

#include "../inc/arp_parse.h"
#include "arpa/inet.h"
#include "net/ethernet.h"
#include "stdio.h"
#include <stdint.h>
#include <string.h>

void get_arp_info(const unsigned char *packet) {
    struct arp_t *arp_head = (struct arp_t *)packet;

    struct ether_addr send_mac;
    memcpy(&send_mac, &arp_head->sha, sizeof(arp_head->sha));

    struct ether_addr target_mac;
    memcpy(&target_mac, &arp_head->tha, sizeof(arp_head->tha));

    char send_ip[32] = {0};
    inet_ntop(AF_INET, &arp_head->spa, send_ip, 32);

    char target_ip[32] = {0};
    inet_ntop(AF_INET, &arp_head->tpa, target_ip, 32);

    printf("\tARP header\n");

    switch (ntohs(arp_head->op)) {
        case 1:
            printf("\t\tOpcode: Request\n");
            break;
        case 2:
            printf("\t\tOpcode: Reply\n");
            break;
    }

    printf("\t\tSender MAC: %s\n", ether_ntoa(&send_mac));
    printf("\t\tSender IP: %s\n", send_ip);
    printf("\t\tTarget MAC: %s\n", ether_ntoa(&target_mac));
    printf("\t\tTarget IP: %s\n", target_ip);
    printf("\n");
}
