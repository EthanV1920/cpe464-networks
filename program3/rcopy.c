/* Filename: rcopy.c
 * Author: Ethan Vosburg
 * Date: February 28, 2025
 * Version: 1.0
 * Description: This program interacts with a server to request a file and then
 *              facilitate the delivery of that file to the client.
 *
 *              These are the arguments that are taken, it is assumed that the
 *              user will always input the correct arguments so there is little
 *              validation.
 *
 *              - from-filename     file to read from
 *              - to-filename       file to write to
 *              - window-size       number of packets in a window
 *              - buffer-size       bytes in a packet
 *              - error-rate        rate of error in transmission
 *              - remote-machine    ip of server
 *              - remote-port       port of server
 */

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "checksum.h"
#include "docs/libcpe464_2_21b/libcpe464/networks/network-hooks.h"
#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"

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

#define FROM_FILENAME 1
#define TO_FILENAME 2
#define WINDOW_SIZE 3
#define BUFFER_SIZE 4
#define ERROR_RATE 5
#define REMOTE_MACHINE 6
#define REMOTE_PORT 7
#define MAXBUF 80

void talkToServer(setupInfo_t *setupInfo);
int readFromStdin(char *buffer);
void processArgs(int argc, char *argv[], setupInfo_t *setupInfo);
int connectBuf(setupInfo_t *setupInfo);
void printBuf(uint8_t *buf, uint16_t len);
void sendData(char *buf, uint16_t bufLen, setupInfo_t *setupInfo);

int main(int argc, char *argv[]) {
    int socketNum = 0;
    struct sockaddr_in6 server; // Supports 4 and 6 but requires IPv6 struct
    setupInfo_t setupInfo;

    processArgs(argc, argv, &setupInfo);
    sendErr_init(setupInfo.errorRate, DROP_ON, FLIP_OFF, DEBUG_ON, RSEED_OFF);

    printf("DEBUG: SetupInfo %d %d\n", setupInfo.bufferSize,
           setupInfo.windowSize);
    printBuf((uint8_t *)&setupInfo, sizeof(setupInfo_t));
    socketNum = setupUdpClientToServer(&server, setupInfo.remoteMachine,
                                       setupInfo.remotePort);
    setupInfo.socketNum = socketNum;
    setupInfo.server = &server;
    connectBuf(&setupInfo);

    // TODO: add a way to send data better
    // sendData(buf, bufferSize, setupInfo);

    talkToServer(&setupInfo);

    close(socketNum);

    return 0;
}

void talkToServer(setupInfo_t *setupInfo) {
    int serverAddrLen = sizeof(struct sockaddr_in6);
    char *ipString = NULL;
    int dataLen = 0;
    char buffer[MAXBUF + 1];

    buffer[0] = '\0';
    while (buffer[0] != '.') {
        dataLen = readFromStdin(buffer);

        printf("Sending: %s with len: %d\n", buffer, dataLen);

        printBuf((uint8_t *)buffer, dataLen);

        // sendtoErr(setupInfo->socketNum, buffer, dataLen, 0,
        //           (struct sockaddr *)setupInfo->server, serverAddrLen);

        sendData(buffer, dataLen, setupInfo);

        safeRecvfrom(setupInfo->socketNum, buffer, MAXBUF, 0,
                     (struct sockaddr *)setupInfo->server, &serverAddrLen);

        // print out bytes received
        ipString = ipAddressToString(setupInfo->server);
        printf("Server with ip: %s and port %d said it received %s\n", ipString,
               ntohs(setupInfo->server->sin6_port), buffer);
    }
}

int readFromStdin(char *buffer) {
    char aChar = 0;
    int inputLen = 0;

    // Important you don't input more characters than you have space
    buffer[0] = '\0';
    printf("Enter data: ");
    while (inputLen < (MAXBUF - 1) && aChar != '\n') {
        aChar = getchar();
        if (aChar != '\n') {
            buffer[inputLen] = aChar;
            inputLen++;
        }
    }

    // Null terminate the string
    buffer[inputLen] = '\0';
    inputLen++;

    return inputLen;
}

void processArgs(int argc, char *argv[], setupInfo_t *setupInfo) {
    /* check command line arguments  */
    if (argc != 8) {
        printf("usage: %s from-filename to-filename window-size buffer-size"
               "error-rate remote-machine remote-port\n\n"

               "	- from-filename     file to read from\n"
               "	- to-filename       file to write to\n"
               "	- window-size       number of packets in a window\n"
               "	- buffer-size       bytes in a packet\n"
               "	- error-rate        rate of error in transmission\n"
               "	- remote-machine    ip of server\n"
               "	- remote-port       port of server\n",
               argv[0]);
        exit(1);
    }

    strncpy(setupInfo->fromFileName, argv[FROM_FILENAME], 100);
    strncpy(setupInfo->toFileName, argv[TO_FILENAME], 100);
    setupInfo->windowSize = atoi(argv[WINDOW_SIZE]);
    setupInfo->bufferSize = atoi(argv[BUFFER_SIZE]);
    setupInfo->errorRate = atof(argv[ERROR_RATE]);
    strncpy(setupInfo->remoteMachine, argv[REMOTE_MACHINE], 64);
    setupInfo->remotePort = atoi(argv[REMOTE_PORT]);

    // int setupInfoSz = 100 + 100 + 4 + 2 + 4 + 64 + 2;
    // printBuf((uint8_t *)&setupInfo, setupInfoSz);

    printf("DEBUG: %s %s %d %d %f %s %d\n", setupInfo->fromFileName,
           setupInfo->toFileName, setupInfo->windowSize, setupInfo->bufferSize,
           setupInfo->errorRate, setupInfo->remoteMachine,
           setupInfo->remotePort);
}

