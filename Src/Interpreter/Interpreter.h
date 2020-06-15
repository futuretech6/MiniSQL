#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "CatalogManager.h"
#include "IndexManager.h"
#include "MiniSQL.h"
#include "RecordManager.h"

extern CatalogManager catalog;
extern RecordManager record;

class Interpreter {
  public:
    string tableName;
    string indexName;
    string columnName;
    string fileName;
    int op;
    Row recordData;
    Table tableInfor;
    Index indexInfor;
    Condition condition;
    vector<Attribute> attributes;
    int primaryKeyPos;
    int uniquePos;

  public:
    Interpreter();
    ~Interpreter() {}
    void parseCommand(string &command);
    void initInterpreter();
};
int getFirstWord(string &command, string &firstWord);
#endif