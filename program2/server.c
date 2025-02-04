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
#include "stdio.h"
#include "string.h"


int main(int arg_c, char * arg_v[]) {
    for (int i = 0; i < arg_c; i++){

        printf("INFO: %s\n", arg_v[i]);
    }


}
