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
#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"

#define MAXBUF 1407

typedef struct {
    uint32_t sequenceNum;
    uint16_t checksum;
    uint8_t flag;

} __attribute__((packed)) header_t;

void processClient(int socketNum);
int checkArgs(int argc, char *argv[]);
void printBuf(uint8_t *buf, uint16_t len);


int main(int argc, char *argv[]) {
    int socketNum = 0;
    int portNumber = 0;

    portNumber = checkArgs(argc, argv);

    socketNum = udpServerSetup(portNumber);

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

    buf[0] = '\0';
    while (buf[0] != '.') {

        dataLen = safeRecvfrom(socketNum, buf, MAXBUF, 0,
                               (struct sockaddr *)&client, &clientAddrLen);

        udpHeader = (header_t *)buf;
        // printf("DEBUG: Flag Value: %x\n", udpHeader->flag);
        // printf("DEBUG: Checksum Value: %x\n", ntohl(udpHeader->checksum));
        // printf("DEBUG: Sequence Value: %x\n", udpHeader->sequenceNum);

        printf("Received message from client with ");
        printIPInfo(&client);
        printBuf((uint8_t *)&buf, dataLen);
        printf(" Len: %d \'%s\'\n", dataLen, buf);

        switch (udpHeader->flag) {

        case 8:
            printf("INFO: Flag 8 f.no init\n");
            memcpy(fileName, buf + 7, 100);
            fileName[100] = '\0';
            printf("INFO: FileName: %s\n", fileName);
            
            FILE* fp = fopen(fileName, "r");
            if (fp != 0){

                printf("DEBUG: File opened success\n");


            }else{

                fprintf(stderr, "Error: File could not be opened or does not exist");
            }

            break;

        default:
            printf("INFO: Flag not found\n");
            break;
        }

        // just for fun send back to client number of bytes received
        sprintf(buf, "bytes: %d", dataLen);
        safeSendto(socketNum, buf, strlen(buf) + 1, 0,
                   (struct sockaddr *)&client, clientAddrLen);
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
