
#include <stdio.h>
#include <string.h>
#include "messageService.h"
static LinkType getBlock();
static LinkType blocksHead;
static LinkType blocksTail;
static LinkType blocksFree[N_MESSAGE_BUFFERS]; 
static LinkType links[N_MESSAGE_BUFFERS];
static int blocksAvailable; 
static char messageBlocksInitialized = 0;

// Allocate the array of blocks
static char messageBlocks[N_MESSAGE_BUFFERS][MESSAGE_BUFFER_SIZE];

/**
 * Initializes the entire set of data structures for the buffer pool system.
 */
void initializeMessageBlocks(void) {
    for (int k = 0; k < N_MESSAGE_BUFFERS; k++) {
        blocksFree[k] = k;
    }
    blocksHead = 0;
    blocksTail = N_MESSAGE_BUFFERS - 1;
    blocksAvailable = N_MESSAGE_BUFFERS;
    messageBlocksInitialized = 1;
}
/**
 * Copies a message from an array of chars (bytes) into a newly-allocated
 * set of buffers.
 *
 * @param myMessage A pointer to the first byte of data to be copied
 * @param nBytes The number of bytes to be copied
 *
 * @return A struct containing the Message
 */
Message createMessage(const char *myMessage, const int nMessage) { 
    if (!messageBlocksInitialized) {
        initializeMessageBlocks();
    }
    // Determine if message length exceeds memory allocated.
    Message retVal;
    if (nMessage > (MESSAGE_BLOCK_BITS * N_MESSAGE_BUFFERS)){ 
        retVal.nBytes = -1;
        return retVal;
    }
    retVal.nBytes = nMessage;
    
    // Determine number of blocks required for message
    int blocksNeeded = nMessage >> (MESSAGE_BLOCK_BITS);
    if (nMessage & MESSAGE_BLOCK_MASK) {
        ++blocksNeeded;
    }

    // Determine if the Message size exceeds current memory
    if (blocksNeeded > blocksAvailable) { 
        retVal.nBytes = -1;   
        return retVal;
    }

    LinkType thisBlock = getBlock();
    retVal.firstBlock = thisBlock;

    DPRINT("Need %d. Using blocks: ", blocksNeeded);
    
    int nLeft = nMessage;
    for (int n = 1; n < blocksNeeded; n++) {  
        DPRINT("  %d", thisBlock);
        
        // Copy message into current block
        int nCopy = (nLeft < MESSAGE_BUFFER_SIZE) ? nLeft : MESSAGE_BUFFER_SIZE;
        memcpy (messageBlocks[thisBlock], myMessage, nCopy);
        myMessage += nCopy;
        nLeft -= nCopy;
       
        // Fetch next available block
        LinkType previousBlock = thisBlock;
        thisBlock = getBlock();
        links[previousBlock] = thisBlock;
    }
    
    // Manage memory in last block
    DPRINT("%  d\n", thisBlock);
    DPRINT("blocksHead %d\n", blocksHead);
    memcpy (messageBlocks[thisBlock], myMessage, nLeft);
    links[thisBlock] = -1;
    return retVal;
}
/*
 * Get one free Message Block
 *
 * @return Block index if successful.  -1 otherwise.
 */
static LinkType getBlock(){
    if (blocksAvailable == 0) {
        return -1;
    }
    LinkType newBlock = blocksFree[blocksHead];
    blocksHead = (blocksHead == N_MESSAGE_BUFFERS - 1) ? 0 : blocksHead + 1;
    blocksAvailable--;
    return newBlock;
}

/**
 * Returns all buffers used by a Message to the buffer pool.
 *
 * @param The Message to be freed
 */
void freeMessage(Message message) {
    LinkType thisBlock = message.firstBlock;
    do {
        DPRINT("Freeing blocks: %d\n", thisBlock);
        blocksTail = (blocksTail == N_MESSAGE_BUFFERS - 1) ? 0 : blocksTail + 1;
        blocksFree[blocksTail] = thisBlock;
        thisBlock = links[thisBlock];
        blocksAvailable++;
    } while (thisBlock != -1);
    DPRINT("blocksTail: %d\n", blocksTail); 
}



/**
 * Returns the number of buffers that are available in the pool.
 * A convenience function that is not needed for using the Message Service.
 *
 * @return The number of free buffers
 */
int numberOfFreeBuffers(void) {
    return blocksAvailable;
}

/**
 * Prints the bytes contained in the Message as a character string to stdout.
 *
 * @param message The Message to be printed
 */
void printMessage(FILE *stream, const Message message) {
    LinkType thisBlock = message.firstBlock;
    int nLeft = message.nBytes;
    do {
        int nCopy = (nLeft < MESSAGE_BUFFER_SIZE) ? nLeft : MESSAGE_BUFFER_SIZE;
        fwrite(messageBlocks[thisBlock], 1, nCopy, stderr);
        nLeft -= nCopy;
        thisBlock = links[thisBlock];
    } while (thisBlock != -1);
}

    
