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

#include "docs/libcpe464_2_21b/libcpe464/networks/network-hooks.h"
#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"

typedef struct {
    char fromFileName[100];
    char toFileName[100];
    uint16_t windowSize;
    uint16_t bufferSize;
    float errorRate;
    char remoteMachine[64];
    int remotePort;

} setupInfo_t;

#define FROM_FILENAME 1
#define TO_FILENAME 2
#define WINDOW_SIZE 3
#define BUFFER_SIZE 4
#define ERROR_RATE 5
#define REMOTE_MACHINE 6
#define REMOTE_PORT 7
#define MAXBUF 80

void talkToServer(int socketNum, struct sockaddr_in6 *server);
int readFromStdin(char *buffer);
setupInfo_t processArgs(int argc, char *argv[]);
int connectString(setupInfo_t setupInfo);
void printBuf(uint8_t *buf, uint16_t len);


int main(int argc, char *argv[]) {
    int socketNum = 0;
    struct sockaddr_in6 server; // Supports 4 and 6 but requires IPv6 struct
    setupInfo_t setupInfo;

    setupInfo = processArgs(argc, argv);

    socketNum = setupUdpClientToServer(&server, argv[REMOTE_MACHINE],
                                       setupInfo.remotePort);

    sendErr_init(setupInfo.errorRate, DROP_ON, FLIP_OFF, DEBUG_ON, RSEED_ON);

    connectString(setupInfo);
    talkToServer(socketNum, &server);

    close(socketNum);

    return 0;
}

void talkToServer(int socketNum, struct sockaddr_in6 *server) {
    int serverAddrLen = sizeof(struct sockaddr_in6);
    char *ipString = NULL;
    int dataLen = 0;
    char buffer[MAXBUF + 1];

    buffer[0] = '\0';
    while (buffer[0] != '.') {
        dataLen = readFromStdin(buffer);

        printf("Sending: %s with len: %d\n", buffer, dataLen);

        sendtoErr(socketNum, buffer, dataLen, 0, (struct sockaddr *)server,
                  serverAddrLen);

        safeRecvfrom(socketNum, buffer, MAXBUF, 0, (struct sockaddr *)server,
                     &serverAddrLen);

        // print out bytes received
        ipString = ipAddressToString(server);
        printf("Server with ip: %s and port %d said it received %s\n", ipString,
               ntohs(server->sin6_port), buffer);
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

setupInfo_t processArgs(int argc, char *argv[]) {

    setupInfo_t args;

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

    printf("Hello \n");
    strncpy(args.fromFileName, argv[FROM_FILENAME], 100);
    strncpy(args.toFileName, argv[TO_FILENAME], 100);
    args.windowSize = atoi(argv[WINDOW_SIZE]);
    args.bufferSize = atoi(argv[BUFFER_SIZE]);
    args.errorRate = atof(argv[ERROR_RATE]);
    strncpy(args.remoteMachine, argv[REMOTE_MACHINE], 64);
    args.remotePort = atoi(argv[REMOTE_PORT]);

    printf("DEBUG: %s %s %d %d %f %s %d\n", args.fromFileName, args.toFileName,
           args.windowSize, args.bufferSize, args.errorRate, args.remoteMachine,
           args.remotePort);

    return args;
}

int connectString(setupInfo_t setupInfo) {
    printf("DEBUG: Setting up connection to server");
    uint16_t bufferSize = 4+2+1+100+2+4;
    uint8_t buf[bufferSize];
    memset(buf, 0x0000, 4);
    memset(buf + 4, 0xFF, 2);
    memset(buf + 6, 0x8, 1);
    memcpy(buf + 7, setupInfo.fromFileName, 100);
    memset(buf + 107, setupInfo.bufferSize, 2);
    memset(buf + 109, setupInfo.windowSize, 4);
    
    printBuf(buf, bufferSize);

    return 1;
}

void printBuf(uint8_t *buf, uint16_t len) {
  printf("%d: 00 00", len);
  for (int i = 0; i < len; i++) {
    if (buf[i] >= 32 && buf[i] <= 127) {
      printf("%c", (char)buf[i]);
    } else {
      printf(" %hhu ", buf[i]);
    }
  }
  printf("\n");
}


