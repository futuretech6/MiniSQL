#include "Interpreter.h"

// enum Operation{CREATABLE, INSERT, SELECT, DELECT, DROPINDEX, CRETEINDEX, DROPTABLE, ERROR};
// Interpreter interprt;
RecordManager record;
IndexManager indexm;
CatalogManager catalog;
Interpreter parsetree;
BufferManager buf;

void welcome();
void ShowResult(Data data, Table tableInfo, vector<Attribute> column);
void Execute();
void AddSeperator(char *command);
short int IsComEnd(char *input);

int main() {
    welcome();
    vector<Condition> conditions;
    Table tableInfo;
    Index indexInfo;
    Row insertValue;
    Data datas;
    char command[COMLEN] = "";
    char input[INPUTLEN] = "";
    char word[WORDLEN]   = "";
    short int ComEnd     = 0;
    /*FILE *stream;
    stream=freopen("instruction10.txt","r",stdin);*/
    /*FILE *fileout;
    fileout=freopen("result.txt","w",stdout);*/

    // Int count = 0;
    while (1) {
        strcpy(command, "");  // Command����
        ComEnd = 0;
        cout << " MiniSQL> ";
        while (!ComEnd) {
            /*		count ++;
                    if(count % 100 == 0)
                        cout << count/100 << "%" << endl;*/
            cin.getline(input, 1000);  // Gets(input);
            if (IsComEnd(input))
                ComEnd = 1;
            strcat(command, input);
            AddSeperator(command);
        }
        parsetree.Parse(command);
        Execute();
    }
    getchar();
    return 0;
}

void welcome() {
    cout << "____Welcome to our micro database system____" << endl;
}

