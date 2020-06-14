#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include "BufferManager.h"
#include "MiniSQL.h"

extern BufferManager buf;

/**
 * @class: Index of branch node's element
 */
class BranchIndex {  // Not a leaf, normal node
  public:
    string key;
    int ptrChild;  // Pointer to position in file
    BranchIndex() : key(""), ptrChild(0) {}
    BranchIndex(string k, int ptrC) : key(k), ptrChild(ptrC) {}
};

/**
 * @class: Index of leaf node's element
 */
class LeafIndex {
  public:
    string key;
    int offsetInFile;
    int offsetInBlock;
    LeafIndex() : key(""), offsetInFile(0), offsetInBlock(0) {}
    LeafIndex(string k, int oif, int oib) : key(k), offsetInFile(oif), offsetInBlock(oib) {}
};

/**
 * @class: Base class for nodes
 */
class TreeNode {
  public:
    bool isRoot;
    int bufferID;
    int ptrFather;     // Block pointer, if is root, this pointer is useless
    int recordNum;     // Num of tuples
    int columnLength;  // Fixed length of column
    TreeNode() {}
    TreeNode(int vbufID) : bufferID(vbufID), recordNum(0) {}
    int getPtr(int pos);
    int getRecordNum();
};

/**
 * @class: Inheritance class for non-leaf nodes
 */
class Branch : public TreeNode {
  public:
    vector<BranchIndex> indexList;
    Branch() {}
    Branch(int vbufID) : TreeNode(vbufID) {}  // This is for new added brach
    Branch(int vbufID, const Index &indexInfo);
    ~Branch();
    void insert(BranchIndex node);
    BranchIndex pop();
    BranchIndex getFront() { return indexList.front(); }
};

/**
 * @class: Inheritance class for leaf nodes
 */
class Leaf : public TreeNode {
  public:
    int nextSib;  // Block pointer
    int prevSib;  // Block pointer
    vector<LeafIndex> indexList;
    Leaf(int vbufNum);
    Leaf(int vbufNum, const Index &indexInfo);
    ~Leaf();
    void insert(LeafIndex node);
    LeafIndex pop();
    LeafIndex getFront() { return indexList.front(); }
};

/**
 * @class: Index manager itself
 */
class IndexManager {
  public:
    void createIndex(const Table &tableInfo, Index &indexInfo);  // Create Index
    BranchIndex insertValue(
        Index &indexInfo, LeafIndex node, int blockOffset = 0);  // Insert value & update index
    Data selectEqual(const Table &tableInfo, const Index &indexInfo, string key,
        int blockOffset = 0);  // Selection of equality
    Data selectBetween(const Table &tableInfo, const Index &indexInfo, string keyFrom, string keyTo,
        int blockOffset = 0);          // Selection of range
    void dropIndex(Index &indexInfo);  // Delete an index

  private:
    Row splitRow(Table tableInfo, string row);
    string getColumnValue(const Table &tableInfo, const Index &indexInfo, string row);
};

#endif
