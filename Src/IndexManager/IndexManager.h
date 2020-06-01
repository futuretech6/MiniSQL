#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include "MiniSQL.h"

/**
 * @class: Index of branch node's element
 */
class BranchIndex {
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
    int offsetFile;
    int offsetBlock;
    LeafIndex() : key(""), offsetFile(0), offsetBlock(0) {}
    LeafIndex(string k, int OF, int OB) : key(k), offsetFile(OF), offsetBlock(OB) {}
};

/**
 * @class: Base class for nodes
 */
class TreeNode {
  public:
    bool isRoot;
    int bufferID;
    int ptrParent;     // Block pointer, if is root, this pointer is useless
    int recordNum;     // Num of tuples
    int columnLength;  // Fixed length of column
    TreeNode() {}
    TreeNode(int bufID) : bufferID(bufID), recordNum(0) {}
    int getPtr(int pos);
    int getRecordNum();
};

/**
 * @class: Inheritance class for non-leaf nodes
 */
class Branch : public TreeNode {
  public:
    vector<BranchIndex> nodeList;
    Branch(int bufID) : TreeNode(bufID) {}  // This is for new added brach
    Branch(int bufID, const Index &indexInfo);
    ~Branch();
    void insert(BranchIndex node);
    BranchIndex pop();
    BranchIndex &getFront() { return nodeList.front(); }
};

/**
 * @class: Inheritance class for leaf nodes
 */
class Leaf : public TreeNode {
  public:
    int nextSib;  // Block pointer
    int prevSib;  // Block pointer
    vector<LeafIndex> nodeList;
    Leaf(int bufID);
    Leaf(int bufID, const Index &indexInfo);
    ~Leaf();
    void insert(LeafIndex node);
    LeafIndex pop();
    LeafIndex &getFront() { return nodeList.front(); }
};

/**
 * @class: Index manager itself
 */
class IndexManager {
  public:
    void createIndex(const Table &tableInfo, Index &indexInfo);                      // Create Index
    BranchIndex insertValue(Index &indexInfo, LeafIndex node, int blockOffset = 0);  // Insert value & update index
    Data selectEqual(
        const Table &tableInfo, const Index &indexInfo, string key, int blockOffset = 0);  // Selection of equality
    Data selectBetween(const Table &tableInfo, const Index &indexInfo, string keyFrom, string keyTo,
        int blockOffset = 0);          // Selection of range
    void dropIndex(Index &indexInfo);  // Delete an index

  private:
    Row splitRow(Table tableInfo, string row);  // For deletion
    string getColumnValue(
        const Table &tableInfo, const Index &indexInfo, string row);  // Get column name based on index in table
};

#endif
