#include "BufferManager.h"

BufferManager::BufferManager() {
    for (int i = 0; i < MAX_BLOCK_NUMBER; i++)
        bufferBlock[i].initialize();
}

BufferManager::~BufferManager() {
    for (int i = 0; i < MAX_BLOCK_NUMBER; i++)
        flashBack(i);
}

/**
 * @funct: Write back to disk
 */
void BufferManager::flashBack(int bufferID) {
    if (!bufferBlock[bufferID].isWritten)
        return;
    string fileName = bufferBlock[bufferID].fileName;
    fstream fout(CACHE_FOLDER + fileName, ios::in | ios::out);
    fout.seekp(BLOCK_SIZE * bufferBlock[bufferID].blockOffset, fout.beg);
    fout.write(bufferBlock[bufferID].values, BLOCK_SIZE);
    bufferBlock[bufferID].initialize();
    fout.close();
}

/**
 * @funct: Get bufferID on which fn:bo locate
 */
int BufferManager::getbufferID(string fileName, int blockOffset) {
    int bufferID = getIfIsInBuffer(fileName, blockOffset);
    if (bufferID == -1) {
        bufferID = getEmptyBufferExcept(fileName);
        readBlock(fileName, blockOffset, bufferID);
    }
    return bufferID;
}

/**
 * @funct: Read data in fn:bo to buf[bID]
 */
void BufferManager::readBlock(string fileName, int blockOffset, int bufferID) {
    bufferBlock[bufferID].isValid     = 1;
    bufferBlock[bufferID].isWritten   = 0;
    bufferBlock[bufferID].fileName    = fileName;
    bufferBlock[bufferID].blockOffset = blockOffset;
    fstream fin(CACHE_FOLDER + fileName, ios::in);
    fin.seekp(BLOCK_SIZE * blockOffset, fin.beg);
    fin.read(bufferBlock[bufferID].values, BLOCK_SIZE);
    fin.close();
}

/**
 * @funct: Write buffer[bufferID] to disk
 */
void BufferManager::writeBlock(int bufferID) {
    bufferBlock[bufferID].isWritten = 1;
    useBlock(bufferID);
}

/**
 * @funct: For LRU Replacement
 */
void BufferManager::useBlock(int bufferID) {  // This LRU algorithm is quite expensive
    for (int i = 1; i < MAX_BLOCK_NUMBER; i++) {
        if (i == bufferID) {
            bufferBlock[bufferID].LRUvalue = 0;
            bufferBlock[i].isValid         = 1;
        } else
            bufferBlock[bufferID].LRUvalue++;  // The greater is LRUvalue, the less useful the block is
    }
}

/**
 * @funct: Get a empty or LRU buffer
 */
int BufferManager::getEmptyBuffer() {
    int bufferID        = 0;
    int highestLRUvalue = bufferBlock[0].LRUvalue;
    for (int i = 0; i < MAX_BLOCK_NUMBER; i++) {
        if (!bufferBlock[i].isValid) {
            bufferBlock[i].initialize();
            bufferBlock[i].isValid = 1;
            return i;
        } else if (highestLRUvalue < bufferBlock[i].LRUvalue) {
            highestLRUvalue = bufferBlock[i].LRUvalue;
            bufferID        = i;
        }
    }
    flashBack(bufferID);
    bufferBlock[bufferID].isValid = 1;
    return bufferID;
}

/**
 * @funct: Get a empty or LRU buffer except the ones in fileName
 */
int BufferManager::getEmptyBufferExcept(
    string fileName) {  // Buffer with the destine fileName is not suppose to be flashback
    int bufferID        = -1;
    int highestLRUvalue = bufferBlock[0].LRUvalue;
    for (int i = 0; i < MAX_BLOCK_NUMBER; i++) {
        if (!bufferBlock[i].isValid) {
            bufferBlock[i].isValid = 1;
            return i;
        } else if (highestLRUvalue < bufferBlock[i].LRUvalue && bufferBlock[i].fileName != fileName) {
            highestLRUvalue = bufferBlock[i].LRUvalue;
            bufferID        = i;
        }
    }
    if (bufferID == -1) {
        cout << "All the buffers in the database system are used up. Sorry about that!" << endl;
        exit(0);
    }
    flashBack(bufferID);
    bufferBlock[bufferID].isValid = 1;
    return bufferID;
}

