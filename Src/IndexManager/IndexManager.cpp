#include "IndexManager.h"
#include "BufferManager.h"

extern BufferManager buf;

int TreeNode::getPtr(int pos) {
    int ptr = 0;
    for (int i = pos; i < pos + POINTER_LENGTH; i++)
        ptr = 10 * ptr + buf.bufferBlock[bufferID].values[i] - '0';
    return ptr;
}

int TreeNode::getRecordNum() {
    int recordNum = 0;
    for (int i = 2; i < 6; i++) {
        if (buf.bufferBlock[bufferID].values[i] == EMPTY)
            break;
        recordNum = 10 * recordNum + buf.bufferBlock[bufferID].values[i] - '0';
    }
    cout << "RecordNum: " << recordNum << endl;
    return recordNum;
}

Branch::Branch(int bufID, const Index &indexInfo) {
    bufferID        = bufID;
    isRoot          = (buf.bufferBlock[bufferID].values[0] == 'R');
    int recordCount = getRecordNum();
    recordNum       = 0;  // RecordNum will increase when function insert() is called, and finally as
                          // Large as recordCount
    ptrParent    = getPtr(6);
    columnLength = indexInfo.columnLength;
    int position = 6 + POINTER_LENGTH;
    for (int i = 0; i < recordCount; i++) {
        string key = "";
        for (int i = position; i < position + columnLength; i++)
            if (buf.bufferBlock[bufferID].values[i] == EMPTY)
                break;
            else
                key += buf.bufferBlock[bufferID].values[i];

        position += columnLength;
        int ptrChild = getPtr(position);
        position += POINTER_LENGTH;
        BranchIndex node(key, ptrChild);
        insert(node);
    }
}

Branch::~Branch() {
    // IsRoot
    if (isRoot)
        buf.bufferBlock[bufferID].values[0] = 'R';
    else
        buf.bufferBlock[bufferID].values[0] = '_';
    // Is not a Leaf
    buf.bufferBlock[bufferID].values[1] = '_';
    // RecordNum
    char tmpt[5];
    // itoa(recordNum, tmpt, 10);
    sprintf(tmpt, "%d", recordNum);
    string strRecordNum = tmpt;
    while (strRecordNum.length() < 4)
        strRecordNum = '0' + strRecordNum;
    strncpy(buf.bufferBlock[bufferID].values + 2, strRecordNum.c_str(), 4);

    // NodeList
    if (nodeList.size() == 0) {
        cout << "Undefined action!" << endl;
        exit(0);
    }

    int position = 6 + POINTER_LENGTH;  //ǰ��ļ�λ����洢index�������Ϣ��
    for (auto i = nodeList.begin(); i != nodeList.end(); i++) {
        string key = i->key;
        while (key.length() < columnLength)
            key += EMPTY;
        strncpy(buf.bufferBlock[bufferID].values + position, key.c_str(), columnLength);
        position += columnLength;

        char tmpt[5];
        sprintf(tmpt, "%d", i->ptrChild);
        // itoa(i->ptrChild, tmpt, 10);
        string ptrChild = tmpt;
        while (ptrChild.length() < POINTER_LENGTH)
            ptrChild = '0' + ptrChild;
        strncpy(buf.bufferBlock[bufferID].values + position, ptrChild.c_str(), POINTER_LENGTH);
        position += POINTER_LENGTH;
    }
}

void Branch::insert(BranchIndex node) {
    recordNum++;
    auto i = nodeList.begin();
    if (nodeList.size() == 0)
        nodeList.insert(i, node);
    else {
        for (i = nodeList.begin(); i != nodeList.end(); i++)
            if (i->key > node.key)
                break;
        nodeList.insert(i, node);
    }
}

BranchIndex Branch::pop() {
    recordNum--;
    BranchIndex tmp = nodeList.back();
    nodeList.pop_back();
    return tmp;
}

Leaf::Leaf(int bufID) {  // This kind of leaf is added when old leaf is needed to be splited
    bufferID  = bufID;
    recordNum = 0;
    nextSib = prevSib = 0;
}

