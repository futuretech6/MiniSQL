#include "MiniSQL.h"
#include "BufferManager.h"
#include "CatalogManager.h"
#include "IndexManager.h"
#include "Interpreter.h"
#include "RecordManager.h"
#include <iostream>

using namespace std;

int IsComEnd(char *input);
void executeCommand(void);
void printResults(Data &data, Table &tableInfor, vector<Attribute> &columnSelected);

RecordManager record;
IndexManager indexMan;
CatalogManager catalog;
Interpreter inter;
BufferManager buf;
string command;

int main() {
    cout << endl;
    cout << "\t\t***********************************************" << endl;
    cout << "\t\t             欢迎使用 MiniSQL !" << endl;
    cout << "\t\t               Version (1.0)  " << endl;
    cout << "\t\t            " << endl;
    cout << "\t\t        Copyright(2020) all right reserved !" << endl;
    cout << "\t\t***********************************************" << endl;
    cout << endl << endl;

    char input[510];
    int flag;
    while (1) {
        flag    = 0;
        command = "";
        cout << ">>> ";
        while (!flag) {
            cin.getline(input, 500);
            flag = IsComEnd(input);
            command += input;
        }
        inter.parseCommand(command);
        executeCommand();
    }
    return 0;
}

int IsComEnd(char *input) {
    int len   = strlen(input);
    int index = len - 1;
    while (input[index] == ' ' || input[index] == '\t' || input[index] == '\n') {
        index--;
    }
    if (input[index] == ';') {
        input[index + 1] = '\0';
        return 1;
    }
    input[index + 1] = ' ';  //确保换行符会变成一个空格，使得行与行之间的内容存在空格
    input[index + 2] = '\0';
    return 0;
}

