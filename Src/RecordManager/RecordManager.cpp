#include "RecordManager.h"

int check(const Table &tableInfor, const Row &row, int columnNum, Comparison op, string value) {
    switch (tableInfor.attributes[columnNum].type) {
        case INT: {
            switch (op) {
                case Lt: return atoi(row.columnList[columnNum].c_str()) < atoi(value.c_str()); break;
                case Le: return atoi(row.columnList[columnNum].c_str()) <= atoi(value.c_str()); break;
                case Gt: return atoi(row.columnList[columnNum].c_str()) > atoi(value.c_str()); break;
                case Ge: return atoi(row.columnList[columnNum].c_str()) >= atoi(value.c_str()); break;
                case Eq: return atoi(row.columnList[columnNum].c_str()) == atoi(value.c_str()); break;
                case Ne: return atoi(row.columnList[columnNum].c_str()) != atoi(value.c_str()); break;
                default: return -1; break;
            }
        }
        case FLOAT:
            switch (op) {
                case Lt: return atof(row.columnList[columnNum].c_str()) < atof(value.c_str()); break;
                case Le: return atof(row.columnList[columnNum].c_str()) <= atof(value.c_str()); break;
                case Gt: return atof(row.columnList[columnNum].c_str()) > atof(value.c_str()); break;
                case Ge: return atof(row.columnList[columnNum].c_str()) >= atof(value.c_str()); break;
                case Eq: return atof(row.columnList[columnNum].c_str()) == atof(value.c_str()); break;
                case Ne: return atof(row.columnList[columnNum].c_str()) != atof(value.c_str()); break;
                default: return -1; break;
            }
        case CHAR:
            switch (op) {
                case Lt: return row.columnList[columnNum] < value; break;
                case Le: return row.columnList[columnNum] <= value; break;
                case Gt: return row.columnList[columnNum] > value; break;
                case Ge: return row.columnList[columnNum] >= value; break;
                case Eq: return row.columnList[columnNum] == value; break;
                case Ne: return row.columnList[columnNum] != value; break;
                default: return -1; break;
            }
        default: return -1; break;
    }
}

/*
    return whether one row in table satisfies condition
*/
int RecordManager::comparator(const Table &tableInfor, const Row &row, const Condition &condition) {
    // cout << condition.column1 << endl;
    // cout << condition.op1 << endl;
    // cout << condition.value1 << endl;
    // cout << condition.relation << endl;
    // cout << condition.column2 << endl;
    // cout << condition.op2 << endl;
    // cout << condition.value2 << endl;
    int ans1 = check(tableInfor, row, condition.column1, condition.op1, condition.value1), ans2 = 0;
    if (condition.relation != None)
        ans2 = check(tableInfor, row, condition.column2, condition.op2, condition.value2);
    if (ans1 == -1 || ans2 == -1)  // condition invalid
    {
        return -1;
    }
    switch (condition.relation) {
        case And: return ans1 && ans2; break;
        case Or: return ans1 || ans2; break;
        case None: return ans1; break;
        default:
            return -1;  // relation invalid
            break;
    }
    return -1;
}

//����string��ʾ��һ��Ԫ��ת����ROW������������е�EMPTY��ȫ����
void RecordManager::splitRow(const Table &tableInfor, string &record, Row &row) {
    int spos = 0, epos = 0;  // start and end
    int i, j;
    string recordValue;  //һ��record�е�����һ�����Զ�Ӧ��ֵ
    //����ÿһ������
    for (i = 0; i < tableInfor.attriNum; i++) {
        recordValue = "";
        spos        = epos;
        epos        = epos + tableInfor.attributes[i].length;
        for (j = spos; j < epos && record[j] != EMPTY; j++) {
            recordValue += record[j];
        }
        row.columnList.push_back(recordValue);
    }
}

//����ROW�����ʾ��һ��Ԫ��ת����string��ʾ���Ҳ������е�EMPTY����
void RecordManager::combineRow(const Table &tableInfor, string &record, Row &row) {
    int i;
    string tmp;
    record = "";
    for (i = 0; i < tableInfor.attriNum; i++) {
        tmp = row.columnList[i];
        while (tmp.length() < tableInfor.attributes[i].length) {
            tmp += EMPTY;
        }
        record += tmp;
    }
}

//����ǰ��Ҫȷ��returnData�ǿյ�
void RecordManager::selectRecords(const Table &tableInfor, Data &returnData) {
    string fileName = tableInfor.name + ".table";
    string record;
    Row splitedRow;
    int length = tableInfor.totalLength + 1;  //���ڿ��е�һ����¼���ڵ�0λ��¼���Ƿ�ɾ��
    int recordNumInOneBlock = BLOCK_SIZE / length;
    //�������ű�����Ӧ�����ɿ飬����ÿ����
    for (int i = 0; i < tableInfor.blockNum; i++) {
        //Ѱ�Ҷ�Ӧ�ļ����ɿ��еĵ�i�飬��������buffer�ڵ�ID
        int bufferID = buf.getIfIsInBuffer(fileName, i);
        if (bufferID == -1) {
            bufferID = buf.getEmptyBuffer();
            buf.readBlock(fileName, i, bufferID);  //������buffer�У�����buffer�е�һ�飬������
        }
        //���ڸÿ��еĵ�j��record
        for (int j = 0; j < recordNumInOneBlock; j++) {
            splitedRow.columnList.clear();
            int pos = j * length;
            //�õ����е�ֵ
            record = buf.bufferBlock[bufferID].getvalues(pos, pos + length);
            if (record.at(0) == DELETED)
                continue;
            record.erase(record.begin());  //ɾ��ɾ���Ǻ�λ(0)ʹrecordֻ��Ԫ�鱾������Ϣ
            splitRow(tableInfor, record, splitedRow);  //��string��ʾ��Ԫ��ת����ROW����
            returnData.rowList.push_back(splitedRow);  //���ӵ�returnData������
        }
    }
}

