/*
 * A message storage service that allocates space on demand from a limited
 * pool of chained buffers.
 * Intended for systems that require efficiency and have limited resources.
 *
 * The buffer pool free list is maintained using an array-based circular queue
 * using a mask for modulo indexing.
 *
 * Author: Dale Shpak
 * File:   messageService.h
 *
 * September 27, 2016
 */

#ifndef MESSAGESERVICE_H
#define	MESSAGESERVICE_H

#include <stdio.h>

typedef short LinkType;  // Links are array indices

#define MESSAGE_DEBUG  // Define if you want to see buffer allocation messages

#ifdef MESSAGE_DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DPRINT(...)
#endif

// For efficient calculation of the required number of buffers per message,
// the buffer size must be a power of 2
#define MESSAGE_BLOCK_BITS (6)
#define MESSAGE_BUFFER_SIZE (1<<MESSAGE_BLOCK_BITS)
#define MESSAGE_BLOCK_MASK (MESSAGE_BUFFER_SIZE-1)

// For efficient modulo addressing the following must be a power of 2
#define N_MESSAGE_BUFFERS (128)
#define N_MESSAGE_BLOCKS_MASK (N_MESSAGE_BLOCKS-1)

#define IS_POWER_OF_2(x) ((x) && !((x) & ((x)-1)))

// Check that the number of buffers is compatible with the LinkType
#define MAX_MESSAGE_BLOCKS ((1<<(8*sizeof(LinkType)-1))-1)
// The previous line cannot be evaluated by the preprocessor for use in the next line!
#if (N_MESSAGE_BUFFERS > 32767) || !IS_POWER_OF_2(N_MESSAGE_BUFFERS)
#error "N_MESSAGE_BUFFERS must be <= 32767 and a power of 2"
#endif

typedef struct {
    int nBytes;  // If nBytes < 0, Message is invalid
    LinkType firstBlock;
} Message;


Message createMessage(const char *myMessage, const int nBytes);

void freeMessage(Message message);

void initializeMessageBlocks(void);

int numberOfFreeBuffers(void);

void printMessage(FILE *stream, const Message message);

#endif	/* MESSAGESERVICE_H */

