#include "IndexManager.h"

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

Branch::Branch(int vbufNum, const Index &indexInfo) {
    bufferID        = vbufNum;
    isRoot          = (buf.bufferBlock[bufferID].values[0] == 'R');
    int recordCount = getRecordNum();
    recordNum = 0;  // RecordNum will increase when function insert is called, and finally as large
                    // As recordCount
    ptrFather    = getPtr(6);
    columnLength = indexInfo.columnLength;
    int position = 6 + POINTER_LENGTH;
    for (int i = 0; i < recordCount; i++) {
        string key = "";
        for (int i = position; i < position + columnLength; i++) {
            if (buf.bufferBlock[bufferID].values[i] == EMPTY)
                break;
            else
                key += buf.bufferBlock[bufferID].values[i];
        }
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
    // Itoa(recordNum, tmpt, 10);
    sprintf(tmpt, "%d", recordNum);
    string strRecordNum = tmpt;
    while (strRecordNum.length() < 4)
        strRecordNum = '0' + strRecordNum;
    strncpy(buf.bufferBlock[bufferID].values + 2, strRecordNum.c_str(), 4);

    // IndexList
    if (indexList.size() == 0) {
        cout << "Undefined action!" << endl;
        exit(0);
    }

    int position = 6 + POINTER_LENGTH;
    for (auto i = indexList.begin(); i != indexList.end(); i++) {
        string key = i->key;
        while (key.length() < columnLength)
            key += EMPTY;
        strncpy(buf.bufferBlock[bufferID].values + position, key.c_str(), columnLength);
        position += columnLength;

        char tmpt[5];
        sprintf(tmpt, "%d", i->ptrChild);

        string ptrChild = tmpt;
        while (ptrChild.length() < POINTER_LENGTH)
            ptrChild = '0' + ptrChild;
        strncpy(buf.bufferBlock[bufferID].values + position, ptrChild.c_str(), POINTER_LENGTH);
        position += POINTER_LENGTH;
    }
}
void Branch::insert(BranchIndex node) {
    recordNum++;
    auto i = indexList.begin();
    if (indexList.size() == 0)
        indexList.insert(i, node);
    else {
        for (i = indexList.begin(); i != indexList.end(); i++)
            if (i->key > node.key)
                break;
        indexList.insert(i, node);
    }
}

BranchIndex Branch::pop() {
    recordNum--;
    BranchIndex tmpt = indexList.back();
    indexList.pop_back();
    return tmpt;
}

Leaf::Leaf(int vbufNum) {  // This kind of leaf is added when old leaf is needed to be splited
    bufferID  = vbufNum;
    recordNum = 0;
    nextSib = prevSib = 0;
}
Leaf::Leaf(int vbufNum, const Index &indexInfo) {
    bufferID        = vbufNum;
    isRoot          = (buf.bufferBlock[bufferID].values[0] == 'R');
    int recordCount = getRecordNum();
    recordNum       = 0;
    ptrFather       = getPtr(6);
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
        int offsetInFile = getPtr(position);
        position += POINTER_LENGTH;
        int offsetInBlock = getPtr(position);
        position += POINTER_LENGTH;
        LeafIndex node(key, offsetInFile, offsetInBlock);
        insert(node);
    }
}
Leaf::~Leaf() {
    // IsRoot
    if (isRoot)
        buf.bufferBlock[bufferID].values[0] = 'R';
    else
        buf.bufferBlock[bufferID].values[0] = '_';
    // IsLeaf
    buf.bufferBlock[bufferID].values[1] = 'L';
    // RecordNum
    char tmpt[5];
    sprintf(tmpt, "%d", recordNum);
    string strRecordNum = tmpt;
    while (strRecordNum.length() < 4)
        strRecordNum = '0' + strRecordNum;
    int position = 2;
    strncpy(buf.bufferBlock[bufferID].values + position, strRecordNum.c_str(), 4);
    position += 4;
    sprintf(tmpt, "%d", ptrFather);
    string strptrFather = tmpt;
    while (strptrFather.length() < POINTER_LENGTH)
        strptrFather = '0' + strptrFather;
    strncpy(buf.bufferBlock[bufferID].values + position, strptrFather.c_str(), POINTER_LENGTH);
    position += POINTER_LENGTH;

    sprintf(tmpt, "%d", prevSib);
    string strLastSibling = tmpt;
    while (strLastSibling.length() < POINTER_LENGTH)
        strLastSibling = '0' + strLastSibling;
    strncpy(buf.bufferBlock[bufferID].values + position, strLastSibling.c_str(), POINTER_LENGTH);
    position += POINTER_LENGTH;

    sprintf(tmpt, "%d", nextSib);
    string strNextSibling = tmpt;
    while (strNextSibling.length() < POINTER_LENGTH)
        strNextSibling = '0' + strNextSibling;
    strncpy(buf.bufferBlock[bufferID].values + position, strNextSibling.c_str(), POINTER_LENGTH);
    position += POINTER_LENGTH;

    // IndexList
    if (indexList.size() == 0) {
        cout << "Undefined action!" << endl;
        exit(0);
    }

    for (auto &i : indexList) {
        string key = i.key;
        while (key.length() < columnLength)
            key += EMPTY;
        strncpy(buf.bufferBlock[bufferID].values + position, key.c_str(), columnLength);
        position += columnLength;

        sprintf(tmpt, "%d", i.offsetInBlock);

        string offsetInFile = tmpt;
        while (offsetInFile.length() < POINTER_LENGTH)
            offsetInFile = '0' + offsetInFile;
        strncpy(buf.bufferBlock[bufferID].values + position, offsetInFile.c_str(), POINTER_LENGTH);
        position += POINTER_LENGTH;

        sprintf(tmpt, "%d", i.offsetInBlock);

        string offsetInBlock = tmpt;
        while (offsetInBlock.length() < POINTER_LENGTH)
            offsetInBlock = '0' + offsetInBlock;
        strncpy(buf.bufferBlock[bufferID].values + position, offsetInBlock.c_str(), POINTER_LENGTH);
        position += POINTER_LENGTH;
    }
}