void executeCommand(void) {
    string tmp = "";
    Data tmpData;
    // Data testData;
    int count;
    // vector<Attribute> testAttributes;
    // Attribute testAttri;
    Index tmpIndex;
    // testAttri.name = "*";
    // testAttributes.push_back(testAttri);
    int dupFlag = 0;
    tmpData.rowList.clear();
    fstream fin(inter.fileName, ios::in);
    int flag         = 0;
    char buffer[510] = {0};
    string tmpCommand;
    if (command.size() > 0)
        command.pop_back();

    switch (inter.op) {
        case QUIT:
            cout << "Press any key to quit." << endl;
            getchar();
            exit(0);
            break;
        case SELECT_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on select" << endl;
            break;
        case TABLE_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "Error : Table does not exists : " << inter.tableName << endl;
            break;
        case INDEX_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "Error : Index does not exists : " << inter.indexName << endl;
            break;
        case EMPTY_COMMAND: cout << "(empty)" << endl; break;
        case TYPE_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            command.push_back(';');
            getFirstWord(command, tmp);
            cout << "TypeError: Unkown keyword : " << tmp << endl;
            break;
        case CREATE_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on create" << endl;
            break;
        case CREATE_TABLE_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on create table" << endl;
            break;
        case CREATE_INDEX_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on create index" << endl;
            break;
        case DELETE_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on delete" << endl;
            break;
        case DROP_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on drop" << endl;
            break;
        case DROP_TABLE_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on drop table" << endl;
            break;
        case DROP_INDEX_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on drop index" << endl;
            break;
        case INVALID_PRIMARY_KEY:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "Error: Primary key is invalid" << endl;
            break;
        case CHAR_BOUND:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "TyprError: Data type char is out of bound" << endl;
            break;
        case COLUMN_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "Error: Column does not exists: " << inter.columnName << endl;
            break;
        case INSERT_NUMBER_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "Error: Incorrect number of attributes to insert data" << endl;
            break;
        case TABLE_EXISTED:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "Error : Can not create table '" << inter.tableName
                 << "', because it already exists in the database" << endl;
            break;
        case INDEX_EXISTED:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "Error : Can not create index '" << inter.indexName
                 << "', because it already exists in the database" << endl;
            break;
        case INSERT_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on insert" << endl;
            break;
        case DATA_TYPE_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Data Type '" << inter.columnName << "' is not allowed" << endl;
            break;
        case EXECFILE_ERROR:
            cout << "ERROR when execute SQL : " << command << endl;
            cout << "SyntaxError: Parse error on execfile " << endl;
            break;
        case CREATE_TABLE:
            catalog.createTable(inter.tableInfor);
            record.createTable(inter.tableInfor);
            cout << "Query OK, 0 row affected " << endl;
            cout << "Table '" << inter.tableName << "' is created successfully " << endl;
            break;
        case CREATE_INDEX:
            if (!inter.tableInfor.attributes[inter.indexInfor.column].isPrimaryKey &&
                !inter.tableInfor.attributes[inter.indexInfor.column].isUnique) {
                cout << "ERROR when execute SQL : " << command << endl;
                cout << "Error: Column '" << inter.tableInfor.attributes[inter.indexInfor.column].name
                     << "' is not unique" << endl;
                break;
            }
            catalog.createIndex(inter.indexInfor);
            indexMan.createIndex(inter.tableInfor, inter.indexInfor);
            catalog.updateIndex(inter.indexInfor);
            cout << "Query OK, 0 row affected " << endl;
            cout << "Index '" << inter.indexName << "' is created successfully " << endl;
            break;
        case INSERT:
            /*cout << "---------" << endl;
            testData.rowList.clear();
            testData.rowList.push_back(inter.recordData);
            printResults(testData, inter.tableInfor, testAttributes);
            cout << "---------" << endl;*/
            for (int i = 0; i < inter.tableInfor.attributes.size(); i++) {
                // cout << "here!" << endl;
                if (inter.tableInfor.attributes[i].isUnique) {
                    // cout << "here inside!" << endl;
                    Condition tmpCond;
                    tmpCond.column1 = i;
                    tmpCond.op1     = Eq;
                    tmpCond.value1  = inter.recordData.columnList[i];
                    // cout << "value : " << tmpCond.value1 << endl;
                    tmpCond.relation = None;
                    tmpData.rowList.clear();
                    record.selectRecords(inter.tableInfor, tmpData, tmpCond);
                    if (tmpData.rowList.size() > 0) {
                        cout << "ERROR when execute SQL : " << command << endl;
                        cout << "Error: Column '" << inter.tableInfor.attributes[i].name << "' is duplicated" << endl;
                        dupFlag = 1;
                        break;
                    }
                }
            }
            if (dupFlag)
                break;
            record.insertRecords(inter.tableInfor, inter.recordData);
            catalog.updateTable(inter.tableInfor);
            cout << "Query OK, 1 row affected " << endl;
            cout << "Table '" << inter.tableName << "' is updated successfully " << endl;
            break;
        case SELECT_NOWHERE_CLAUSE:
            tmpData.rowList.clear();
            record.selectRecords(inter.tableInfor, tmpData);
            printResults(tmpData, inter.tableInfor, inter.attributes);
            if (tmpData.rowList.size() == 1)
                cout << "1 row in set " << endl;
            else
                cout << tmpData.rowList.size() << " rows in set " << endl;
            break;
            break;
        case SELECT_WHERE_CLAUSE:
            if (inter.condition.relation == None)  // only one condition
            {
                for (int i = 0; i < inter.tableInfor.attributes.size(); i++) {
                    if (inter.columnName == inter.tableInfor.attributes[i].name && inter.condition.op1 == Eq &&
                        inter.condition.column1 == i &&
                        (inter.tableInfor.attributes[i].isPrimaryKey || inter.tableInfor.attributes[i].isUnique)) {
                        inter.indexInfor = catalog.getIndexInfor(inter.tableName, i);
                        tmpData = indexMan.selectEqual(inter.tableInfor, inter.indexInfor, inter.condition.value1);
                        break;
                    }
                }
                record.selectRecords(inter.tableInfor, tmpData, inter.condition);
            } else
                record.selectRecords(inter.tableInfor, tmpData, inter.condition);
            printResults(tmpData, inter.tableInfor, inter.attributes);
            if (tmpData.rowList.size() == 1)
                cout << "1 row in set " << endl;
            else
                cout << tmpData.rowList.size() << " rows in set " << endl;
            break;
        case DELETE_NOWHERE_CLAUSE:
            count = record.deleteRecords(inter.tableInfor);
            if (count == 1)
                cout << "Query OK, 1 row affected" << endl;
            else
                cout << "Query OK, " << count << " rows affected" << endl;
            cout << "Table '" << inter.tableName << "' is updated successfully " << endl;
            break;
        case DELETE_WHERE_CLAUSE:
            count = record.deleteRecords(inter.tableInfor, inter.condition);
            if (count == 1)
                cout << "Query OK, 1 row affected" << endl;
            else
                cout << "Query OK, " << count << " rows affected" << endl;
            cout << "Table '" << inter.tableName << "' is updated successfully " << endl;
            break;
        case EXECFILE:
            if (!fin.is_open()) {
                cout << "ERROR when execute SQL : " << command << endl;
                cout << "Error: file does not exist " << endl;
                break;
            }
            while (!fin.eof()) {
                tmpCommand = "";
                while (!flag && !fin.eof()) {
                    fin.getline(buffer, 500);
                    flag = IsComEnd(buffer);
                    tmpCommand += buffer;
                    // cout << buffer << endl;
                }
                // cout << tmpCommand << endl;
                command = tmpCommand;
                flag    = 0;
                inter.parseCommand(tmpCommand);
                executeCommand();
            }
            break;
        case DROP_TABLE:
            record.dropTable(inter.tableInfor);
            /*for (int i = 0; i < inter.tableInfor.attriNum; i++)
            {
                tmpIndex = catalog.getIndexInfor(inter.tableInfor.name, i);
                if (tmpIndex.indexName != "")
                {
                    index.dropIndex(tmpIndex);
                }
            }*/
            catalog.dropTable(inter.tableName);
            cout << "Query OK, table '" << inter.tableName << "' is dropped successfully " << endl;
            break;
        case DROP_INDEX:
            tmpIndex = catalog.getIndexInfor(inter.indexName);
            indexMan.dropIndex(tmpIndex);
            catalog.dropIndex(inter.indexName);
            cout << "Query OK, index '" << inter.indexName << "' is dropped successfully " << endl;
            break;
        default: break;
    }
}