Leaf::Leaf(int bufID, const Index &indexInfo) {
    bufferID        = bufID;
    isRoot          = (buf.bufferBlock[bufferID].values[0] == 'R');
    int recordCount = getRecordNum();
    recordNum       = 0;
    ptrParent       = getPtr(6);
    prevSib         = getPtr(6 + POINTER_LENGTH);
    nextSib         = getPtr(6 + POINTER_LENGTH * 2);
    columnLength    = indexInfo.columnLength;

    int position = 6 + POINTER_LENGTH * 3;
    for (int i = 0; i < recordCount; i++) {
        string key = "";
        for (int i = position; i < position + columnLength; i++) {
            if (buf.bufferBlock[bufferID].values[i] == EMPTY)
                break;
            else
                key += buf.bufferBlock[bufferID].values[i];
        }
        position += columnLength;
        int offsetFile = getPtr(position);
        position += POINTER_LENGTH;
        int offsetBlock = getPtr(position);
        position += POINTER_LENGTH;

        insert(LeafIndex(key, offsetFile, offsetBlock));
    }
}

Leaf::~Leaf() {
    if (isRoot)  // Root leaf
        buf.bufferBlock[bufferID].values[0] = 'R';
    else  // Non-root leaf
        buf.bufferBlock[bufferID].values[0] = '_';
    buf.bufferBlock[bufferID].values[1] = 'L';
    // RecordNum
    char tmpt[5];  // 4 char wide
    sprintf(tmpt, "%d", recordNum);
    string strRecordNum(tmpt);
    while (strRecordNum.length() < 4)
        strRecordNum = '0' + strRecordNum;
    int position = 2;
    strncpy(buf.bufferBlock[bufferID].values + position, strRecordNum.c_str(), 4);
    position += 4;
    sprintf(tmpt, "%d", ptrParent);
    // itoa(ptrFather, tmpt, 10);
    string strptrFather = tmpt;
    while (strptrFather.length() < POINTER_LENGTH)
        strptrFather = '0' + strptrFather;
    strncpy(buf.bufferBlock[bufferID].values + position, strptrFather.c_str(), POINTER_LENGTH);
    position += POINTER_LENGTH;

    sprintf(tmpt, "%d", prevSib);
    // itoa(prevSib, tmpt, 10);
    string strLastSibling = tmpt;
    while (strLastSibling.length() < POINTER_LENGTH)
        strLastSibling = '0' + strLastSibling;
    strncpy(buf.bufferBlock[bufferID].values + position, strLastSibling.c_str(), POINTER_LENGTH);
    position += POINTER_LENGTH;

    sprintf(tmpt, "%d", nextSib);
    // itoa(nextSibling, tmpt, 10);
    string strNextSibling = tmpt;
    while (strNextSibling.length() < POINTER_LENGTH)
        strNextSibling = '0' + strNextSibling;
    strncpy(buf.bufferBlock[bufferID].values + position, strNextSibling.c_str(), POINTER_LENGTH);
    position += POINTER_LENGTH;

    // NodeList
    if (nodeList.size() == 0) {
        cout << "Undefined action!" << endl;
        exit(0);
    }

    for (auto &i : nodeList) {
        string key = i.key;
        while (key.length() < columnLength)
            key += EMPTY;
        strncpy(buf.bufferBlock[bufferID].values + position, key.c_str(), columnLength);
        position += columnLength;

        sprintf(tmpt, "%d", i.offsetBlock);
        // itoa(i.offsetFile, tmpt, 10);
        string offsetFile = tmpt;
        while (offsetFile.length() < POINTER_LENGTH)
            offsetFile = '0' + offsetFile;
        strncpy(buf.bufferBlock[bufferID].values + position, offsetFile.c_str(), POINTER_LENGTH);
        position += POINTER_LENGTH;

        sprintf(tmpt, "%d", i.offsetBlock);
        // itoa(i.offsetBlock, tmpt, 10);
        string offsetBlock = tmpt;
        while (offsetBlock.length() < POINTER_LENGTH)
            offsetBlock = '0' + offsetBlock;
        strncpy(buf.bufferBlock[bufferID].values + position, offsetBlock.c_str(), POINTER_LENGTH);
        position += POINTER_LENGTH;
        // cout << key<< "\t" <<offsetFile<<"\t"<< offsetFile<< endl;
    }
}

void Leaf::insert(LeafIndex node) {
    recordNum++;
    // cout << "onece" << endl;
    auto i = nodeList.begin();
    if (nodeList.size() == 0) {
        nodeList.insert(i, node);
        return;
    } else {
        for (auto &i : nodeList)
            if (i.key > node.key)
                break;
    }
    nodeList.insert(i, node);
}

LeafIndex Leaf::pop() {
    recordNum--;
    LeafIndex tmpt = nodeList.back();
    nodeList.pop_back();
    return tmpt;
}

