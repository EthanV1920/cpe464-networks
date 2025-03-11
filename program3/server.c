/* Server side - UDP Code				    */
/* By Hugh Smith	4/1/2017	*/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "checksum.h"
#include "docs/libcpe464_2_21b/libcpe464/networks/network-hooks.h"
#include "gethostbyname.h"
#include "net_struct.h"
#include "networks.h"
#include "pollLib.h"
#include "printBuf.h"
#include "safeUtil.h"
#include "sendData.h"

#define MAXBUF 1407

void processClient(int socketNum);
void handleZombies(int sig);
void gracefulClosing(int sig);
int checkArgs(int argc, char *argv[]);
void handleSend(setupInfo_t *setupInfo, char *buf);

int main(int argc, char *argv[]) {
#ifdef DEBUG
    printf("%d: ", getpid());
    printf("DEBUGGING ON\n");
#endif
    int socketNum = 0;
    int portNumber = 0;

    portNumber = checkArgs(argc, argv);

    socketNum = udpServerSetup(portNumber);

    signal(SIGINT, gracefulClosing);
    signal(SIGCHLD, handleZombies);

    processClient(socketNum);

    close(socketNum);

    return 0;
}

void processClient(int socketNum) {
    int dataLen = 0;
    char buf[MAXBUF + 1];
    struct sockaddr_in6 client;
    header_t *udpHeader;
    int clientAddrLen = sizeof(client);
    char fileName[101];
    setupInfo_t setupInfo;
    uint8_t pidCount = 0;
    int pid = -1;

    buf[0] = '\0';
    while (pollCall(0)) {

        dataLen = safeRecvfrom(socketNum, buf, MAXBUF, 0,
                               (struct sockaddr *)&client, &clientAddrLen);

        udpHeader = (header_t *)buf;

        verifyData((uint8_t *)buf, dataLen, udpHeader->checksum);

        setupInfo.server = &client;
        setupInfo.socketNum = socketNum;

#ifdef DEBUG
        printf("%d: ", getpid());
        printf("DEBUG: Flag Value: %x\n", udpHeader->flag);
        printf("%d: ", getpid());
        printf("DEBUG: Checksum Value: %x\n", ntohl(udpHeader->checksum));
        printf("%d: ", getpid());
        printf("DEBUG: Sequence Value: %x\n", udpHeader->sequenceNum);
        printf("%d: ", getpid());
        printf("Received message from client with ");
        printf("%d: ", getpid());
        printf(" Len: %d \'%s\'\n", dataLen, buf);
#endif

        // printIPInfo(&client);
        printIPInfo(setupInfo.server);
        printBuf((uint8_t *)&buf, dataLen);

        int rr = 0;

        switch (udpHeader->flag) {
            // Then in main add :
            //
            //     // We are going to fork() so need to clean up (SIGCHLD),
            //     where
            //     // handleZombies is the function
            //
            //     // you wrote - see above
            //     signal(SIGCHLD, handleZombies);

        case 6: // SREJ Packet
            break;
        case 8: // Connect from client to server
            printf("%d: ", getpid());
            printf("INFO: Flag 8 f.no init\n");

            // Getting windowSize and bufferSize
            // strcpy(buf + 7, setupInfo.fromFileName);
            memcpy(&setupInfo.fromFileName, buf + 7, 100);

            uint32_t windowSizeTemp = 0;
            memcpy(&windowSizeTemp, buf + 7 + strlen(setupInfo.fromFileName),
                   sizeof(uint32_t));
            setupInfo.windowSize = ntohl(windowSizeTemp);

            uint16_t bufferSizeTemp = 0;
            memcpy(&bufferSizeTemp, buf + 11 + strlen(setupInfo.fromFileName),
                   sizeof(uint16_t));
            setupInfo.bufferSize = ntohs(bufferSizeTemp);

#ifdef DEBUG
            printf("%d: ", getpid());
            printf("INFO: from filename %s\n", setupInfo.fromFileName);
            printf("INFO: windowSize %d\n", setupInfo.windowSize);
            printf("INFO: buffer size %d\n", setupInfo.bufferSize);
#endif

            if (pidCount <= 10) {
                pid = fork();
                if (pid == 0) {
                    printf("INFO: Child Process %d\n", getpid());
                    handleSend(&setupInfo, buf);
                } else if (pid > 0) {
                    pidCount++;
                    printf("INFO: this is the parent, Child: %d\n", pid);
                }
            }

            break;
        case 9: // Connect response from server
            break;
        case 10: // Packet is EOF last packet
            break;
        case 16: // Regular data packet
            printBuf((uint8_t *)buf, dataLen);
            char buf[] = "MSG WAS RECEIVED";
            sendData(buf, 17, 0, 9, &setupInfo);
            break;
        case 17: // Resent data packet after SREJ
            break;
        case 18: // Resent data packet after timeout
            break;
        default:
            printf("INFO: Flag not found %d\n", udpHeader->flag);
            break;
        }

        // just for fun send back to client number of bytes received
        // sprintf(buf, "bytes: %d", dataLen);
        // safeSendto(socketNum, buf, strlen(buf) + 1, 0,
        //            (sruct sockaddr *)&client, clientAddrLen);
    }
}

