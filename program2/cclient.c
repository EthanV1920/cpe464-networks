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
#include "stdio.h"
#include "string.h"
#include "inc/pollLib.h"
#include "inc/networks.h"
#include "inc/safeUtil.h"
#include "inc/gethostbyname.h"