/**
 * This function create the packet that is needed to connect to the server:
 * - sequence number: 0000
 * - checksum
 * - flag: 8
 * - from filename
 * - window size
 * - buffer size
 * @param setupInfo_t *setupInfo, connection information
 * @param int socketNum, socket to send data to
 * @param struct sockaddr_in6 *server, the ip of the server to talk to
 */
int connectBuf(setupInfo_t *setupInfo) {
    uint8_t filenameLen = strlen(setupInfo->fromFileName);
    uint16_t bufferSize = 4 + 2 + 1 + filenameLen + 2 + 4;
    char buf[bufferSize];

    // Set sequence number to 0
    memset(buf, 0x0, 4);

    // Set checksum to 0 to calculate checksum
    memset(buf + 4, 0x0, 2);

    // Set f.no flag
    buf[6] = 0x8;

    // Set filename
    memcpy(buf + 7, setupInfo->fromFileName, filenameLen);

    // Set windowSize
    uint32_t translatedWinSz = htonl(setupInfo->windowSize);
    memcpy(buf + 7 + filenameLen, (char *)&translatedWinSz, sizeof(uint32_t));

    // Set bufferSize
    uint32_t translatedBufSz = htonl(setupInfo->bufferSize);
    memcpy(buf + 11 + filenameLen, (char *)&translatedBufSz, sizeof(uint16_t));

    // int serverAddrLen = sizeof(struct sockaddr_in6);
    // sendtoErr(setupInfo->socketNum, buf, bufferSize, 0,
              // (struct sockaddr *)setupInfo->server, serverAddrLen);

    sendData(buf, bufferSize, setupInfo);

    return bufferSize;
}

void printBuf(uint8_t *buf, uint16_t len) {
    printf("Length: %d\n", len);

    // Print in groups of 8 bytes per row
    for (int i = 0; i < len; i += 8) {
        // Print row offset
        printf("%04x | ", i);

        // Print hex values
        for (int j = 0; j < 8; j++) {
            if (i + j < len) {
                if (buf[i + j] >= 32 && buf[i + j] <= 127) {
                    printf("\e[38;5;196m%02x\e[0m  ", buf[i + j]);
                } else {
                    printf("%02x  ", buf[i + j]);
                }
            } else {
                printf("    "); // Padding for incomplete final row
            }
        }

        // Print ASCII representation
        printf("| ");
        for (int j = 0; j < 8; j++) {
            if (i + j < len) {
                if (buf[i + j] >= 32 && buf[i + j] <= 127) {
                    printf("\e[38;5;196m%c\e[0m", buf[i + j]);
                } else {
                    printf(".");
                }
            }
        }
        printf("\n");
    }
}
/**
 * This function calculates the checksum and then sends the data packet
 * @param uint8_t *buf, Data buffer to send
 * @param uint16_t bufLen, Length of the buffer to send
 * @param
 */
void sendData(char *buf, uint16_t bufLen, setupInfo_t *setupInfo) {

    // TODO: need to add connection info
    // Calculate checksum
    uint16_t checksum = 0;
    int serverAddrLen = sizeof(struct sockaddr_in6);
    checksum = htons(in_cksum((ushort *)buf, bufLen));
    printf("INFO: checksum = %d\n", checksum);

    printBuf((uint8_t *)buf, bufLen);
    memcpy(buf + 4, &checksum, sizeof(uint16_t));
    printBuf((uint8_t *)buf, bufLen);

    // printf("DEBUG: Setting up connection to server %d %d \n",
    //        setupInfo->bufferSize, setupInfo->windowSize);

    // printf("INFO: Connection bufferSize: %d\n", bufLen);
    // printBuf((uint8_t *)buf, bufLen);
    // printf("DEBUG: Sending Data... %d\n", setupInfo->socketNum);

    char *ipString = NULL;
    ipString = ipAddressToString(setupInfo->server);

    printf("Server with ip: %s and port %d said it received %s\n", ipString,
           ntohs(setupInfo->server->sin6_port), buf);
    printf("Sending Data Return: %zd\n",
           sendtoErr(setupInfo->socketNum, buf, bufLen, 0,
                     (struct sockaddr *)setupInfo->server, serverAddrLen));

    // sendtoErr(setupInfo->socketNum, buffer, dataLen, 0,
    //           (struct sockaddr *)setupInfo->server, serverAddrLen);

    // printf("DEBUG: Sending Data... %d\n", setupInfo.remotePort);
    // int serverAddrLen = sizeof(struct sockaddr_in6);
    // printf("Sending Data Return: %zd\n",
    //        sendtoErr(setupInfo.remotePort, buf, bufLen, 0,
    //                  (struct sockaddr *)setupInfo.remoteMachine,
    //                  serverAddrLen));
}
