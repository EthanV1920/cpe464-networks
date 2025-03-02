/* Filename: server.c
 * Author: Ethan Vosburg
 * Date: February 1, 2025
 * Version: 1.0
 * Description: The server acts as a packet forwarder between the cclients. The
 *              main goal of the server is to forward messages between clients.
 *
 *              server [port-number]
 *                  port-number an optional parameter
 *                  If present it tells the server which port number to use
 *                  If not present have the OS assign the port number
 */

// Includes
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
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

#include "PDU.h"
#include "handle.h"
#include "networks.h"
#include "pollLib.h"
#include "safeUtil.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

struct handle_table *handle_table;

int checkArgs(int argc, char *argv[]);
void serverControl(int mainServerSocket);
void addNewSocket(int mainServerSocket);
void processClient(int clientSocket);
void messageBroadcast(char *buf, int senderSocket, int messageLen);
void messageSend(uint8_t *buf, int clientSocket, int len);
void listSend(int clientSocket);

int main(int argc, char *argv[]) {
  int mainServerSocket = 0; // socket descriptor for the server socket
  int portNumber = 0;
  handle_table = create_table();

  portNumber = checkArgs(argc, argv);

  // create the server socket
  mainServerSocket = tcpServerSetup(portNumber);

  serverControl(mainServerSocket);

  /* close the sockets */
  close(mainServerSocket);

  return 0;
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

/// Accept clients and prints client messages ///
void serverControl(int mainServerSocket) {
  int socketNumber;

  /// Set up polling ///
  setupPollSet();
  addToPollSet(mainServerSocket);

  while (1) {
    socketNumber = pollCall(-1);

    if (socketNumber == mainServerSocket) { // Add client to polling table
      addNewSocket(mainServerSocket);

    } else if (socketNumber > mainServerSocket) { // Print client's message
      processClient(socketNumber);
    }
  }
}

/// Adds client's socket to polling table ///
void addNewSocket(int mainServerSocket) {
  int clientSocket;

  clientSocket = tcpAccept(mainServerSocket, DEBUG_FLAG); // Accept client
  addToPollSet(clientSocket); // Add client to polling table
}

/// Print client's message ///
void processClient(int clientSocket) {
  uint8_t dataBuffer[MAXBUF];
  int messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF);
  int flag = dataBuffer[2];
  printf("INFO: Flag %x invoked\n", flag);

  /// Print data from the client_socket ///
  if (messageLen > 0) { // Message in buffer

    switch (flag) {
    case 1:
      add_entry(clientSocket, (char *)(dataBuffer + 4), dataBuffer[3],
                handle_table);
      printf("INFO: added entry %s\n", get_handle(handle_table, clientSocket));
      return;
    case 4:
      messageBroadcast((char *)(dataBuffer + 2), clientSocket, messageLen - 2);
    case 5:
      messageSend(dataBuffer, clientSocket, messageLen);
      break;
    case 6:
      messageSend(dataBuffer, clientSocket, messageLen);
      break;
    case 10:
      printf("INFO: sending list to client %d\n", clientSocket);
      listSend(clientSocket);
      break;
    }

    printf("Message received on socket: %d\nLength: %d\nData: %s\n\n",
           clientSocket, messageLen, dataBuffer + 2);

  } else if (messageLen == 0) { // Client connection closed
    printf("Client has closed their connection\n");
    removeFromPollSet(clientSocket);
    close(clientSocket);

  } else { // Error detection: Buffer < PDU length or default condition in
           // recv() was reached
    exit(1);
  }
}

void messageBroadcast(char *buf, int senderSocket, int messageLen) {
  printf("INFO: Broadcasting Message %d\n",handle_table->entry_count);
  int socket;
  for (int i = 0; i < handle_table->entry_count; i++) {
    socket = handle_table->handle_table_entries[i].socket_number;
    if (socket != senderSocket) {
      sendPDU(socket, (uint8_t *)buf, messageLen);
    }
  }
}

void listSend(int clientSocket) {
  // send the count
  uint8_t countBuf[5];
  uint32_t handleCount = htonl(handle_table->entry_count);

  countBuf[0] = 11;
  memcpy(countBuf + 1, &handleCount, 4);
  sendPDU(clientSocket, countBuf, 5);

  for (int i = 0; i < handle_table->entry_count; i++) {
    int socket;
    socket = handle_table->handle_table_entries[i].socket_number;
    uint8_t handleBuf[MAXBUF];
    handleBuf[0] = 12;
    handleBuf[1] = strlen(get_handle(handle_table, socket));
    printf("INFO: Handle being sent: %d %s\n", handleBuf[1],
           get_handle(handle_table, socket));
    memcpy(handleBuf + 2, get_handle(handle_table, socket), handleBuf[1]);
    sendPDU(clientSocket, handleBuf, handleBuf[1] + 2);
  }

  uint8_t buf[1];
  buf[0] = 13;
  sendPDU(clientSocket, buf, 1);
}

void messageSend(uint8_t *buf, int clientSocket, int messageLen) {
  uint8_t readerLocation = 0;
  char senderHandle[100];
  char senderMessage[200];
  char recipientHandle[100];

  uint8_t senderHandleLen = buf[3];
  memcpy(senderHandle, buf + 4, senderHandleLen);
  senderHandle[senderHandleLen] = '\0';

  uint8_t handleListStart = 4 + senderHandleLen;
  uint8_t numRecipients = buf[handleListStart];
  readerLocation = handleListStart + 1;

  for (int i = 0; i < numRecipients; i++) {
    uint8_t recipientHandleLen = buf[readerLocation];
    readerLocation++; // Advance to start of handle string

    memcpy(recipientHandle, buf + readerLocation, recipientHandleLen);
    recipientHandle[recipientHandleLen] = '\0';
    readerLocation += recipientHandleLen; // Move past the handle text

    int recipientSocket = get_socket(handle_table, recipientHandle);
    sendPDU(recipientSocket, buf + 2, messageLen);

    printf("INFO: Routing message to %s (socket %d)\n", recipientHandle,
           recipientSocket);
  }

  // strncpy(senderMessage, (char*)(buf + readerLocation), sizeof(senderMessage)
  // - 1); senderMessage[sizeof(senderMessage) - 1] = '\0'; printf("%s: %s\n",
  // senderHandle, senderMessage);
}
