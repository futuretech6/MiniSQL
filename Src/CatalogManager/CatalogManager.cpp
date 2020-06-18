#include "CatalogManager.h"
/*
read table.catalog file and initialize the table list
*/
void CatalogManager::LoadTableCatalog() {
    const string fileName = "table.catlog";
    fstream fin(CACHE_FOLDER + fileName, ios::in);
    fin >> tableNum;
    for (int i = 0; i < tableNum; i++)  // read table information
    {
        Table temp_table;
        fin >> temp_table.name;
        fin >> temp_table.attriNum;
        fin >> temp_table.blockNum;
        for (int j = 0; j < temp_table.attriNum; j++)  // read attribute information
        {
            Attribute temp_attri;
            fin >> temp_attri.name;
            fin >> temp_attri.type;
            fin >> temp_attri.length;
            fin >> temp_attri.isPrimaryKey;
            fin >> temp_attri.isUnique;
            temp_table.attributes.push_back(temp_attri);
            temp_table.totalLength += temp_attri.length;
        }
        tables.push_back(temp_table);
    }
    fin.close();
}
/*
read index.catalog file and initialize the index list
*/
void CatalogManager::LoadIndexCatalog() {
    const string fileName = "index.catlog";
    fstream fin(CACHE_FOLDER + fileName, ios::in);
    fin >> indexNum;
    for (int i = 0; i < indexNum; i++)  // read index information
    {
        Index temp_index;
        fin >> temp_index.indexName;
        fin >> temp_index.tableName;
        fin >> temp_index.column;
        fin >> temp_index.columnLength;
        fin >> temp_index.blockNum;
        indexes.push_back(temp_index);
    }
    fin.close();
}

/*
save the changes made during running
*/
void CatalogManager::SaveTableCatalog() {
    string fileName = "table.catlog";
    fstream fout(CACHE_FOLDER + fileName, ios::out);

    fout << tableNum << endl;
    for (int i = 0; i < tableNum; i++) {
        fout << tables[i].name << " ";
        fout << tables[i].attriNum << " ";
        fout << tables[i].blockNum << endl;

        for (int j = 0; j < tables[i].attriNum; j++) {
            fout << tables[i].attributes[j].name << " ";
            fout << tables[i].attributes[j].type << " ";
            fout << tables[i].attributes[j].length << " ";
            fout << tables[i].attributes[j].isUnique << " ";
            fout << tables[i].attributes[j].isPrimaryKey << endl;
        }
    }
    fout.close();
}

void CatalogManager::SaveIndexCatalog() {
    string fileName = "index.catlog";
    fstream fout(CACHE_FOLDER + fileName, ios::out);
    fout << indexNum << endl;
    for (int i = 0; i < indexNum; i++) {
        fout << indexes[i].indexName << " ";
        fout << indexes[i].tableName << " ";
        fout << indexes[i].column << " ";
        fout << indexes[i].columnLength << " ";
        fout << indexes[i].blockNum << endl;
    }
    fout.close();
}

/*
call it when a table is created to record the table
*/
void CatalogManager::createTable(const Table &table) {
    tableNum++;
    tables.push_back(table);
}

/*
call it when an index is created to record the index
*/
void CatalogManager::createIndex(const Index &index) {
    indexNum++;
    indexes.push_back(index);
}

/*
call it when deleting table to clear records of about the table, table and indexes
*/
void CatalogManager::dropTable(string tableName) {
    vector<Table>::iterator iter = tables.begin();
    for (; iter != tables.end(); iter++) {
        if (iter->name == tableName) {
            tables.erase(iter);
            break;
        }
    }
    tableNum--;
    vector<Index>::iterator iter2 = indexes.begin();
    for (; iter2 != indexes.end();) {
        if (iter2->tableName == tableName)
            indexes.erase(iter2);
        else
            iter2++;
    }
}

void CatalogManager::dropIndex(string indexName) {
    vector<Index>::iterator iter2 = indexes.begin();
    for (; iter2 != indexes.end();) {
        if (iter2->tableName == indexName)
            indexes.erase(iter2);
        else
            iter2++;
    }
}

