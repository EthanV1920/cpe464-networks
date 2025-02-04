/* Filename: cclient.c
 * Author: Ethan Vosburg
 * Date: February 1, 2025
 * Version: 1.0
 * Description: This file defines the chat client that interacts with the server
 *
 *              This program will take the IP address and port number of the
 *              server as well as the handle for the client.
 *
 *              cclient handle server-name server-port
 *                  handle: is this clients handle (name), max 100 characters
 *                  server-name: is the remote machine running the server
 *                  server-port: is the port number of the server application
 *
 *              The cclient must support the following commands %M
 *              (send message), %C (multicast a message), %B (broadcast a
 *              message), %L (get a listing of all handles from the server).
 *
 */

// Includes
#include "PDU.h"
#include "networks.h"
#include "pollLib.h"
#include "safeUtil.h"
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

#define MAXBUF 1024
#define DEBUG_FLAG 1

void checkArgs(int argc, char *argv[]);
void clientControl(int clientSocket);
void processStdin(int clientSocket);
int readFromStdin(uint8_t *buffer);
void processMsgFromServer(int serverSocket);
void printBcast(uint8_t *buf);
void setHandle(char handle[100], int socket);

int main(int argc, char *argv[]) {
  int clientSocket = 0; // Socket descriptor

  checkArgs(argc, argv);

  /// Set up the TCP Client socket ///
  clientSocket = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);
  char myHandle[100];
  memcpy(myHandle, argv[1], strlen(argv[1]));

  setHandle(myHandle, clientSocket);

  /// Set up polling ///
  setupPollSet();
  addToPollSet(clientSocket);
  addToPollSet(STDIN_FILENO);

  while (1) {
    clientControl(clientSocket);
  }

  close(clientSocket);
  return 0;
}

/* Ensure the correct # of parameters were passed. Terminate otherwise. */
void checkArgs(int argc, char *argv[]) {
  /// Check command line arguments  ///
  if (argc != 4) {
    printf("usage: %s handle host-name port-number \n", argv[0]);
    exit(1);
  }
}

/* Client control logic: Read/send from stdin or handle server disconnect */
void clientControl(int clientSocket) {
  int pollResult;

  pollResult = pollCall(-1);

  if (pollResult == STDIN_FILENO) { // Available message in stdin
    processStdin(clientSocket);     // Read client's messages

  } else if (pollResult == clientSocket) { // Server connection closed
    processMsgFromServer(clientSocket);    // Terminate client socket
  }
}

/* Reads input from stdin, converts into a PDU and sends to the server */
void processStdin(int clientSocket) {
  uint8_t inputBuffer[MAXBUF];
  int bytesSent = 0;
  int bufferLength = 0;

  bufferLength = readFromStdin(inputBuffer); // Read from Stdin
  printf("Reading: %s\nString length: %d (including null)\n", inputBuffer,
         bufferLength);

  bytesSent = sendPDU(clientSocket, inputBuffer,
                      bufferLength); // Send message to server

  if (bytesSent < 0) { // ERROR DETECTION
    perror("send call");
    exit(-1);
  }
  printf("Amount of data bytesSent is: %d\n\n", bytesSent);
}

/* Reads input from stdin: Ensure the input length < buffer size and null
 * terminates the string */
int readFromStdin(uint8_t *buffer) {
  char inputChar = 0;
  int bufferIndex = 0;

  // Important you don't input more characters than you have space
  buffer[0] = '\0';
  while (bufferIndex < (MAXBUF - 1) && inputChar != '\n') {
    inputChar = getchar();
    if (inputChar != '\n') {
      buffer[bufferIndex] = inputChar;
      bufferIndex++;
    }
  }

  /// Null terminate the string ///
  buffer[bufferIndex] = '\0';
  bufferIndex++;

  return bufferIndex;
}

/* Handle server disconnect: Close the client and terminate the program */
// TODO: Process messages from server
void processMsgFromServer(int clientSocket) {

  uint8_t buf[MAXBUF];
  uint16_t len = recvPDU(clientSocket, buf, MAXBUF);

  if (len == 0) {
    printf("Server has terminated\n");
    close(clientSocket); // Close client socket
    exit(1);
  }

  switch (buf[2]) {
  case 2:
    printf("INFO: Handle Received\n");
    return;
  case 3:
    printf("WARN: Handle Denied\n");
    exit(1);
  case 4:
    printBcast(buf);
    return;
  case 5:
    return;
  case 6:
    return;
  case 7:
    return;
  case 10:
    return;
  case 11:
    return;
  case 12:
    return;
  case 13:
    return;
  default:
    return;
  }
}

void printBcast(uint8_t *buf) {
  uint16_t handleLen = buf[3];
  uint8_t tmp_handle[MAXBUF];
  memcpy(tmp_handle, buf + 4, handleLen);
  tmp_handle[handleLen] = '\0';
  printf("%s: %s\n", tmp_handle, buf + 4 + handleLen);
}

void setHandle(char handle[100], int socket) {
  uint8_t handleBuf[MAXBUF];
  uint8_t handleBufLen;

  handleBuf[0] = 1;
  handleBuf[1] = 1 + strlen(handle);

  memcpy(handleBuf+2, handle, strlen(handle));
  
  sendPDU(socket, handleBuf, handleBufLen);
}
