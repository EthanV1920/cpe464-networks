/* Filename: net_struct.h
 * Author: Ethan Vosburg
 * Date: March 10, 2025
 * Version: 1.0
 * Description: This header file defines network sturcts that are useful for
 *              rcopy
 *
 *              - setupInfo_t: carries the connection information for a device
 *              - header_t: carries the header info for our rcopy program
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef NET_STRUCTS_H
#define NET_STRUCTS_H

typedef struct {
    char fromFileName[100];
    char toFileName[100];
    uint32_t windowSize;
    uint16_t bufferSize;
    float errorRate;
    char remoteMachine[64];
    int remotePort;
    int socketNum;
    struct sockaddr_in6 *server;

} setupInfo_t;

typedef struct {
    uint32_t sequenceNum;
    uint16_t checksum;
    uint8_t flag;

} __attribute__((packed)) header_t;

#endif
