#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "MiniSQL.h"

class BufferManager {
  public:
    friend class RecordManager;
    friend class IndexManager;
    friend class Leaf;
    friend class Branch;
    friend class TreeNode;
    BufferManager();
    ~BufferManager();

  private:
    buffer bufferBlock[MAX_BLOCK_NUMBER];
    void flashBack(int bufferID);                              // Write back to disk
    int getbufferID(string fileName, int blockOffset);         // Get bufferID on which fn:bo locate
    void readBlock(string fileName, int blockOff, int bufID);  // Read data in fn:bo to buf[bID]
    void writeBlock(int bufferID);                             // Write buffer[bufferID] to disk
    void useBlock(int bufferID);                               // For LRU Replacement
    int getEmptyBuffer();                                      // Get a empty or LRU buffer
    int getEmptyBufferExcept(string fileName);                 // Except the ones in fileName
    insertPos getInsertPosition(Table &tableInfo);
    int addBlockInFile(Table &tableInfo);  // Add one more block in file for the table
    int addBlockInFile(Index &indexInfo);  // Add one more block in file for the index
    int getIfIsInBuffer(
        string fileName, int blockOffset);  // Get bufferID if it's inside given location
    void setInvalid(string fileName);       // Invalidate pertaining buffer
};

#endif
