# Public Methods

`BufferManager();`：构造函数，对manager里的每个块调用`initialize()`函数，设置块的属性

`~BufferManager();`：析构函数，将manager里的每个块的内容写入硬盘

`int getEmptyBuffer();`：找空的buffer块，如果都使用了，就找LRU值最大的那个(即最近最少用的块)，返回块的ID

`int getbufferID(string fileName, int blockOffset);`：如果fileName:blockOffset在buffer中则返回buffer的ID，如果不在就找一个空的或者LRU的块，将fileName对应的Block从磁盘中读入。Index manager模块需要该函数

`int getIfIsInBuffer(string fileName, int blockOffset);`：字面意思，检查fileName:blockOffset是否在buffer中，是的话返回其ID，否则返回-1。Record manager和Index manager需要

`void readBlock(string fileName, int blockOff, int bufID);`：从磁盘文件`fileName`将位移为`blockOff`位置开始，大小为`BLOCK_SIZE`的数据读进`bufferBlock[bufferID].values`

`void setInvalid(string fileName);`：将buffer中fileName对应的block都设为invalid以便之后使用。Record manager和Index manager需要

`insertPos getInsertPosition(Table &tableInfo);`：返回一个可插入record的位置，存储在`insertPos`类中，`insertPos.bufferID:insertPos.position`表示具体位置。Record manager需要

`int addBlockInFile(Index &indexInfo);`：在buffer中找到一个块用来存储`indexInfo`，返回块的ID