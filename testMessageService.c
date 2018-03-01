/*
 * A quick and dirty test of the message storage service
 *
 * Author: Dale Shpak
 * File:   testMessageService.c
 *
 * September 27, 2016
 */

#include <stdio.h>
#include <string.h>
#include "messageService.h"

#define N_TEST_LOOPS 100

// There is a #define in messageService.h to control verbosity

char *msg  = "Welcome to the Twilight Zone.  This is a test of the messaging system. Please do not adjust your television set or leave the room.";
char *msg2 = "This is a brief test of the messaging system.";
char *msg3 = "This is a somewhat longer test of the messaging system for your consideration.";

int allocationErrorCheck(int nBytes) {
    if (nBytes < 0) {
        fprintf(stderr, "***** Unable to allocate blocks\n");
        return 1;
    }
    return 0;
}

int main(void) {
    Message message, message2;

    // Note that the printf's go to stderr so that they will align with the debug output,
    // even if output is redirected to a file.
    for (int k = 0; k < N_TEST_LOOPS; k++) {
        fprintf(stderr, "\nTest %d\n", k);
        message = createMessage(msg, strlen(msg));
        if (allocationErrorCheck(message.nBytes)) break;
        printMessage(stderr, message);
        fprintf(stderr, "\n");

        message2 = createMessage(msg2, strlen(msg2));
        if (allocationErrorCheck(message.nBytes)) break;
        printMessage(stderr, message2);
        fprintf(stderr, "\n");

        freeMessage(message2);
        freeMessage(message);

        message = createMessage(msg3, strlen(msg3));
        if (allocationErrorCheck(message.nBytes)) break;
        printMessage(stderr, message);
        fprintf(stderr,"\n");
        freeMessage(message);

        fprintf(stderr, "Free %d\n", numberOfFreeBuffers());
    }
    
    message = createMessage(NULL, 2*MESSAGE_BLOCK_BITS*N_MESSAGE_BUFFERS);
    if (message.nBytes != -1) {
      fprintf(stderr, "***** Negative test of excessive allocation failed!\n");
    }
}