void IndexManager::createIndex(const Table &tableInfo, Index &indexInfo) {
    // Create a new file
    string fileName = indexInfo.indexName + ".index";
    fstream fout(fileName.c_str(), ios::out);
    fout.close();
    // Create a root for the tree
    int blockID                          = buf.getEmptyBuffer();
    buf.bufferBlock[blockID].fileName    = fileName;
    buf.bufferBlock[blockID].blockOffset = 0;
    buf.bufferBlock[blockID].isWritten   = 1;
    buf.bufferBlock[blockID].isValid     = 1;
    buf.bufferBlock[blockID].values[0]   = 'R';  // Root Node Block
    buf.bufferBlock[blockID].values[1]   = 'L';  // Leaf Node Block
    memset(buf.bufferBlock[blockID].values + 2, '0', 4);
    for (int i = 0; i < 3; i++) {
        memset(buf.bufferBlock[blockID].values + 6 + POINTER_LENGTH * i, '0', POINTER_LENGTH);
        indexInfo.blockID++;
    }
    // Retrieve datas of the table and form a B+ Tree
    fileName = tableInfo.name + ".table";
    string stringrow;
    string key;

    int length          = tableInfo.totalLength + 1;
    const int recordNum = BLOCK_SIZE / length;

    // Read datas from the record and sort it into a B+ Tree and store it
    for (int bOf = 0; bOf < tableInfo.blockID; bOf++) {
        int bufferID = buf.getIfIsInBuffer(fileName, bOf);
        if (bufferID == -1) {
            bufferID = buf.getEmptyBuffer();
            buf.readBlock(fileName, bOf, bufferID);
        }
        for (int offset = 0; offset < recordNum; offset++) {
            int position = offset * length;
            stringrow    = buf.bufferBlock[bufferID].getvalues(position, position + length);
            if (stringrow.c_str()[0] == EMPTY)
                continue;  // Inticate that this row of record have been deleted
            stringrow.erase(stringrow.begin());
            key = getColumnValue(tableInfo, indexInfo, stringrow);
            LeafIndex node(key, bOf, offset);
            insertValue(indexInfo, node);
        }
    }
}