/**
 * @funct: Get pos for Record Manager
 * @ret: A insertPos class which stores info required
 */
insertPos BufferManager::getInsertPosition(
    Table &tableInfo) {  // To increase efficiency, we always insert values from the back of the file
    insertPos iPos;
    if (tableInfo.blockNum == 0) {  // The *.table file is empty and the data is firstly inserted
        iPos.bufferID = addBlockInFile(tableInfo);
        iPos.position = 0;
        return iPos;
    }
    string fileName = tableInfo.name + ".table";
    int length      = tableInfo.totalLength + 1;
    int blockOffset = tableInfo.blockNum - 1;  // Get the block offset in file of the last block
    int bufferID    = getIfIsInBuffer(fileName, blockOffset);
    if (bufferID == -1) {  // Indicate that the data is not read in buffer yet
        bufferID = getEmptyBuffer();
        readBlock(fileName, blockOffset, bufferID);
    }
    const int recordNum = BLOCK_SIZE / length;
    for (int offset = 0; offset < recordNum; offset++) {
        int position = offset * length;
        char isEmpty = bufferBlock[bufferID].values[position];
        if (isEmpty == EMPTY) {  // Find an empty space
            iPos.bufferID = bufferID;
            iPos.position = position;
            return iPos;
        }
    }
    // If the program run till here, the last block is full, therefor one more block is added
    iPos.bufferID = addBlockInFile(tableInfo);
    iPos.position = 0;
    return iPos;
}

/**
 * @funct: Add one more block in file for the table
 * @ret: bufferID
 */
int BufferManager::addBlockInFile(Table &tableInfo) {  // Add one more block in file for the table
    int bufferID = getEmptyBuffer();
    bufferBlock[bufferID].initialize();
    bufferBlock[bufferID].isValid     = 1;
    bufferBlock[bufferID].isWritten   = 1;
    bufferBlock[bufferID].fileName    = tableInfo.name + ".table";
    bufferBlock[bufferID].blockOffset = tableInfo.blockNum++;
    return bufferID;
}

/**
 * @funct: Add one more block in file for the index
 * @ret: bufferID
 */
int BufferManager::addBlockInFile(Index &indexInfo) {  // Add one more block in file for the index
    string fileName = indexInfo.indexName + ".index";
    int bufferID    = getEmptyBufferExcept(fileName);
    bufferBlock[bufferID].initialize();
    bufferBlock[bufferID].isValid     = 1;
    bufferBlock[bufferID].isWritten   = 1;
    bufferBlock[bufferID].fileName    = fileName;
    bufferBlock[bufferID].blockOffset = indexInfo.blockNum++;
    return bufferID;
}

/**
 * @funct: Get bufferID if it's inside given location
 * @ret: If not inside, return -1, otherwise return bufferID
 */
int BufferManager::getIfIsInBuffer(string fileName, int blockOffset) {
    for (int bufferID = 0; bufferID < MAX_BLOCK_NUMBER; bufferID++) {
        if (bufferBlock[bufferID].fileName == fileName && bufferBlock[bufferID].blockOffset == blockOffset)
            return bufferID;
    }
    return -1;  // Indicate that the data is not read in buffer yet
}

/**
 * @funct: When a table or an index file is deleted,
 *         all the value in buffer should be set invalid
 */
void BufferManager::setInvalid(string fileName) {
    for (int i = 0; i < MAX_BLOCK_NUMBER; i++)
        if (bufferBlock[i].fileName == fileName) {
            bufferBlock[i].isValid   = 0;
            bufferBlock[i].isWritten = 0;
        }
}