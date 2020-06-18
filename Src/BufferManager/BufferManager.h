#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "MiniSQL.h"

class BufferManager {
  public:
    BufferManager();
    ~BufferManager();

    int getEmptyBuffer();                                      // Get a empty or LRU buffer
    int getbufferID(string fileName, int blockOffset);         // Get bufferID on which fn:bo locate
    int getIfIsInBuffer(string fileName, int blockOffset);     // Get bufferID if it's inside given location
    void readBlock(string fileName, int blockOff, int bufID);  // Read data in fn:bo to buf[bID]
    void setInvalid(string fileName);                          // Invalidate pertaining buffer
    insertPos getInsertPosition(Table &tableInfo);             // Get pos for Record Manager
    int addBlockInFile(Index &indexInfo);                      // Add one more block in file for the index

    buffer bufferBlock[MAX_BLOCK_NUMBER];

  private:
    void flashBack(int bufferID);               // Write back to disk
    void writeBlock(int bufferID);              // Write buffer[bufferID] to disk
    void useBlock(int bufferID);                // For LRU Replacement
    int getEmptyBufferExcept(string fileName);  // Except the ones in fileName
    int addBlockInFile(Table &tableInfo);       // Add one more block in file for the table
};

#endif