void Leaf::insert(LeafIndex node) {
    recordNum++;
    auto i = indexList.begin();
    if (indexList.size() == 0) {
        indexList.insert(i, node);
        return;
    } else {
        for (i = indexList.begin(); i != indexList.end(); i++)
            if (i->key > node.key)
                break;
    }
    indexList.insert(i, node);
}
LeafIndex Leaf::pop() {
    recordNum--;
    LeafIndex tmpt = indexList.back();
    indexList.pop_back();
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
    for (int blockOffset = 0; blockOffset < tableInfo.blockID; blockOffset++) {
        int bufferID = buf.getIfIsInBuffer(fileName, blockOffset);
        if (bufferID == -1) {
            bufferID = buf.getEmptyBuffer();
            buf.readBlock(fileName, blockOffset, bufferID);
        }
        for (int offset = 0; offset < recordNum; offset++) {
            int position = offset * length;
            stringrow    = buf.bufferBlock[bufferID].getvalues(position, position + length);
            if (stringrow.c_str()[0] == EMPTY)
                continue;  // Inticate that this row of record have been deleted
            stringrow.erase(stringrow.begin());
            key = getColumnValue(tableInfo, indexInfo, stringrow);
            LeafIndex node(key, blockOffset, offset);
            insertValue(indexInfo, node);
        }
    }
}