BranchIndex IndexManager::insertValue(Index &indexInfo, LeafIndex node, int blockOffset) {
    BranchIndex ret;  // For return, intial to be empty
    string fileName = indexInfo.indexName + ".index";
    int bufferID    = buf.getbufferID(fileName, blockOffset);
    bool isLeaf     = (buf.bufferBlock[bufferID].values[1] == 'L');  // L for leaf
    if (isLeaf) {
        Leaf leaf(bufferID, indexInfo);
        leaf.insert(node);

        const int RecordLength = indexInfo.columnLength + POINTER_LENGTH * 2;
        const int MaxrecordNum = (BLOCK_SIZE - 6 - POINTER_LENGTH * 3) / RecordLength;
        if (leaf.recordNum > MaxrecordNum) {                    // Record number is too great, need to split
            if (leaf.isRoot) {                                  // This leaf is a root
                int rootBufID = leaf.bufferID;                  // Buffer number for new root
                leaf.bufferID = buf.addBlockInFile(indexInfo);  // Find a new place for old leaf
                int sibBufID  = buf.addBlockInFile(indexInfo);  // Buffer number for sibling
                Branch branchRoot(rootBufID);                   // New root, which is branch indeed
                Leaf leafadd(sibBufID);                         // Sibling

                // Is root
                branchRoot.isRoot = 1;
                leafadd.isRoot    = 0;
                leaf.isRoot       = 0;

                branchRoot.ptrParent = leafadd.ptrParent = leaf.ptrParent = 0;
                branchRoot.columnLength = leafadd.columnLength = leaf.columnLength;
                // Link the newly added leaf block in the link list of leaf
                leafadd.prevSib = buf.bufferBlock[leaf.bufferID].blockOffset;
                leaf.nextSib    = buf.bufferBlock[leafadd.bufferID].blockOffset;
                while (leafadd.nodeList.size() < leaf.nodeList.size()) {
                    LeafIndex tnode = leaf.pop();
                    leafadd.insert(tnode);
                }

                BranchIndex tmptNode;
                tmptNode.key      = leafadd.getFront().key;
                tmptNode.ptrChild = buf.bufferBlock[leafadd.bufferID].blockOffset;
                branchRoot.insert(tmptNode);
                tmptNode.key      = leaf.getFront().key;
                tmptNode.ptrChild = buf.bufferBlock[leaf.bufferID].blockOffset;
                branchRoot.insert(tmptNode);
                return ret;
            } else {  // This leaf is not a root, we have to cascade up
                int bufferID = buf.addBlockInFile(indexInfo);
                Leaf leafadd(bufferID);
                leafadd.isRoot       = 0;
                leafadd.ptrParent    = leaf.ptrParent;
                leafadd.columnLength = leaf.columnLength;

                // Link the newly added leaf block in the link list of leaf
                leafadd.nextSib = leaf.nextSib;
                leafadd.prevSib = buf.bufferBlock[leaf.bufferID].blockOffset;
                leaf.nextSib    = buf.bufferBlock[leafadd.bufferID].blockOffset;
                if (leafadd.nextSib != 0) {
                    int bufferID = buf.getbufferID(fileName, leafadd.nextSib);
                    Leaf leafnext(bufferID, indexInfo);
                    leafnext.prevSib = buf.bufferBlock[leafadd.bufferID].blockOffset;
                }
                while (leafadd.nodeList.size() < leaf.nodeList.size()) {
                    LeafIndex tnode = leaf.pop();
                    leafadd.insert(tnode);
                }
                ret.key      = leafadd.getFront().key;
                ret.ptrChild = leaf.nextSib;
                return ret;
            }
        } else  // Not need to split,just return
            return ret;

    } else {  // Not a leaf
        Branch branch(bufferID, indexInfo);
        auto i = branch.nodeList.begin();
        if (i->key > node.key) {
            i->key = node.key;
        } else {
            for (i = branch.nodeList.begin(); i != branch.nodeList.end(); i++)
                if (i->key > node.key)
                    break;
            i--;
        }
        BranchIndex bnode = insertValue(indexInfo, node, i->ptrChild);  // Go down

        if (bnode.key == "") {
            return ret;
        } else {  // Bnode.key != "",need to split up
            branch.insert(bnode);
            const int RecordLength = indexInfo.columnLength + POINTER_LENGTH;
            const int MaxrecordNum = (BLOCK_SIZE - 6 - POINTER_LENGTH) / RecordLength;
            if (branch.recordNum > MaxrecordNum) {  // Need to split up
                if (branch.isRoot) {
                    int rbufferID   = branch.bufferID;                // Buffer number for new root
                    branch.bufferID = buf.addBlockInFile(indexInfo);  // Find a new place for old branch
                    int sbufferID   = buf.addBlockInFile(indexInfo);  // Buffer number for sibling
                    Branch branchRoot(rbufferID);                     // New root
                    Branch branchadd(sbufferID);                      // Sibling

                    // Is root
                    branchRoot.isRoot = 1;
                    branchadd.isRoot  = 0;
                    branch.isRoot     = 0;

                    branchRoot.ptrParent = branchadd.ptrParent = branch.ptrParent = 0;
                    branchRoot.columnLength = branchadd.columnLength = branch.columnLength;

                    while (branchadd.nodeList.size() < branch.nodeList.size()) {
                        BranchIndex tnode = branch.pop();
                        branchadd.insert(tnode);
                    }

                    BranchIndex tmptNode;
                    tmptNode.key      = branchadd.getFront().key;
                    tmptNode.ptrChild = buf.bufferBlock[branchadd.bufferID].blockOffset;
                    branchRoot.insert(tmptNode);
                    tmptNode.key      = branch.getFront().key;
                    tmptNode.ptrChild = buf.bufferBlock[branch.bufferID].blockOffset;
                    branchRoot.insert(tmptNode);
                    return ret;  // Here the function must have already returned to the top lay
                } else {         // Branch is not a root
                    int bufferID = buf.addBlockInFile(indexInfo);
                    Branch branchadd(bufferID);
                    branchadd.isRoot       = 0;
                    branchadd.ptrParent    = branch.ptrParent;
                    branchadd.columnLength = branch.columnLength;

                    while (branchadd.nodeList.size() < branch.nodeList.size()) {
                        BranchIndex tnode = branch.pop();
                        branchadd.insert(tnode);
                    }
                    ret.key      = branchadd.getFront().key;
                    ret.ptrChild = buf.bufferBlock[bufferID].blockOffset;
                    return ret;
                }
            } else {  // Not need to split,just return
                return ret;
            }
        }
    }
    return ret;  // Here is just for safe
}

