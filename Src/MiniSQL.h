#ifndef MINISQL_H
#define MINISQL_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

using namespace std;

const int INT         = 1;
const int FLOAT       = 2;
const int CHAR        = 3;
const int INTLENGTH   = 11;
const int FLOATLENGTH = 10;
const int INTLEN      = 11;
const int FLOATLEN    = 10;

const int IS_PRIMARY_KEY  = 1;
const int NOT_PRIMARY_KEY = 0;
const int IS_UNIQUE       = 1;
const int NOT_UNIQUE      = 2;

const int MAX_BLOCK_NUMBER = 10000;
const int BLOCK_SIZE       = 4096;
const char EMPTY           = '@';
const char END             = '@';
const char NOT_EMPTY       = '1';
const char DELETED         = '@';

const double UTF8_WIDTH = 1.5;

#define CACHE_FOLDER "./.DBFILE/"

const int MAX_PRIMARY_KEY_LENGTH = 25;

// Interpreter: op field
enum OP_FIELD {
    // 6 items each row
    SELECT,
    CREATE_TABLE,
    CREATE_INDEX,
    DROP_TABLE,
    DROP_INDEX,
    DELETE,
    INSERT,
    QUIT,
    EXECFILE,
    EMPTY_COMMAND,
    CREATE_ERROR,
    DATA_TYPE_ERROR,
    UNKNOWN,
    SELECT_ERROR,
    CREATE_INDEX_ERROR,
    DELETE_ERROR,
    INSERT_ERROR,
    CREATE_TABLE_ERROR,
    DROP_TABLE_ERROR,
    DROP_INDEX_ERROR,
    EXECFILE_ERROR,
    INVALID_PRIMARY_KEY,
    CHAR_BOUND,
    TABLE_ERROR,
    COLUMN_ERROR,
    INSERT_NUMBER_ERROR,
    SELECT_WHERE_CLAUSE,
    SELECT_NOWHERE_CLAUSE,
    TABLE_EXISTED,
    INDEX_EXISTED,
    INDEX_ERROR,
    DELETE_NOWHERE_CLAUSE,
    DELETE_WHERE_CLAUSE,
    DROP_ERROR,
    TYPE_ERROR
};

const int COMLEN         = 400;
const int INPUTLEN       = 200;
const int WORDLEN        = 100;
const int VALLEN         = 300;
const int NAMELEN        = 100;
const int POINTER_LENGTH = 5;

class Attribute {
  public:
    string name;
    int type;
    int length;
    bool isPrimaryKey;
    bool isUnique;
    Attribute() : name(""), type(INT), length(0) {
        isPrimaryKey = false;
        isUnique     = false;
    }
    Attribute(string n, int t, int l, bool isP, bool isU)
        : name(n), type(t), length(l), isPrimaryKey(isP), isUnique(isU) {}
};

class Table {
  public:
    string name;   // All the datas is store in file name.table
    int blockNum;  // Number of block the datas of the table occupied in the file name.table
    // Int recordNum;    //number of record in name.table
    int attriNum;     // The number of attributes in the tables
    int totalLength;  // Total length of one record, should be equal to sum(attributes[i].length)
    vector<Attribute> attributes;
    Table() : name(""), blockNum(0), attriNum(0), totalLength(0) {}

    void initTable() {
        name     = "";
        attriNum = blockNum = totalLength = 0;
        if (attributes.size() > 0)
            attributes.clear();
    }
};

class Index {
  public:
    string indexName;  // All the datas is store in file index_name.index
    string tableName;  // The name of the table on which the index is create
    int column;        // On which column the index is created
    int columnLength;
    int blockNum;  // Number of block the datas of the index occupied in the file index_name.table
    Index() : column(0), columnLength(0), blockNum(0) {}

    void initIndex() {
        blockNum = column = columnLength = 0;
        indexName                        = "";
        tableName                        = "";
    }
};

class Row {
  public:
    vector<string> columnList;
};

class Data {
  public:
    vector<Row> rowList;
};

enum Comparison { Lt, Le, Gt, Ge, Eq, Ne };  // <, <=, >, >=, =, !=

enum Relation { And, Or, None };  // None for only one condition

class Condition {
  public:
    Comparison op1;
    int column1;
    string value1;
    Relation relation;
    Comparison op2;
    int column2;
    string value2;

    Condition() : op1(Eq), column1(0), value1(""), relation(And), op2(Eq), column2(0), value2("") {}
    void initCond() {
        op1      = Eq;
        op2      = Eq;
        column1  = 0;
        column2  = 0;
        value1   = "";
        value2   = "";
        relation = None;
    }
};
// class Condition {
//  public:
//    Comparison op;
//    int columnNum;
//    string value;
//};

class buffer {
  public:
    bool isWritten;
    bool isValid;
    string fileName;
    int blockOffset;  // Block offset in file, position in file
    int LRUvalue;     // Least Recent Used. The lower, the newer
    char *values = new char[BLOCK_SIZE + 1];
    buffer() { initialize(); }
    void initialize() {
        isWritten   = 0;
        isValid     = 0;
        fileName    = "NULL";
        blockOffset = 0;
        LRUvalue    = 0;
        for (int i = 0; i < BLOCK_SIZE; i++)
            values[i] = EMPTY;
        values[BLOCK_SIZE] = '\0';
    }
    string getvalues(int startpos, int endpos) {
        string tmpt = "";
        if (startpos >= 0 && startpos <= endpos && endpos <= BLOCK_SIZE)
            copy(values + startpos, values + endpos, back_inserter(tmpt));
        // For (int i = startpos; i < endpos; i++)
        //     tmpt += values[i];
        return tmpt;
    }
    char getvalues(int pos) { return pos >= 0 && pos <= BLOCK_SIZE ? values[pos] : '\0'; }
};

class insertPos {
  public:
    int bufferID;  // buffer�еڼ���
    int position;  //�ÿ��еĵڼ�λ
};

#endif