void CatalogManager::updateTable(const Table &table) {
    vector<Table>::iterator iter = tables.begin();
    for (; iter != tables.end(); iter++) {
        if (iter->name == table.name) {
            iter->attributes  = table.attributes;
            iter->attriNum    = table.attriNum;
            iter->blockNum    = table.blockNum;
            iter->totalLength = table.totalLength;
            break;
        }
    }
}

void CatalogManager::updateIndex(const Index &index) {
    vector<Index>::iterator iter = indexes.begin();
    for (; iter != indexes.end(); iter++) {
        if (iter->indexName == index.indexName) {
            iter->blockNum     = index.blockNum;
            iter->column       = index.column;
            iter->tableName    = index.tableName;
            iter->columnLength = index.columnLength;
        }
    }
}

bool CatalogManager::existTable(string tableName) {
    vector<Table>::iterator iter = tables.begin();
    for (; iter != tables.end(); iter++) {
        if (iter->name == tableName) {
            return true;
        }
    }
    return false;
}

bool CatalogManager::existIndex(string tableName, int column) {
    vector<Index>::iterator iter = indexes.begin();
    for (; iter != indexes.end(); iter++) {
        if (iter->tableName == tableName && iter->column == column) {
            return true;
        }
    }
    return false;
}

bool CatalogManager::existIndex(string indexName) {
    vector<Index>::iterator iter = indexes.begin();
    for (; iter != indexes.end(); iter++) {
        if (iter->indexName == indexName) {
            return true;
        }
    }
    return false;
}

Table CatalogManager::getTableInfor(string tableName) {
    vector<Table>::iterator iter = tables.begin();
    for (; iter != tables.end(); iter++) {
        if (iter->name == tableName) {
            return *iter;
        }
    }
    return Table();
}

Index CatalogManager::getIndexInfor(string tableName, int column) {
    vector<Index>::iterator iter = indexes.begin();
    for (; iter != indexes.end(); iter++) {
        if (iter->tableName == tableName && iter->column == column) {
            return *iter;
        }
    }
    return Index();
}

Index CatalogManager::getIndexInfor(string indexName) {
    vector<Index>::iterator iter = indexes.begin();
    for (; iter != indexes.end(); iter++) {
        if (iter->indexName == indexName) {
            return *iter;
        }
    }
    return Index();
}

void CatalogManager::printTableCatalog() {
    cout << "Number of tables:" << tableNum << endl;
    for (int i = 0; i < tableNum; i++) {
        cout << "TABLE " << i << endl;
        cout << "Table Name: " << tables[i].name << endl;
        cout << "Number of attributes: " << tables[i].attriNum << endl;
        cout << "Number of blocks occupied in disk: " << tables[i].blockNum << endl;
        for (int j = 0; j < tables[i].attriNum; j++) {
            cout << tables[i].attributes[j].name << "\t";
            switch (tables[i].attributes[j].type) {
                case CHAR: cout << "CHAR(" << tables[i].attributes[j].length << ")\t"; break;
                case INT: cout << "INT\t"; break;
                case FLOAT: cout << "FLOAT\t"; break;
                default: cout << "UNKNOW TYPE\t"; break;
            }
            if (tables[i].attributes[j].isUnique)
                cout << "Unique\t";
            else
                cout << "Not Unique ";
            if (tables[i].attributes[j].isPrimaryKey)
                cout << "Primary Key\t" << endl;
        }
    }
}

void CatalogManager::printIndexCatalog() {
    cout << "Number of indexes:" << indexNum << endl;
    for (int i = 0; i < indexNum; i++) {
        cout << "INDEX " << i << endl;
        cout << "Index Name: " << indexes[i].indexName << endl;
        cout << "Table Name: " << indexes[i].tableName << endl;
        cout << "Column Number: " << indexes[i].column << endl;
        cout << "Column Number of blocks occupied in disk: " << indexes[i].blockNum << endl;
    }
}
/*
return -1 for no such column
*/
int getColumnID(Table &tableInfor, string columnName) {
    vector<Attribute>::iterator iter = tableInfor.attributes.begin();
    for (; iter != tableInfor.attributes.end(); iter++) {
        if (iter->name == columnName) {
            return distance(tableInfor.attributes.begin(), iter);
        }
    }
    return -1;
}

int getColumnAmount(const Table &tableInfor) {
    return tableInfor.attriNum;
}