Data IndexManager::selectEqual(const Table &tableInfo, const Index &indexInfo, string key,
    int blockOffset) {  // Start from the root and look down
    Data datas;
    string fileName = indexInfo.indexName + ".index";
    int bufferID    = buf.getbufferID(fileName, blockOffset);
    bool isLeaf     = (buf.bufferBlock[bufferID].values[1] == 'L');  // L for leaf
    if (isLeaf) {
        Leaf leaf(bufferID, indexInfo);
        auto i = leaf.nodeList.begin();
        for (i = leaf.nodeList.begin(); i != leaf.nodeList.end(); i++)
            if (i->key == key) {
                fileName            = indexInfo.table_name + ".table";
                int recordBufferNum = buf.getbufferID(fileName, i->offsetFile);
                int position        = (tableInfo.totalLength + 1) * (i->offsetBlock);
                string stringrow =
                    buf.bufferBlock[recordBufferNum].getvalues(position, position + tableInfo.totalLength);
                if (stringrow.c_str()[0] != EMPTY) {
                    stringrow.erase(stringrow.begin());
                    Row splitedRow = splitRow(tableInfo, stringrow);
                    datas.rowList.push_back(splitedRow);
                    return datas;
                }
            }
    } else {  // It is not a leaf
        Branch branch(bufferID, indexInfo);
        auto i = branch.nodeList.begin();
        for (i = branch.nodeList.begin(); i != branch.nodeList.end(); i++) {
            if (i->key > key) {
                i--;
                break;
            }
        }
        if (i == branch.nodeList.end())
            i--;
        datas = selectEqual(tableInfo, indexInfo, key, i->ptrChild);
    }
    return datas;
}

Data IndexManager::selectBetween(
    const Table &tableInfo, const Index &indexInfo, string keyFrom, string keyTo, int blockOffset) {
    Data datas;
    string fileName = indexInfo.indexName + ".index";
    int bufferID    = buf.getbufferID(fileName, blockOffset);
    bool isLeaf     = (buf.bufferBlock[bufferID].values[1] == 'L');  // L for leaf
    if (isLeaf) {
        for (;;) {
            Leaf leaf(bufferID, indexInfo);
            for (auto i = leaf.nodeList.begin(); i != leaf.nodeList.end(); i++) {
                if (i->key >= keyFrom) {
                    if (i->key > keyTo) {
                        return datas;
                    }
                    fileName            = indexInfo.table_name + ".table";
                    int recordBufferNum = buf.getbufferID(fileName, i->offsetFile);
                    int position        = (tableInfo.totalLength + 1) * (i->offsetBlock);
                    string stringrow =
                        buf.bufferBlock[recordBufferNum].getvalues(position, position + tableInfo.totalLength);
                    if (stringrow.c_str()[0] != EMPTY) {
                        stringrow.erase(stringrow.begin());
                        Row splitedRow = splitRow(tableInfo, stringrow);
                        datas.rowList.push_back(splitedRow);
                    }
                }
            }
            if (leaf.nextSib != 0) {
                fileName = indexInfo.indexName + ".index";
                bufferID = buf.getbufferID(fileName, leaf.nextSib);
            } else
                return datas;
        }
    } else {  // Not leaf, go down to the leaf
        Branch branch(bufferID, indexInfo);
        auto i = branch.nodeList.begin();
        if (i->key > keyFrom) {
            datas = selectBetween(tableInfo, indexInfo, keyFrom, keyTo, i->ptrChild);
            return datas;
        } else {
            for (i = branch.nodeList.begin(); i != branch.nodeList.end(); i++) {
                if (i->key > keyFrom) {
                    i--;  //�õ�(*i) ��ߵ�ָ���λ��
                    break;
                }
            }
            datas = selectBetween(tableInfo, indexInfo, keyFrom, keyTo, i->ptrChild);
            return datas;
        }
    }
    return datas;
}

Row IndexManager::splitRow(Table tableInfo, string row) {
    Row splitedRow;
    int s_pos = 0, f_pos = 0;  // Start position & finish position
    for (int i = 0; i < tableInfo.attriNum; i++) {
        s_pos = f_pos;
        f_pos += tableInfo.attributes[i].length;
        string col;
        for (int j = s_pos; j < f_pos; j++) {
            if (row[j] == EMPTY)
                break;
            col += row[j];
        }
        splitedRow.columnList.push_back(col);
    }
    return splitedRow;
}

string IndexManager::getColumnValue(const Table &tableInfo, const Index &indexInfo, string row) {
    string colValue;
    int s_pos = 0, f_pos = 0;  // Start position & finish position
    for (int i = 0; i <= indexInfo.column; i++) {
        s_pos = f_pos;
        f_pos += tableInfo.attributes[i].length;
    }
    for (int j = s_pos; j < f_pos && row[j] != EMPTY; j++)
        colValue += row[j];
    return colValue;
}

void IndexManager::dropIndex(Index &indexInfo) {
    string fileName = indexInfo.indexName + ".index";
    if (remove(fileName.c_str()) != 0)
        perror("Error deleting file");
    else
        buf.setInvalid(fileName);
}
