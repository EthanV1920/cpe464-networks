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
#include "networks.h"
#include "pollLib.h"
#include "printBuf.h"
#include "safeUtil.h"
#include "sendData.h"
#include "net_struct.h"

#define MAXBUF 1407


void processClient(int socketNum);
void handleZombies(int sig);
void gracefulClosing(int sig);
int checkArgs(int argc, char *argv[]);
int verifyData(uint8_t *buf, uint16_t len, uint16_t checksum);

int main(int argc, char *argv[]) {
#ifdef DEBUG
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
    uint8_t pidCount = 0;
    int pid = -1;
    setupInfo_t setupInfo;

    buf[0] = '\0';
    while (buf[0] != '.') {

        dataLen = safeRecvfrom(socketNum, buf, MAXBUF, 0,
                               (struct sockaddr *)&client, &clientAddrLen);

        udpHeader = (header_t *)buf;

        verifyData((uint8_t *)buf, dataLen, udpHeader->checksum);

        setupInfo.server = &client;
        setupInfo.socketNum = socketNum;

#ifdef DEBUG
        printf("DEBUG: Flag Value: %x\n", udpHeader->flag);
        printf("DEBUG: Checksum Value: %x\n", ntohl(udpHeader->checksum));
        printf("DEBUG: Sequence Value: %x\n", udpHeader->sequenceNum);
#endif

        printf("Received message from client with ");
        // printIPInfo(&client);
        printIPInfo(setupInfo.server);
        printBuf((uint8_t *)&buf, dataLen);
        printf(" Len: %d \'%s\'\n", dataLen, buf);

        switch (udpHeader->flag) {
            // Then in main add :
            //
            //     // We are going to fork() so need to clean up (SIGCHLD),
            //     where
            //     // handleZombies is the function
            //
            //     // you wrote - see above
            //     signal(SIGCHLD, handleZombies);

        case 5: // RR Packet
            break;
        case 6: // SREJ Packet
            break;
        case 8: // Connect from client to server
            printf("INFO: Flag 8 f.no init\n");
            if (pidCount <= 10) {
                pid = fork();

                /// Set up polling ///
                setupPollSet();
                addToPollSet(setupInfo.socketNum);
                addToPollSet(STDIN_FILENO);

                sendErr_init(setupInfo.errorRate, DROP_ON, FLIP_ON, DEBUG_ON,
                             RSEED_OFF);
            }

            if (pid == 0) {
                printf("INFO: Child Process %d\n", getpid());
                memcpy(fileName, buf + 7, 100);
                fileName[100] = '\0';
                printf("INFO: FileName: %s\n", fileName);

                FILE *fp = fopen(fileName, "r");
                if (fp != 0) {

#ifdef DEBUG
                    printf("DEBUG: File opened success\n");

#endif
                    char buf[] = "GOOD FILE";
                    sendData(buf, 1, 0, 9, &setupInfo);

                } else {

                    fprintf(
                        stderr,
                        "Error: File could not be opened or does not exist\n");
                    char buf[] = "BAD FILE";
                    sendData(buf, 1, 0, 9, &setupInfo);
                }

            } else if (pid > 0) {
                pidCount++;
                printf("INFO: this is the parent, Child: %d\n", pid);
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

/**
 * Verify that the data being received passes the checksum
 * @param uint8_t *buf, data to check
 * @param uint16_t len, length of the data to check
 * @return int result, result of the check
 */
int verifyData(uint8_t *buf, uint16_t len, uint16_t checksum) {
    // Zero out the data to take checksum
    memset(buf + 4, 0, 2);

    // Calculate checksum
    uint16_t calculatedChecksum = ntohs(in_cksum((ushort *)buf, len));

#ifdef DEBUG
    printf("DEBUG: Checksum = %d\n", calculatedChecksum - checksum);
#endif
    return calculatedChecksum - checksum;
}

// SIGCHLD handler - clean up terminated processes
void handleZombies(int sig) {
    int stat = 0;
    while (waitpid(-1, &stat, WNOHANG) > 0) {
    }
    exit(0);
}

void gracefulClosing(int sig) {

    printf("\n\nINFO: Gracefully Closing...\n");

    exit(0);
}
