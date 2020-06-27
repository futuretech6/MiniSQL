# IndexManager

## Class

BranchIndex：存储B+树非叶节点的index信息

LeafIndex：存储B+树叶节点的index信息

TreeNode：B+树节点的基类

BranchNode：B+树的非叶节点

LeafNode：B+树的叶节点

## Public Methods

`void createIndex(const Table &tableInfo, Index &indexInfo);`：提供表和索引类，在索引类的引用上创建索引

`Data selectEqual(const Table &tableInfo, const Index &indexInfo, string key, int blockOffset = 0);`：等值选取操作，用indexInfo传递比较属性和索引，用字符串key(类型转换)作为比较值。外部调用的时候blockOffset=0(从根开始)，内部递归的时候会另外取值。

`Data selectBetween(const Table &tableInfo, const Index &indexInfo, string keyFrom, string keyTo, int blockOffset = 0);`：范围选取，keyFrom和keyTo是字符型的比较值，其他同上

`void dropIndex(Index &indexInfo);`：删除索引indexInfo

# BufferManager

## Public Methods

`BufferManager();`：构造函数，对manager里的每个块调用`initialize()`函数，设置块的属性

`~BufferManager();`：析构函数，将manager里的每个块的内容写入硬盘

`int getEmptyBuffer();`：找空的buffer块，如果都使用了，就找LRU值最大的那个(即最近最少用的块)，返回块的ID

`int getbufferID(string fileName, int blockOffset);`：如果fileName:blockOffset在buffer中则返回buffer的ID，如果不在就找一个空的或者LRU的块，将fileName对应的Block从磁盘中读入。Index manager模块需要该函数

`int getIfIsInBuffer(string fileName, int blockOffset);`：字面意思，检查fileName:blockOffset是否在buffer中，是的话返回其ID，否则返回-1。Record manager和Index manager需要

`void readBlock(string fileName, int blockOff, int bufID);`：从磁盘文件`fileName`将位移为`blockOff`位置开始，大小为`BLOCK_SIZE`的数据读进`bufferBlock[bufferID].values`

`void setInvalid(string fileName);`：将buffer中fileName对应的block都设为invalid以便之后使用。Record manager和Index manager需要

`insertPos getInsertPosition(Table &tableInfo);`：返回一个可插入record的位置，存储在`insertPos`类中，`insertPos.bufferID:insertPos.position`表示具体位置。Record manager需要

`int addBlockInFile(Index &indexInfo);`：在buffer中找到一个块用来存储`indexInfo`，返回块的ID

# 心得

## 对于模块本身

* Index模块作为整个MiniSQL代码量最大、最易出错的模块，在写和debug过程中都需要极大的耐心
* B+树实现在ADS课程上就有学习因此不算太难，但debug过程比较困难
* Buffer模块作为一个最基础的模块，主要是配合其他模块使用，并不存在很大的困难

## Debug过程

在长久的debug过程中我对于其他模块、模块间关系有了更深刻的认识，在此期间，我遇到以下问题

### B+树的data应该存什么？

我本来认为B+树的data应该直接存tuples的数据，这样对于有Index的属性的索引可以非常快直接得到答案。

但对于有Index的属性和无Index属性复合条件，如果使Index模块进行有索引查询，record模块进行无索引查询再合并，这样record模块需要搜索全局，显然起不到Index索引的优化效果。

因此为了关联Index和Record模块的功能，Index的data中应该存**<u>Record模块中每条tuple的位置偏移量</u>**。之后再通过偏移量之间的集合运算快速获得结果的tuples在record中的位置进而获得tuples的内容。

### 键值key对于不同类型数据的形式（尤其是int/float与char[]类型之间）

在读入`create table`信息时无法提前知道每个属性的数据类型，因此将每个tuple的属性都以str的list形式存储。

我一开始以位字符串类型和代数类型的变量要有不同的索引方式，后来发现其实全部以str类型即可，因为即使是不同类型，都以str的方式进行比较结果也是相同的。

### 构造和析构函数

我一开始将模块的构造和析构都整合进了模块的子函数中，后来发现这是不必要的。

由于实际构造和析构函数是为了对于每次打开程序时已有文件的读入和结束程序时的文件存储，因此只需要在API中作为全局变量一次性全部构造，最后程序退出时全部析构。

### 对于macOS系统中出现的问题

macOS系统由于其生成一个文件夹时，会自动生成一个不可见的.DS_Store文件用于存储文件夹信息，但这个文件对于编码会有很大影响，导致文件读写出错(这似乎是macOS Catalina的又一新bug)，因此我选择每次进入程序前删除一次该文件。

为此写了一个shell脚本配合上面的all_in_one使用：

```shell
#!/bin/zsh
rm -f ./.DBFILE/.DS_Store
./all_in_one.sh
```

后期为了避免这个问题我直接在Linux下进行测试了

## 体会&教训

在本次大作业过程中，我从与用户交互Interpreter，到底层Index、Buffer、Record实现对于MySQL系统有了更加全面系统的认识。

其中让我受益颇丰的除了对于B+树操作、各模块功用之外，还有深刻意识到API对于其他模块统筹安排的重要性和困难性。

因此我认为在一个大工程开始之前应该**<u>先决定各模块的作用</u>**，这样可以有效避免模块使用上的冲突或困难。
