/* Filename: sendDate.c
 * Author: Ethan Vosburg
 * Date: February 28, 2025
 * Version: 1.0
 * Description: This program sends data and prints out the information that it
 *              sends.
 */

#include "sendData.h"

/**
 * This function calculates the checksum and then sends the data packet
 * @param uint8_t *buf, Data buffer to send
 * @param uint16_t bufLen, Length of the buffer to send
 * @param
 */
void sendData(char *buf, uint16_t bufLen, uint32_t sequence, uint8_t flag,
              setupInfo_t *setupInfo) {
    char sendBuf[bufLen + 7];

    // Set sequence number
    uint32_t networkSequence = htonl(sequence);
    memcpy(sendBuf, &networkSequence, 4);

    // Set checksum to 0 to calculate checksum
    memset(sendBuf + 4, 0x0, 2);

    // Set flag
    sendBuf[6] = flag;

    // Append the rest of the data
    memcpy(sendBuf + 7, buf, bufLen);

    // Calculate checksum
    uint16_t checksum = 0;
    int serverAddrLen = sizeof(struct sockaddr_in6);
    checksum = htons(in_cksum((ushort *)sendBuf, bufLen + 7));
    printf("INFO: checksum = %d\n", checksum);

    memcpy(sendBuf + 4, &checksum, sizeof(uint16_t));
    printBuf((uint8_t *)sendBuf, bufLen + 7);

    char *ipString = NULL;
    ipString = ipAddressToString(setupInfo->server);

    printf("Server with ip: %s and port %d said it received %s\n", ipString,
           ntohs(setupInfo->server->sin6_port), sendBuf);

    printf("Sending Data Return: %zd\n",
           sendtoErr(setupInfo->socketNum, sendBuf, bufLen + 7, 0,
                     (struct sockaddr *)setupInfo->server, serverAddrLen));
}