void RecordManager::selectRecords(const Table &tableInfor, Data &returnData, const Condition &condition) {
    string fileName = tableInfor.name + ".table";
    string record;
    Row splitedRow;
    int length = tableInfor.totalLength + 1;  //���ڿ��е�һ����¼���ڵ�0λ��¼���Ƿ�ɾ��
    int recordNumInOneBlock = BLOCK_SIZE / length;
    //�������ű�����Ӧ�����ɿ飬����ÿ����
    for (int i = 0; i < tableInfor.blockNum; i++) {
        //Ѱ�Ҷ�Ӧ�ļ����ɿ��еĵ�i�飬��������buffer�ڵ�ID
        int bufferID = buf.getIfIsInBuffer(fileName, i);
        if (bufferID == -1) {
            bufferID = buf.getEmptyBuffer();
            buf.readBlock(fileName, i, bufferID);  //������buffer�У�����buffer�е�һ�飬������
        }
        //���ڸÿ��еĵ�j��record
        for (int j = 0; j < recordNumInOneBlock; j++) {
            if (splitedRow.columnList.size() > 0)
                splitedRow.columnList.clear();
            int pos = j * length;
            //�õ����е�ֵ
            record = buf.bufferBlock[bufferID].getvalues(pos, pos + length);
            if (record.at(0) == DELETED)
                continue;
            record.erase(record.begin());  //ɾ��ɾ���Ǻ�λ(0)ʹrecordֻ��Ԫ�鱾������Ϣ
            splitRow(tableInfor, record, splitedRow);  //��string��ʾ��Ԫ��ת����ROW����
            int resOfComparator = comparator(tableInfor, splitedRow, condition);
            if (resOfComparator == -1)
                printf("Select condition wrong in RecordManager!\n");
            else if (resOfComparator == 1)
                returnData.rowList.push_back(splitedRow);  //���ӵ�returnData������
        }
    }
}

void RecordManager::insertRecords(Table &tableInfor, Row &insertRow) {
    int i;
    string record = "";
    //���ݱ���Ϣ�õ�buffer�еĲ���λ��
    insertPos pos = buf.getInsertPosition(tableInfor);
    //��Row����ת����string
    combineRow(tableInfor, record, insertRow);
    buf.bufferBlock[pos.bufferID].values[pos.position] = NOT_EMPTY;
    for (i = 0; i < tableInfor.totalLength; i++) {
        buf.bufferBlock[pos.bufferID].values[pos.position + i + 1] = record[i];
    }
    buf.bufferBlock[pos.bufferID].isWritten = 1;
}

int RecordManager::deleteRecords(const Table &tableInfor) {
    string fileName         = tableInfor.name + ".table";
    int count               = 0;
    int length              = tableInfor.totalLength + 1;
    int recordNumInOneBlock = BLOCK_SIZE / length;
    for (int i = 0; i < tableInfor.blockNum; i++) {
        int bufferID = buf.getIfIsInBuffer(fileName, i);
        if (bufferID == -1) {
            bufferID = buf.getEmptyBuffer();
            buf.readBlock(fileName, i, bufferID);
        }
        for (int j = 0; j < recordNumInOneBlock; j++) {
            int pos = j * length;
            if (buf.bufferBlock[bufferID].values[pos] != DELETED) {
                buf.bufferBlock[bufferID].values[pos] = DELETED;
                count++;
            }
        }
    }
    return count;
}

int RecordManager::deleteRecords(const Table &tableInfor, const Condition &condition) {
    string fileName = tableInfor.name + ".table";
    string record;
    Row splitedRow;
    int count               = 0;
    int length              = tableInfor.totalLength + 1;
    int recordNumInOneBlock = BLOCK_SIZE / length;
    for (int i = 0; i < tableInfor.blockNum; i++) {
        int bufferID = buf.getIfIsInBuffer(fileName, i);
        if (bufferID == -1) {
            bufferID = buf.getEmptyBuffer();
            buf.readBlock(fileName, i, bufferID);
        }
        for (int j = 0; j < recordNumInOneBlock; j++) {
            if (splitedRow.columnList.size() > 0)
                splitedRow.columnList.clear();
            int pos = j * length;
            record  = buf.bufferBlock[bufferID].getvalues(pos, pos + length);
            if (record.at(0) != DELETED) {
                record.erase(record.begin());
                splitRow(tableInfor, record, splitedRow);
                int tmp = comparator(tableInfor, splitedRow, condition);
                // cout << "comparator : " << tmp << endl;
                if (tmp == 1) {
                    buf.bufferBlock[bufferID].values[pos] = DELETED;
                    count++;
                }
            }
        }
        buf.bufferBlock[bufferID].isWritten = 1;
    }
    return count;
}

void RecordManager::dropTable(Table &tableInfor) {
    string fileName = tableInfor.name + ".table";
    if (remove(fileName.c_str()) != 0) {
        perror("Drop table wrong in record manager!\n");
    } else {
        buf.setInvalid(fileName);
    }
}

void RecordManager::createTable(Table &tableInfor) {
    string fileName = tableInfor.name + ".table";
    fstream fout(fileName.c_str(), ios::out);  //���������Դ�ļ���
    fout.close();
}
