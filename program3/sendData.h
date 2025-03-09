/* Filename: sendDate.c
 * Author: Ethan Vosburg
 * Date: February 28, 2025
 * Version: 1.0
 * Description: This program sends data and prints out the information that it
 *              sends.
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
#include "safeUtil.h"
#include "printBuf.h"

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

void sendData(char *buf, uint16_t bufLen, uint32_t sequence, uint8_t flag,
              setupInfo_t *setupInfo);

