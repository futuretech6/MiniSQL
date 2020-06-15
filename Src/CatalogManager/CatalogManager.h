#ifndef CatalogManager_h
#define CatalogManager_h

#include "MiniSQL.h"
class CatalogManager {
  private:
    vector<Table> tables;
    int tableNum;  // the number of tables in the database
    vector<Index> indexes;
    int indexNum;  // the number of indexes
  private:
    void LoadTableCatalog();
    void LoadIndexCatalog();
    void SaveTableCatalog();
    void SaveIndexCatalog();

  public:
    CatalogManager() {
        LoadTableCatalog();
        LoadIndexCatalog();
    }
    ~CatalogManager()  // ensure all changes will be saved
    {
        SaveTableCatalog();
        SaveIndexCatalog();
    }
    void createTable(const Table &table);
    void createIndex(const Index &index);
    void dropTable(string tableName);
    void dropIndex(string indexName);
    void updateTable(const Table &table);
    void updateIndex(const Index &index);
    bool existTable(string tableName);
    bool existIndex(string tableName, int column);
    bool existIndex(string indexName);
    Table getTableInfor(string tableName);
    Index getIndexInfor(string tableName, int column);
    Index getIndexInfor(string indexName);
    void printTableCatalog();
    void printIndexCatalog();
};

int getColumnID(Table &tableInfor, string columnName);
int getColumnAmount(const Table &tableInfor);
#endif