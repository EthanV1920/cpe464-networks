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
#include "pollLib.h"
#include "printBuf.h"
#include "safeUtil.h"
#include "sendData.h"

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

int main(int argc, char *argv[]) {
#ifdef DEBUG
    printf("DEBUGGING ON\n");
#endif
    int socketNum = 0;
    struct sockaddr_in6 server; // Supports 4 and 6 but requires IPv6 struct
    setupInfo_t setupInfo;

    processArgs(argc, argv, &setupInfo);
#ifdef DEBUG
    sendErr_init(setupInfo.errorRate, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);
#else
    sendErr_init(setupInfo.errorRate, DROP_ON, FLIP_ON, DEBUG_OFF, RSEED_ON);
#endif

#ifdef DEBUG
    printf("DEBUG: SetupInfo %d %d\n", setupInfo.bufferSize,
           setupInfo.windowSize);
#endif
    // printBuf((uint8_t *)&setupInfo, sizeof(setupInfo_t));
    socketNum = setupUdpClientToServer(&server, setupInfo.remoteMachine,
                                       setupInfo.remotePort);
    /// Set up polling ///
    setupPollSet();
    addToPollSet(socketNum);

    setupInfo.socketNum = socketNum;
    setupInfo.server = &server;

    connectBuf(&setupInfo);

    talkToServer(&setupInfo);

    close(socketNum);

    return 0;
}

void talkToServer(setupInfo_t *setupInfo) {
    int serverAddrLen = sizeof(struct sockaddr_in6);
    char *ipString = NULL;
    int dataLen = 0;
    char buf[MAXBUF + 1];
    int pollResult = 0;

    struct sockaddr_in6 client;
    header_t *udpHeader;
    int clientAddrLen = sizeof(client);
    char fileName[101];
    uint8_t pidCount = 0;
    int pid = -1;
    uint32_t rr = 1;

    buf[0] = '\0';
    while (pollCall(1000) != -1) {
        // pollResult = pollCall(-1);
        //
        // Available message in stdin
        if (pollResult == STDIN_FILENO) {
            // printf("INFO: POLL RESULT\n");
            // processStdin(clientSocket);

        } else if (pollResult == setupInfo->socketNum) {
            // printf("INFO: POLL CLOSED\n");
            // processMsgFromServer(clientSocket);    // Terminate client socket
        }

        printf("Sending: %s with len: %d\n", buf, dataLen);

        printBuf((uint8_t *)buf, dataLen);

        // sendData(buf, dataLen, 0, 16, setupInfo);
        // connectBuf(setupInfo);

        dataLen =
            safeRecvfrom(setupInfo->socketNum, buf, MAXBUF, 0,
                         (struct sockaddr *)setupInfo->server, &serverAddrLen);

        // NOTE:Make sure you check before you start modifying
        verifyData((uint8_t *)buf, dataLen, udpHeader->checksum);

        // Print what is received from the server
        printBuf((uint8_t *)buf, dataLen);

        // print out bytes received
        // ipString = ipAddressToString(setupInfo->server);
        // printf("Server with ip: %s and port %d said it received %s\n",
        // ipString,
        //        ntohs(setupInfo->server->sin6_port), buf);

        // dataLen = safeRecvfrom(socketNum, buf, MAXBUF, 0,
        //                        (struct sockaddr *)&client, &clientAddrLen);

        udpHeader = (header_t *)buf;



        udpHeader->sequenceNum = ntohl(udpHeader->sequenceNum);

        // setupInfo->server = &client;
        // setupInfo->socketNum = socketNum;

#ifdef DEBUG
        printf("DEBUG: Flag Value: %x\n", udpHeader->flag);
        printf("DEBUG: Checksum Value: %x\n", ntohl(udpHeader->checksum));
        printf("DEBUG: Sequence Value: %x\n", udpHeader->sequenceNum);
#endif

        // printf("Received message from client with ");
        // printIPInfo(&client);
        // printIPInfo(setupInfo->server);
        // printBuf((uint8_t *)&buf, dataLen);
        // printf(" Len: %d \'%s\'\n", dataLen, buf);

#ifdef DEBUG
        printf("DEBUG: rcopy flag %d\n", udpHeader->flag);
#endif
        switch (udpHeader->flag) {
        case 5: // RR Packet
            break;
        case 6: // SREJ Packet
            break;
        case 8: // Connect from client to server
            break;
        case 9: // Connect response from server

            break;
        case 10: // Packet is EOF last packet
            break;
        case 16: // Regular data packet
            printBuf((uint8_t *)buf, dataLen);
            rr = udpHeader->sequenceNum;
            // if (rr - 1 == udpHeader->sequenceNum) {
            // }
#ifdef DEBUG
            printf("DEBUG: RR number sent = %d\n", rr);
#endif
            char rrBuf[4];
            rr = htonl(rr);
            memcpy(rrBuf, &rr, sizeof(uint32_t));
            sendData((char *)rrBuf, 4, 0, 5, setupInfo);
            break;
        case 17: // Resent data packet after SREJ
            break;
        case 18: // Resent data packet after timeout
            break;
        default:
            printf("INFO: Flag not found %d\n", udpHeader->flag);
            break;
        }
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

#ifdef DEBUG
    printf("DEBUG: %s %s %d %d %f %s %d\n", setupInfo->fromFileName,
           setupInfo->toFileName, setupInfo->windowSize, setupInfo->bufferSize,
           setupInfo->errorRate, setupInfo->remoteMachine,
           setupInfo->remotePort);
#endif
}

/**
 * This function create the packet that is needed to connect to the server:
 * - sequence number: 0000
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
    uint16_t bufferSize = filenameLen + 2 + 4;
    char buf[bufferSize];

#ifdef DEBUG
    printf("DEBUG: Sending connection buffer\n");
#endif

    // Set filename
    memcpy(buf, setupInfo->fromFileName, filenameLen);

    // Set windowSize
    uint32_t translatedWinSz = htonl(setupInfo->windowSize);
    memcpy(buf + filenameLen, (char *)&translatedWinSz, sizeof(uint32_t));

    // Set bufferSize
    uint16_t translatedBufSz = htons(setupInfo->bufferSize);
    memcpy(buf + 4 + filenameLen, (char *)&translatedBufSz, sizeof(uint16_t));

    sendData(buf, bufferSize, 0, 8, setupInfo);

    return bufferSize;
}