static int getUtf8LetterNumber(string &raw_s) {
    const char *s = raw_s.c_str();
    int i = 0, j = 0;
    while (s[i]) {
        if ((s[i] & 0xc0) != 0x80)
            j++;
        i++;
    }
    return j;
}

int getLength(string &input) {
    int len_char = getUtf8LetterNumber(input);  //字符数
    int len_byte = input.length();              //字节数
    if (len_char == len_byte)
        return len_byte;
    else if (len_char == 2 * len_byte) {
        return (UTF8_WIDTH - 1) * (double)len_byte;
    } else {
        return (UTF8_WIDTH - 1) * (double)len_byte;
    }
}

void printResults(Data &data, Table &tableInfor, vector<Attribute> &columnSelected) {
    if (columnSelected[0].name == "*")  // select *
    {
        /*构建表头*/
        cout << "\n+";
        for (int i = 0; i < tableInfor.attriNum; i++) {
            string tmp = "";
            for (int j = 0; j <= tableInfor.attributes[i].length; j++) {
                tmp += "-";
            }
            cout << tmp << "+";
        }
        cout << endl << "|";
        for (int i = 0; i < tableInfor.attriNum; i++) {
            cout << " " << tableInfor.attributes[i].name;
            string tmp    = "";
            string ttmmpp = tableInfor.attributes[i].name;
            for (int j = 0; j < tableInfor.attributes[i].length - getLength(ttmmpp); j++)
            // 最大长度减去已显示的长度，保证对齐
            {
                tmp += " ";
            }
            cout << tmp << "|";
        }
        cout << endl << "+";
        for (int i = 0; i < tableInfor.attriNum; i++) {
            string tmp = "";
            for (int j = 0; j < tableInfor.attributes[i].length + 1; j++) {
                tmp += "-";
            }
            cout << tmp << "+";
        }
        cout << endl;
        // 显示数据
        for (int i = 0; i < data.rowList.size(); i++) {
            cout << "| ";
            for (int j = 0; j < tableInfor.attriNum; j++) {
                string out = data.rowList[i].columnList[j];
                cout << out;
                string tmp     = "";
                int lengthLeft = tableInfor.attributes[j].length - out.length();
                for (int k = 0; k < lengthLeft + out.length() - getLength(out); k++)
                    tmp += " ";
                cout << tmp << "| ";
            }
            cout << endl;
        }
        // 显示表格框架的最后一行字符
        cout << "+";
        for (int i = 0; i < tableInfor.attriNum; i++) {
            string tmp = "";
            for (int j = 0; j < tableInfor.attributes[i].length + 1; j++) {
                tmp += "-";
            }
            cout << tmp << "+";
        }
        cout << endl;
    } else {
        // 保存要显示的列的序号
        vector<int> columnNumList;
        for (int i = 0; i < columnSelected.size(); i++) {
            columnNumList.push_back(getColumnID(tableInfor, columnSelected[i].name));
        }
        // 显示表头
        cout << "\n+";
        for (int i = 0; i < columnNumList.size(); i++) {
            string tmp = "";
            for (int j = 0; j < tableInfor.attributes[i].length + 1; j++) {
                tmp += "-";
            }
            cout << tmp << "+";
        }
        cout << endl << "|";
        for (int i = 0; i < columnNumList.size(); i++) {
            cout << " " << tableInfor.attributes[columnNumList[i]].name;
            string tmp = "";
            int lengthLeft =
                tableInfor.attributes[columnNumList[i]].length - tableInfor.attributes[columnNumList[i]].name.length();
            string ttmmpp = tableInfor.attributes[columnNumList[i]].name;
            for (int j = 0; j < lengthLeft + ttmmpp.length() - getLength(ttmmpp); j++) {
                tmp += " ";
            }
            cout << tmp << "|";
        }
        cout << endl << "+";
        for (int i = 0; i < columnNumList.size(); i++) {
            string tmp = "";
            for (int j = 0; j < tableInfor.attributes[i].length + 1; j++) {
                tmp += "-";
            }
            cout << tmp << "+";
        }
        cout << endl;
        // 显示数据
        for (int i = 0; i < data.rowList.size(); i++) {
            cout << "| ";
            for (int j = 0; j < columnNumList.size(); j++) {
                string out = data.rowList[i].columnList[columnNumList[j]];
                cout << out;
                int lengthLeft = tableInfor.attributes[j].length - out.length();
                string tmp     = "";
                for (int k = 0; k < lengthLeft + out.length() - getLength(out); k++) {
                    tmp += " ";
                }
                cout << tmp << "| ";
            }
            cout << endl /*<< "|"*/;
        }
        // 显示表格框架的最后一行字符
        cout << "+";
        for (int i = 0; i < columnNumList.size(); i++) {
            string tmp = "";
            for (int j = 0; j < tableInfor.attributes[i].length + 1; j++) {
                tmp += "-";
            }
            cout << tmp << "+";
        }
        cout << endl;
    }
}