void handleSend(setupInfo_t *setupInfo, char *buf) {
    int dataLen = 0;
    struct sockaddr_in6 client;
    uint32_t rrBuf = 0;
    header_t *udpHeader;
    int clientAddrLen = sizeof(client);
    char fileName[101];
    int upper = setupInfo->windowSize;
    int lower = 0;
    int current = 0;
    int seq = 0;
    int rr = -1;
    FILE *fp = NULL;
    char **window;

    window = (char **)malloc(setupInfo->windowSize * sizeof(char *));

    for (int i = 0; i < setupInfo->windowSize; i++) {

        printf("%d: ", getpid());
        printf("Setting up window %d\n", i);

        window[i] = (char *)malloc(setupInfo->bufferSize * sizeof(char));
    }

    printf("INFO: Process %d\n", getpid());

    // TODO: change this to reflect an arg
    sendErr_init(0, DROP_ON, FLIP_ON, DEBUG_ON, RSEED_OFF);

    // Set up polling
    setupPollSet();
    addToPollSet(setupInfo->socketNum);

    printf("INFO: Child Process %d\n", getpid());
    memcpy(fileName, buf + 7, 100);
    fileName[100] = '\0';
    printf("INFO: FileName: %s\n", fileName);

    fp = fopen(fileName, "r");
    if (fp != 0) {

#ifdef DEBUG
        printf("%d: ", getpid());
        printf("DEBUG: File opened success\n");
#endif
        char buf[] = "GOOD FILE";
        sendData(buf, 9, 0, 9, setupInfo);

        // while (fread(buf, 1, 10, fp)) {
        //     // fread(window[seq++], sizeof(char),
        //     // setupInfo->bufferSize, fp); fread(buf, 1, 10, fp);
        // sendData(buf, 10, seq++, 16, &setupInfo);
        // }

    } else {

        fprintf(stderr, "Error: File could not be opened or does not exist\n");
        char buf[] = "BAD FILE";
        sendData(buf, 8, 0, 9, setupInfo);
    }

    while (pollCall(1000) != -1) {

        switch (udpHeader->flag) {
        case 5: // RR Packet

            memcpy(&rrBuf, buf + 7, sizeof(uint32_t));
            rrBuf = ntohl(rrBuf);

#ifdef DEBUG
            printf("%d: ", getpid());
            printf("DEBUG: RR received %d\n", rrBuf);
#endif
            break;
        }
        dataLen = safeRecvfrom(setupInfo->socketNum, buf, MAXBUF, 0,
                               (struct sockaddr *)&client, &clientAddrLen);

        while (fread(window[seq], sizeof(char), setupInfo->bufferSize, fp)) {
            lower = rr;
            upper = lower + setupInfo->bufferSize;
            // if (rr > lower) {
            // while (fread(buf,1,10,fp)) {
            // fread(window[seq++], sizeof(char), setupInfo->bufferSize, fp);
            // fread(buf, 1, 10, fp);
            // sendData(buf, 10, seq++, 16, &setupInfo);
            printf("%d: ", getpid());
            printf("I am going to send data now: %s\n", window[seq]);
            sendData(window[seq], setupInfo->bufferSize, seq, 16, setupInfo);
            seq++;
        }
    }

    exit(0);
}

int checkArgs(int argc, char *argv[]) {
    // Checks args and returns port number
    int portNumber = 0;

    if (argc > 2) {
        fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
        exit(-1);
    }

    if (argc == 2) {
        portNumber = atoi(argv[1]);
    }

    return portNumber;
}

// SIGCHLD handler - clean up terminated processes
void handleZombies(int sig) {
    int stat = 0;
    while (waitpid(-1, &stat, WNOHANG) > 0) {
    }
    exit(0);
}

void gracefulClosing(int sig) {

    printf("%d: ", getpid());
    printf("\n\nINFO: Gracefully Closing...\n");

    exit(0);
}
