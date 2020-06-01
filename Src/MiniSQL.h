#ifndef MINISQL_H
#define MINISQL_H

#include <cstdio>
#include <cstdlib>
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

const int MAX_BLOCK_NUMBER = 1000;  // Should be 10000
const int BLOCK_SIZE       = 4096;
const char EMPTY           = '@';
const char END             = '@';
const char NOT_EMPTY       = '1';
const char DELETED         = '@';

const int MAX_PRIMARY_KEY_LENGTH = 25;  // Should change sometime

const int UNKNOW                = 12;
const int SELERR                = 13;
const int CREINDERR             = 14;
const int DELETEERR             = 15;
const int INSERTERR             = 16;
const int CRETABERR             = 17;
const int DRPTABERR             = 18;
const int DRPINDERR             = 19;
const int EXEFILERR             = 20;
const int VOIDPRI               = 21;
const int VOIDUNI               = 22;
const int CHARBOUD              = 23;
const int TABLEERROR            = 24;
const int COLUMNERROR           = 25;
const int INSERTNUMBERERROR     = 26;
const int NOPRIKEY              = 27;
const int SELECT_WHERE_CAULSE   = 28;
const int SELECT_NOWHERE_CAULSE = 29;
const int TABLEEXISTED          = 30;
const int INDEXERROR            = 31;
const int INDEXEROR             = 32;

const int SELECT  = 0;
const int CRETAB  = 1;
const int CREIND  = 2;
const int DRPTAB  = 3;
const int DRPIND  = 4;
const int DELETE  = 5;
const int INSERT  = 6;
const int QUIT    = 7;
const int EXEFILE = 8;

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
    bool isPrimeryKey;
    bool isUnique;
    Attribute() {
        isPrimeryKey = false;
        isUnique     = false;
    }
    Attribute(string n, int t, int l, bool isP, bool isU)
        : name(n), type(t), length(l), isPrimeryKey(isP), isUnique(isU) {}
};

class Table {
  public:
    string name;  // All the datas is store in file name.table
    int blockID;  // Number of block the datas of the table occupied in the file name.table
    // Int recordNum;    //number of record in name.table
    int attriNum;     // The number of attributes in the tables
    int totalLength;  // Total length of one record, should be equal to sum(attributes[i].length)
    vector<Attribute> attributes;
    Table() : blockID(0), attriNum(0), totalLength(0) {}
};

class Index {
  public:
    string indexName;   // All the datas is store in file index_name.index
    string table_name;  // The name of the table on which the index is create
    int column;         // On which column the index is created
    int columnLength;
    int blockID;  // Number of block the datas of the index occupied in the file index_name.table
    Index() : column(0), blockID(0) {}
};

class Row {
  public:
    vector<string> columnList;
};
class Data {
  public:
    vector<Row> rowList;
};

enum Comparison {
    Lt,
    Le,
    Gt,
    Ge,
    Eq,
    Ne
};  // Stants for less than, less equal, greater than, greater equal, equal, not equal respectivly

class Condition {
  public:
    Comparison op;
    int columnNum;
    string value;
};

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
    int bufferID;
    int position;
};

#endif