BranchIndex IndexManager::insertValue(Index &indexInfo, LeafIndex node, int blockOffset) {
    BranchIndex reBranch;  // For return, intial to be empty
    string fileName = indexInfo.indexName + ".index";
    int bufferID    = buf.getbufferID(fileName, blockOffset);
    bool isLeaf     = (buf.bufferBlock[bufferID].values[1] == 'L');  // L for leaf
    if (isLeaf) {
        Leaf leaf(bufferID, indexInfo);
        leaf.insert(node);

        const int RecordLength = indexInfo.columnLength + POINTER_LENGTH * 2;
        const int MaxrecordNum = (BLOCK_SIZE - 6 - POINTER_LENGTH * 3) / RecordLength;
        if (leaf.recordNum > MaxrecordNum) {    // Record number is too great, need to split
            if (leaf.isRoot) {                  // This leaf is a root
                int rbufferID = leaf.bufferID;  // Buffer number for new root
                leaf.bufferID = buf.addBlockInFile(indexInfo);  // Find a new place for old leaf
                int sbufferID = buf.addBlockInFile(indexInfo);  // Buffer number for sibling
                Branch branchRoot(rbufferID);                   // New root, which is branch indeed
                Leaf leafadd(sbufferID);                        // Sibling

                // Is root
                branchRoot.isRoot = 1;
                leafadd.isRoot    = 0;
                leaf.isRoot       = 0;

                branchRoot.ptrFather = leafadd.ptrFather = leaf.ptrFather = 0;
                branchRoot.columnLength = leafadd.columnLength = leaf.columnLength;
                // Link the newly added leaf block in the link list of leaf
                leafadd.prevSib = buf.bufferBlock[leaf.bufferID].blockOffset;
                leaf.nextSib    = buf.bufferBlock[leafadd.bufferID].blockOffset;
                while (leafadd.indexList.size() < leaf.indexList.size()) {
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
                return reBranch;
            } else {  // This leaf is not a root, we have to cascade up
                int bufferID = buf.addBlockInFile(indexInfo);
                Leaf leafadd(bufferID);
                leafadd.isRoot       = 0;
                leafadd.ptrFather    = leaf.ptrFather;
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
                while (leafadd.indexList.size() < leaf.indexList.size()) {
                    LeafIndex tnode = leaf.pop();
                    leafadd.insert(tnode);
                }
                reBranch.key      = leafadd.getFront().key;
                reBranch.ptrChild = leaf.nextSib;
                return reBranch;
            }
        } else {  // Not need to split,just return
            return reBranch;
        }

    } else {  // Not a leaf
        Branch branch(bufferID, indexInfo);
        auto i = branch.indexList.begin();
        if (i->key > node.key) {
            i->key = node.key;
        } else {
            for (i = branch.indexList.begin(); i != branch.indexList.end(); i++)
                if (i->key > node.key)
                    break;
            i--;
        }
        BranchIndex bnode = insertValue(indexInfo, node, i->ptrChild);  // Go down

        if (bnode.key == "") {
            return reBranch;
        } else {  // Bnode.key != "",need to split up
            branch.insert(bnode);
            const int RecordLength = indexInfo.columnLength + POINTER_LENGTH;
            const int MaxrecordNum = (BLOCK_SIZE - 6 - POINTER_LENGTH) / RecordLength;
            if (branch.recordNum > MaxrecordNum) {  // Need to split up
                if (branch.isRoot) {
                    int rbufferID = branch.bufferID;  // Buffer number for new root
                    branch.bufferID =
                        buf.addBlockInFile(indexInfo);  // Find a new place for old branch
                    int sbufferID = buf.addBlockInFile(indexInfo);  // Buffer number for sibling
                    Branch branchRoot(rbufferID);                   // New root
                    Branch branchadd(sbufferID);                    // Sibling

                    // Is root
                    branchRoot.isRoot = 1;
                    branchadd.isRoot  = 0;
                    branch.isRoot     = 0;

                    branchRoot.ptrFather = branchadd.ptrFather = branch.ptrFather = 0;
                    branchRoot.columnLength = branchadd.columnLength = branch.columnLength;

                    while (branchadd.indexList.size() < branch.indexList.size()) {
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
                    return reBranch;  // Here the function must have already returned to the top lay
                } else {              // Branch is not a root
                    int bufferID = buf.addBlockInFile(indexInfo);
                    Branch branchadd(bufferID);
                    branchadd.isRoot       = 0;
                    branchadd.ptrFather    = branch.ptrFather;
                    branchadd.columnLength = branch.columnLength;

                    while (branchadd.indexList.size() < branch.indexList.size()) {
                        BranchIndex tnode = branch.pop();
                        branchadd.insert(tnode);
                    }
                    reBranch.key      = branchadd.getFront().key;
                    reBranch.ptrChild = buf.bufferBlock[bufferID].blockOffset;
                    return reBranch;
                }
            } else {  // Not need to split,just return
                return reBranch;
            }
        }
    }
    return reBranch;  // Here is just for safe
}

Data IndexManager::selectEqual(const Table &tableInfo, const Index &indexInfo, string key,
    int blockOffset) {  // Start from the root and look down
    Data datas;
    string fileName = indexInfo.indexName + ".index";
    int bufferID    = buf.getbufferID(fileName, blockOffset);
    bool isLeaf     = (buf.bufferBlock[bufferID].values[1] == 'L');  // L for leaf
    if (isLeaf) {
        Leaf leaf(bufferID, indexInfo);
        auto i = leaf.indexList.begin();
        for (i = leaf.indexList.begin(); i != leaf.indexList.end(); i++)
            if (i->key == key) {
                fileName            = indexInfo.table_name + ".table";
                int recordBufferNum = buf.getbufferID(fileName, i->offsetInFile);
                int position        = (tableInfo.totalLength + 1) * (i->offsetInBlock);
                string stringrow    = buf.bufferBlock[recordBufferNum].getvalues(
                    position, position + tableInfo.totalLength);
                if (stringrow.c_str()[0] != EMPTY) {
                    stringrow.erase(stringrow.begin());
                    Row splitedRow = splitRow(tableInfo, stringrow);
                    datas.rowList.push_back(splitedRow);
                    return datas;
                }
            }
    } else {  // It is not a leaf
        Branch branch(bufferID, indexInfo);
        auto i = branch.indexList.begin();
        for (i = branch.indexList.begin(); i != branch.indexList.end(); i++) {
            if (i->key > key) {
                i--;
                break;
            }
        }
        if (i == branch.indexList.end())
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
        do {
            Leaf leaf(bufferID, indexInfo);

            for (auto &i : leaf.indexList) {
                if (i.key >= keyFrom) {
                    if (i.key > keyTo) {
                        return datas;
                    }
                    fileName            = indexInfo.table_name + ".table";
                    int recordBufferNum = buf.getbufferID(fileName, i.offsetInFile);
                    int position        = (tableInfo.totalLength + 1) * (i.offsetInBlock);
                    string stringrow    = buf.bufferBlock[recordBufferNum].getvalues(
                        position, position + tableInfo.totalLength);
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
        } while (1);
    } else {  // Not leaf, go down to the leaf
        Branch branch(bufferID, indexInfo);
        auto i = branch.indexList.begin();
        if (i->key > keyFrom) {
            datas = selectBetween(tableInfo, indexInfo, keyFrom, keyTo, i->ptrChild);
            return datas;
        } else {
            for (i = branch.indexList.begin(); i != branch.indexList.end(); i++)
                if (i->key > keyFrom) {
                    i--;  //�õ�(*i) ��ߵ�ָ���λ��
                    break;
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