void ShowResult(Data data, Table tableInfo, vector<Attribute> column) {
    if (column[0].name == "*") {
        cout << endl << "+";
        for (int i = 0; i < tableInfo.attriNum; i++) {
            for (int j = 0; j < tableInfo.attributes[i].length + 1; j++) {
                cout << "-";
            }
            cout << "+";
        }
        cout << endl;
        cout << "| ";
        for (int i = 0; i < tableInfo.attriNum; i++) {
            cout << tableInfo.attributes[i].name;
            int lengthLeft =
                tableInfo.attributes[i].length - tableInfo.attributes[i].name.length();
            for (int j = 0; j < lengthLeft; j++) {
                cout << ' ';
            }
            cout << "| ";
        }
        cout << endl;
        cout << "+";
        for (int i = 0; i < tableInfo.attriNum; i++) {
            for (int j = 0; j < tableInfo.attributes[i].length + 1; j++) {
                cout << "-";
            }
            cout << "+";
        }
        cout << endl;

        //����
        for (int i = 0; i < data.rowList.size(); i++) {
            cout << "| ";
            for (int j = 0; j < tableInfo.attriNum; j++) {
                int lengthLeft = tableInfo.attributes[j].length;
                for (int k = 0; k < data.rowList[i].columnList[j].length(); k++) {
                    if (data.rowList[i].columnList[j].c_str()[k] == EMPTY)
                        break;
                    else {
                        cout << data.rowList[i].columnList[j].c_str()[k];
                        lengthLeft--;
                    }
                }
                for (int k = 0; k < lengthLeft; k++)
                    cout << " ";
                cout << "| ";
            }
            cout << endl;
        }

        cout << "+";
        for (int i = 0; i < tableInfo.attriNum; i++) {
            for (int j = 0; j < tableInfo.attributes[i].length + 1; j++) {
                cout << "-";
            }
            cout << "+";
        }
        cout << endl;
    } else {
        cout << endl << "+";
        for (int i = 0; i < column.size(); i++) {
            int col;
            for (col = 0; col < tableInfo.attributes.size(); col++) {
                if (tableInfo.attributes[col].name == column[i].name)
                    break;
            }
            for (int j = 0; j < tableInfo.attributes[col].length + 1; j++) {
                cout << "-";
            }
            cout << "+";
        }
        cout << endl;
        cout << "| ";
        for (int i = 0; i < column.size(); i++) {
            int col;
            for (col = 0; col < tableInfo.attributes.size(); col++) {
                if (tableInfo.attributes[col].name == column[i].name)
                    break;
            }
            cout << tableInfo.attributes[col].name;
            int lengthLeft =
                tableInfo.attributes[col].length - tableInfo.attributes[col].name.length();
            for (int j = 0; j < lengthLeft; j++) {
                cout << ' ';
            }
            cout << "| ";
        }
        cout << endl;
        cout << "+";
        for (int i = 0; i < column.size(); i++) {
            int col;
            for (col = 0; col < tableInfo.attributes.size(); col++) {
                if (tableInfo.attributes[col].name == column[i].name)
                    break;
            }
            for (int j = 0; j < tableInfo.attributes[col].length + 1; j++) {
                cout << "-";
            }
            cout << "+";
        }
        cout << endl;

        //����
        for (int i = 0; i < data.rowList.size(); i++) {
            cout << "| ";
            for (int j = 0; j < column.size(); j++) {
                int col;
                for (col = 0; col < tableInfo.attributes.size(); col++) {
                    if (tableInfo.attributes[col].name == column[j].name)
                        break;
                }
                int lengthLeft = tableInfo.attributes[col].length;
                for (int k = 0; k < data.rowList[i].columnList[col].length(); k++) {
                    if (data.rowList[i].columnList[col].c_str()[k] == EMPTY)
                        break;
                    else {
                        cout << data.rowList[i].columnList[col].c_str()[k];
                        lengthLeft--;
                    }
                }
                for (int k = 0; k < lengthLeft; k++)
                    cout << " ";
                cout << "| ";
            }
            cout << endl;
        }

        cout << "+";
        for (int i = 0; i < column.size(); i++) {
            int col;
            for (col = 0; col < tableInfo.attributes.size(); col++) {
                if (tableInfo.attributes[col].name == column[i].name)
                    break;
            }
            for (int j = 0; j < tableInfo.attributes[col].length + 1; j++) {
                cout << "-";
            }
            cout << "+";
        }
        cout << endl;
    }
    cout << data.rowList.size() << " rowList have been found." << endl;
}
void Execute() {
    int i;
    int j;
    int k;
    Table tableInfo;
    Index indexInfo;
    string tempKeyValue;
    int tempPrimaryPosition = -1;
    int rowCount            = 0;
    Data data;
    switch (parsetree.m_operation) {
        case CRETAB:
            parsetree.getTableInfo.attriNum = parsetree.getTableInfo.attributes.size();
            catalog.createTable(parsetree.getTableInfo);
            record.createTable(parsetree.getTableInfo);
            cout << "Table " << parsetree.getTableInfo.name << " has been created successfully"
                 << endl;
            break;
        case TABLEEXISTED:
            cout << "The table has been created,please check the database" << endl;
            break;
        case DRPTAB:
            record.dropTable(parsetree.getTableInfo);
            for (int i = 0; i < parsetree.getTableInfo.attriNum; i++) {  //���������е�index��ɾ��
                indexInfo = catalog.getIndexInformation(parsetree.getTableInfo.name, i);
                if (indexInfo.indexName != "")
                    indexm.dropIndex(indexInfo);
            }
            catalog.dropTable(parsetree.getTableInfo);
            cout << "Table " << parsetree.getTableInfo.name << " has been dropped successfully"
                 << endl;
            break;
        case INSERT:
            tableInfo = parsetree.getTableInfo;
            if (parsetree.PrimaryKeyPosition == -1 && parsetree.UniquePostion == -1) {
                record.insertValue(tableInfo, parsetree.row);
                catalog.update(tableInfo);
                cout << "One record has been inserted successfully" << endl;
                break;
            }
            if (parsetree.PrimaryKeyPosition != -1) {
                data = record.select(tableInfo, parsetree.condition);
                if (data.rowList.size() > 0) {
                    cout << "Primary Key Redundancy occurs, thus insertion failed" << endl;
                    break;
                }
            }
            if (parsetree.UniquePostion != -1) {

                data = record.select(tableInfo, parsetree.UniqueCondition);
                if (data.rowList.size() > 0) {
                    cout << "Unique Value Redundancy occurs, thus insertion failed" << endl;
                    break;
                }
            }
            record.insertValue(tableInfo, parsetree.row);
            catalog.update(tableInfo);
            cout << "One record has been inserted successfully" << endl;
            break;
        case INSERTERR:
            cout << "Incorrect usage of \"insert\" query! Please check your input!" << endl;
            break;
        case SELECT_NOWHERE_CAULSE:
            tableInfo = parsetree.getTableInfo;
            data       = record.select(tableInfo);
            if (data.rowList.size() != 0)
                ShowResult(data, tableInfo, parsetree.column);
            else {
                cout << "No data is found!!!" << endl;
            }
            break;
        case SELECT_WHERE_CAULSE:
            tableInfo = parsetree.getTableInfo;
            if (parsetree.condition.size() == 1) {
                for (int i = 0; i < parsetree.getTableInfo.attributes.size(); i++) {
                    /*�޸�*/ if ((parsetree.getTableInfo.attributes[i].isPrimeryKey == true ||
                                         parsetree.getTableInfo.attributes[i].isUnique == true) &&
                                     parsetree.m_colname ==
                                         parsetree.getTableInfo.attributes[i].name) {
                        tempPrimaryPosition = i;
                        indexInfo          = catalog.getIndexInformation(tableInfo.name, i);
                        break;
                    }
                }
                if (tempPrimaryPosition == parsetree.condition[0].columnNum &&
                    parsetree.condition[0].op == Eq && indexInfo.table_name != "") {

                    tempKeyValue = parsetree.condition[0].value;
                    data         = indexm.selectEqual(tableInfo, indexInfo, tempKeyValue);
                } else {

                    data = record.select(tableInfo, parsetree.condition);
                }
            } else {
                data = record.select(tableInfo, parsetree.condition);
            }
            if (data.rowList.size() != 0)
                ShowResult(data, tableInfo, parsetree.column);
            else {
                cout << "No data is found!!!" << endl;
            }
            break;
        case DELETE:
            rowCount = record.deleteValue(parsetree.getTableInfo, parsetree.condition);
            cout << rowCount << "  rowList have been deleted." << endl;
            break;
        case CREIND:
            tableInfo = parsetree.getTableInfo;
            indexInfo = parsetree.getIndexInfo;
            if (!tableInfo.attributes[indexInfo.column].isPrimeryKey &&
                !tableInfo.attributes[indexInfo.column]
                     .isUnique) {  //����primary key�������Խ�index
                cout << "Column " << tableInfo.attributes[indexInfo.column].name
                     << "  is not unique." << endl;
                break;
            }
            catalog.createIndex(indexInfo);
            indexm.createIndex(tableInfo, indexInfo);
            catalog.update(indexInfo);
            cout << "The index " << indexInfo.indexName << "has been created successfully" << endl;
            break;
        case INDEXERROR:
            cout << "The index on primary key of table has been existed" << endl;
            break;
        case DRPIND:
            indexInfo = catalog.getIndexInformation(parsetree.m_indname);
            if (indexInfo.indexName == "") {
                cout << "Index" << parsetree.m_indname << "does not exist!" << endl;
            }
            indexm.dropIndex(indexInfo);
            catalog.dropIndex(parsetree.m_indname);
            cout << "The index has been dropped successfully" << endl;
            break;
        case CREINDERR:
            cout << "Incorrect usage of \"create index\" query! Please check your input!" << endl;
            break;
        case QUIT:
            cout << "Have a good day! Press any key to close this window." << endl;
            getchar();
            exit(0);
            break;
        case EMPTY: cout << "Empty query! Please enter your command!" << endl; break;
        case UNKNOW: cout << "UNKNOW query! Please check your input!" << endl; break;
        case SELERR:
            cout << "Incorrect usage of \"select\" query! Please check your input!" << endl;
            break;
        case CRETABERR:
            cout << "Incorrect usage of \"create table\" query! Please check your input!" << endl;
            break;
        case DELETEERR:
            cout << "Incorrect usage of \"delete from\" query! Please check your input!" << endl;
            break;
        case DRPTABERR:
            cout << "Incorrect usage of \"drop table\" query! Please check your input!" << endl;
            break;
        case DRPINDERR:
            cout << "Incorrect usage of \"drop index\" query! Please check your input!" << endl;
            break;
        case VOIDPRI: cout << "Error: invalid primary key! Please check your input!" << endl; break;
        case VOIDUNI: cout << "Error: invalid unique key! Please check your input!" << endl; break;
        case CHARBOUD:
            cout << "Error: only 1~255 charactors is allowed! Please check your input!" << endl;
            break;
        case NOPRIKEY: cout << "No primary key is defined! Please check your input!" << endl; break;
        case TABLEERROR: cout << "Table is not existed,please check the database" << endl; break;
        case INDEXEROR: cout << "Index is not existed,please check the database" << endl; break;
        case COLUMNERROR: cout << "One column is not existed" << endl; break;
        case INSERTNUMBERERROR:
            cout << "The column number is not according to the columnList in our database" << endl;
            break;
    }
}
void AddSeperator(char *command) {
    unsigned len     = strlen(command);
    command[len]     = ' ';
    command[len + 1] = '\0';
}

short int IsComEnd(char *input) {
    unsigned int next = strlen(input) - 1;
    char prev         = ' ';
    while (prev == '\t' || prev == ' ') {
        prev = input[next];
        next--;
    }
    if (prev == ';') {
        input[next + 1] = '\0';
        return 1;
    }
    return 0;
}
