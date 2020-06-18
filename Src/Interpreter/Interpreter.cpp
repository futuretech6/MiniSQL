#include "Interpreter.h"

Interpreter::Interpreter() {
    tableName     = "";
    indexName     = "";
    columnName    = "";
    fileName      = "";
    op            = UNKNOWN;
    primaryKeyPos = -1;
    uniquePos     = -1;
}

void Interpreter::initInterpreter() {
    tableName     = "";
    indexName     = "";
    columnName    = "";
    fileName      = "";
    op            = UNKNOWN;
    primaryKeyPos = -1;
    uniquePos     = -1;
    if (attributes.size() > 0)
        attributes.clear();
    if (recordData.columnList.size() > 0)
        recordData.columnList.clear();
    condition.initCond();
    tableInfor.initTable();
    indexInfor.initIndex();
}

int getFirstWord(string &command, string &firstWord) {
    if (command.length() == 0)
        return 0;
    firstWord.clear();
    unsigned int commandPos = 0, firstWordPos = 0;
    char pivot = command.at(commandPos);
    while (pivot == ' ' || pivot == '\t' || pivot == '\n') {
        commandPos++;
        if (commandPos > command.length() - 1)
            return 0;
        pivot = command.at(commandPos);
    }
    switch (pivot) {
        case ',':
        case '(':
        case ')':
        case '*':
        case '=':
        case '\'':
        case ';':
            firstWord += pivot;
            command.erase(0, commandPos + 1);
            break;
        case '<':
            firstWord += pivot;
            pivot = command.at(++commandPos);
            if (pivot == '=' || pivot == '>') {
                firstWord += pivot;
                command.erase(0, commandPos + 1);
            } else {
                command.erase(0, commandPos);
            }
            break;
        case '>':
            firstWord += pivot;
            pivot = command.at(++commandPos);
            if (pivot == '=') {
                firstWord += pivot;
                command.erase(0, commandPos + 1);
            } else {
                command.erase(0, commandPos);
            }
            break;
        default:
            firstWord += pivot;
            pivot = command[++commandPos];
            while (pivot != ',' && pivot != '(' && pivot != ')' && pivot != '*' && pivot != '=' && pivot != '\'' &&
                   pivot != '<' && pivot != '>' && pivot != '\t' && pivot != '\n' && pivot != ';' && pivot != ' ') {
                firstWord += pivot;
                firstWordPos++;
                pivot = command[++commandPos];
            }
            command.erase(0, commandPos);
    }
    return 1;
}

int getString(string &command, string &firstString) {
    firstString.clear();
    unsigned int pos = 0;
    while (command[pos] != '\'') {
        firstString += command[pos];
        pos++;
        if (pos > command.length() - 1) {
            return 0;
        }
    }
    command.erase(0, pos);
    return 1;
}

// int isBlank(string &command)
//{
//	unsigned int i;
//	int flag = 1;
//	for (i = 0; i < command.length(); i++)
//	{
//		if (command[i] != '\n' && command[i] != ' ' && command[i] != '\t')
//		{
//			flag = 0;
//			break;
//		}
//	}
//	return flag;
//}

void Interpreter::parseCommand(string &command) {
    initInterpreter();
    string firstWord;
    string tmpCommand = command;
    // if (isBlank(command))
    //{
    //	op = EMPTY_COMMAND;
    //	return;
    //}
    int flag = getFirstWord(tmpCommand, firstWord);
    op       = EMPTY_COMMAND;
    if (!flag) {
        op = EMPTY_COMMAND;
        return;
    } else if ((firstWord == ";")) {
        op = EMPTY_COMMAND;
        return;
    } else if ((firstWord == "quit") || (firstWord == "QUIT")) {
        op = QUIT;
        return;
    }

    else if ((firstWord == "select") || (firstWord == "SELECT")) {
        op = SELECT_ERROR;
        //���û�ܵõ���һ�����ʻ�����һ�������Ƿֺţ�op���ش���
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;

        Attribute tmpAttr;
        tmpAttr.name = firstWord;
        attributes.push_back(tmpAttr);

        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //��select�������
        while ((firstWord == ",")) {
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            tmpAttr.name = firstWord;
            attributes.push_back(tmpAttr);
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
        }
        //�����һ������word����from��op���ش���
        if ((firstWord != "from") && (firstWord != "FROM"))
            return;
        //���catalog�б��Ƿ����
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        tableName = firstWord;
        if (!catalog.existTable(firstWord))  //!!!!!!!
        {
            op = TABLE_ERROR;
            return;
        }
        tableInfor = catalog.getTableInfor(tableName);  //!!!!
        //������������û��where�Ӿ䣬���ض�Ӧop
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";")) {
            op = SELECT_NOWHERE_CLAUSE;
            return;
        }
        //�����û�н������������word����where
        if ((firstWord != "where") && (firstWord != "WHERE"))
            return;
        //����where��
        //�����һ������
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        condition.column1 = getColumnID(tableInfor, firstWord);  //��¼��һ������ֵ
        if (condition.column1 == -1)  //����������ڱ��в�����
        {
            columnName = firstWord;
            op         = COLUMN_ERROR;
            return;
        }
        //����ȽϷ���
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //�ж��ȽϷ���
        if ((firstWord == "<"))
            condition.op1 = Lt;
        else if ((firstWord == "<="))
            condition.op1 = Le;
        else if ((firstWord == ">"))
            condition.op1 = Gt;
        else if ((firstWord == ">="))
            condition.op1 = Ge;
        else if ((firstWord == "="))
            condition.op1 = Eq;
        else if ((firstWord == "<>"))
            condition.op1 = Ne;
        else
            return;
        //������Ƚ�ֵ
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //�����''��ס���ַ���
        if ((firstWord == "\'")) {
            //����''֮����ַ�
            if (!getString(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            condition.value1 = firstWord;
            //�����һ��'
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            //�������Ĳ���'
            if ((firstWord != "\'"))
                return;
        }
        //�������''��ס���ַ���
        else {
            condition.value1 = firstWord;
        }
        //�ж��ڶ�����������û�еڶ�������������condition�󷵻�
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";")) {
            op                 = SELECT_WHERE_CLAUSE;
            condition.relation = None;
            return;
        }
        //����еڶ���������ӦΪand��or
        if ((firstWord == "and") || (firstWord == "AND"))
            condition.relation = And;
        else if ((firstWord == "or") || (firstWord == "OR"))
            condition.relation = Or;
        else
            return;
        //����ڶ�������
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        condition.column2 = getColumnID(tableInfor, firstWord);  //��¼��һ������ֵ
        if (condition.column2 == -1)  //����������ڱ��в�����
        {
            columnName = firstWord;
            op         = COLUMN_ERROR;
            return;
        }
        //����ȽϷ���
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //�ж��ȽϷ���
        if ((firstWord == "<"))
            condition.op2 = Lt;
        else if ((firstWord == "<="))
            condition.op2 = Le;
        else if ((firstWord == ">"))
            condition.op2 = Gt;
        else if ((firstWord == ">="))
            condition.op2 = Ge;
        else if ((firstWord == "="))
            condition.op2 = Eq;
        else if ((firstWord == "<>"))
            condition.op2 = Ne;
        else
            return;
        //������Ƚ�ֵ
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //�����''��ס���ַ���
        if ((firstWord == "\'")) {
            //����''֮����ַ�
            if (!getString(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            condition.value2 = firstWord;
            //�����һ��'
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            //�������Ĳ���'
            if ((firstWord != "\'"))
                return;
        }
        //�������''��ס���ַ���
        else {
            condition.value2 = firstWord;
        }
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";")) {
            op = SELECT_WHERE_CLAUSE;
            return;
        }
    }

    else if ((firstWord == "create") || (firstWord == "CREATE")) {
        op = CREATE_ERROR;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //�����create table
        if ((firstWord == "table") || (firstWord == "TABLE")) {
            op = CREATE_TABLE_ERROR;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            tableName = firstWord;
            //�����Ѿ�����
            if (catalog.existTable(tableName)) {
                op = TABLE_EXISTED;
                return;
            }
            tableInfor.name = firstWord;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            //������Ĳ���(�����ش���
            if ((firstWord != "("))
                return;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            if ((firstWord == "unique") || (firstWord == "primary") || (firstWord == "UNIQUE") ||
                (firstWord == "UNIQUE")) {
                return;
            }
            Attribute tmpAttr;
            tmpAttr.name = firstWord;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            //�ж���������
            if ((firstWord == "int") || (firstWord == "INT")) {
                tmpAttr.type   = INT;
                tmpAttr.length = INTLEN;
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if ((firstWord == ";"))
                    return;
                if ((firstWord == "unique") || (firstWord == "UNIQUE")) {
                    tmpAttr.isUnique = 1;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if ((firstWord == ";"))
                        return;
                }
                //ע���ʱ�����һ������
            } else if ((firstWord == "float") || (firstWord == "FLOAT")) {
                tmpAttr.type   = FLOAT;
                tmpAttr.length = FLOATLEN;
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if ((firstWord == ";"))
                    return;
                if ((firstWord == "unique") || (firstWord == "UNIQUE")) {
                    tmpAttr.isUnique = 1;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if ((firstWord == ";"))
                        return;
                }
            } else if ((firstWord == "char") || (firstWord == "CHAR")) {
                tmpAttr.type = CHAR;
                //����������
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if ((firstWord == ";"))
                    return;
                if ((firstWord != "("))
                    return;
                //����char����
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if ((firstWord == ";"))
                    return;
                tmpAttr.length = atoi(firstWord.c_str()) + 1;
                if (tmpAttr.length > 255 || tmpAttr.length < 1) {
                    op = CHAR_BOUND;
                    return;
                }
                //����������
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if ((firstWord == ";"))
                    return;
                if ((firstWord != ")"))
                    return;

                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if ((firstWord == ";"))
                    return;
                if ((firstWord == "unique") || (firstWord == "UNIQUE")) {
                    tmpAttr.isUnique = 1;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if ((firstWord == ";"))
                        return;
                }
            } else {
                columnName = firstWord;  //����columnName��Ŵ������������
                op         = DATA_TYPE_ERROR;
                return;
            }
            tableInfor.attributes.push_back(tmpAttr);
            tmpAttr.isPrimaryKey = tmpAttr.isUnique = 0;
            //���ڶ�����Ի����primary key����
            while ((firstWord == ",")) {
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if ((firstWord == ";"))
                    return;
                //����primary key����
                if ((firstWord == "primary") || (firstWord == "PRIMARY")) {
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if ((firstWord == ";"))
                        return;
                    //������Ĳ���key
                    if ((firstWord != "key") && (firstWord != "KEY"))
                        return;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if ((firstWord == ";"))
                        return;
                    //������Ĳ���(
                    if ((firstWord != "("))
                        return;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if ((firstWord == ";"))
                        return;
                    int primaryKeyInTable = 0;
                    for (unsigned int i = 0; i < tableInfor.attributes.size(); i++) {
                        if (!strcmp(tableInfor.attributes[i].name.c_str(), firstWord.c_str())) {
                            primaryKeyInTable                     = 1;
                            tableInfor.attributes[i].isPrimaryKey = 1;
                            tableInfor.attributes[i].isUnique     = 1;
                        }
                    }
                    if (!primaryKeyInTable) {
                        op = INVALID_PRIMARY_KEY;
                        return;
                    }
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if ((firstWord == ";"))
                        return;
                    //��δ����������
                    if ((firstWord != ")"))
                        return;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if ((firstWord == ";"))
                        return;
                } else {
                    tmpAttr.name = firstWord;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if ((firstWord == ";"))
                        return;
                    //�ж���������
                    if ((firstWord == "int") || (firstWord == "INT")) {
                        tmpAttr.type   = INT;
                        tmpAttr.length = INTLEN;
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if ((firstWord == ";"))
                            return;
                        if ((firstWord == "unique") || (firstWord == "UNIQUE")) {
                            tmpAttr.isUnique = 1;
                            if (!getFirstWord(tmpCommand, firstWord))
                                return;
                            if ((firstWord == ";"))
                                return;
                        }
                        //ע���ʱ�����һ������
                    } else if ((firstWord == "float") || (firstWord == "FLOAT")) {
                        tmpAttr.type   = FLOAT;
                        tmpAttr.length = FLOATLEN;
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if ((firstWord == ";"))
                            return;
                        if ((firstWord == "unique") || (firstWord == "UNIQUE")) {
                            tmpAttr.isUnique = 1;
                            if (!getFirstWord(tmpCommand, firstWord))
                                return;
                            if ((firstWord == ";"))
                                return;
                        }
                    } else if ((firstWord == "char") || (firstWord == "CHAR")) {
                        tmpAttr.type = CHAR;
                        //����������
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if ((firstWord == ";"))
                            return;
                        if ((firstWord != "("))
                            return;
                        //����char����
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if ((firstWord == ";"))
                            return;
                        tmpAttr.length = atoi(firstWord.c_str()) + 1;
                        if (tmpAttr.length > 255 || tmpAttr.length < 1) {
                            op = CHAR_BOUND;
                            return;
                        }
                        //����������
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if ((firstWord == ";"))
                            return;
                        if ((firstWord != ")"))
                            return;

                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if ((firstWord == ";"))
                            return;
                        if ((firstWord == "unique") || (firstWord == "UNIQUE")) {
                            tmpAttr.isUnique = 1;
                            if (!getFirstWord(tmpCommand, firstWord))
                                return;
                            if ((firstWord == ";"))
                                return;
                        }
                    } else {
                        op = DATA_TYPE_ERROR;
                        return;
                    }
                    tableInfor.attributes.push_back(tmpAttr);
                    tmpAttr.isPrimaryKey = tmpAttr.isUnique = 0;
                }
            }
            if ((firstWord != ")"))
                return;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";")) {
                tableInfor.attriNum    = tableInfor.attributes.size();
                tableInfor.totalLength = 0;
                for (unsigned int i = 0; i < tableInfor.attributes.size(); i++) {
                    tableInfor.totalLength += tableInfor.attributes[i].length;
                }
                op = CREATE_TABLE;
                return;
            }
        } else if ((firstWord == "index") || (firstWord == "INDEX")) {
            op = CREATE_INDEX_ERROR;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            indexName            = firstWord;
            indexInfor.indexName = firstWord;
            if (catalog.existIndex(indexName)) {
                op = INDEX_EXISTED;
                return;
            }
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            if ((firstWord != "on") && (firstWord != "ON"))
                return;

            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            tableName = firstWord;
            if (!catalog.existTable(firstWord)) {
                op = TABLE_ERROR;
                return;
            }
            tableInfor           = catalog.getTableInfor(tableName);
            indexInfor.tableName = firstWord;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            if ((firstWord != "("))
                return;

            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            int tmpColumID = getColumnID(tableInfor, firstWord);
            if (tmpColumID == -1) {
                columnName = firstWord;
                op         = COLUMN_ERROR;
                return;
            }
            if (catalog.existIndex(tableName, tmpColumID)) {
                op = INDEX_EXISTED;
                return;
            }
            indexInfor.column       = tmpColumID;
            indexInfor.blockNum     = 0;
            indexInfor.columnLength = tableInfor.attributes[tmpColumID].length;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            if ((firstWord != ")"))
                return;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";")) {
                op = CREATE_INDEX;
                return;
            }
        } else
            return;
    }

    else if ((firstWord == "delete") || (firstWord == "DELETE")) {
        op = DELETE_ERROR;  // �﷨����
        getFirstWord(tmpCommand, firstWord);
        if ((firstWord != "from") && (firstWord != "FROM"))
            return;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //���catalog�б��Ƿ����
        tableName = firstWord;
        if (!catalog.existTable(firstWord))  //!!!!!!!
        {
            op = TABLE_ERROR;
            return;
        }
        tableInfor = catalog.getTableInfor(tableName);  //!!!!
        //������������û��where�Ӿ䣬���ض�Ӧop
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";")) {
            op = DELETE_NOWHERE_CLAUSE;
            return;
        }
        //�����û�н������������word����where
        if ((firstWord != "where") && (firstWord != "WHERE"))
            return;
        //����where��
        //�����һ������
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        condition.column1 = getColumnID(tableInfor, firstWord);  //��¼��һ������ֵ
        if (condition.column1 == -1)  //����������ڱ��в�����
        {
            columnName = firstWord;
            op         = COLUMN_ERROR;
            return;
        }
        //����ȽϷ���
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //�ж��ȽϷ���
        if ((firstWord == "<"))
            condition.op1 = Lt;
        else if ((firstWord == "<="))
            condition.op1 = Le;
        else if ((firstWord == ">"))
            condition.op1 = Gt;
        else if ((firstWord == ">="))
            condition.op1 = Ge;
        else if ((firstWord == "="))
            condition.op1 = Eq;
        else if ((firstWord == "<>"))
            condition.op1 = Ne;
        else
            return;
        //������Ƚ�ֵ
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //�����''��ס���ַ���
        if ((firstWord == "\'")) {
            //����''֮����ַ�
            if (!getString(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            condition.value1 = firstWord;
            //�����һ��'
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            //�������Ĳ���'
            if ((firstWord != "\'"))
                return;
        }
        //�������''��ס���ַ���
        else {
            condition.value1 = firstWord;
        }
        //�ж��ڶ�����������û�еڶ�������������condition�󷵻�
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";")) {
            op                 = DELETE_WHERE_CLAUSE;
            condition.relation = None;
            return;
        }
        //����еڶ���������ӦΪand��or
        if ((firstWord == "and") || (firstWord == "AND"))
            condition.relation = And;
        else if ((firstWord == "or") || (firstWord == "OR"))
            condition.relation = Or;
        else
            return;
        //����ڶ�������
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        condition.column2 = getColumnID(tableInfor, firstWord);  //��¼��һ������ֵ
        if (condition.column2 == -1)  //����������ڱ��в�����
        {
            columnName = firstWord;
            op         = COLUMN_ERROR;
            return;
        }
        //����ȽϷ���
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //�ж��ȽϷ���
        if ((firstWord == "<"))
            condition.op2 = Lt;
        else if ((firstWord == "<="))
            condition.op2 = Le;
        else if ((firstWord == ">"))
            condition.op2 = Gt;
        else if ((firstWord == ">="))
            condition.op2 = Ge;
        else if ((firstWord == "="))
            condition.op2 = Eq;
        else if ((firstWord == "<>"))
            condition.op2 = Ne;
        else
            return;
        //������Ƚ�ֵ
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        //�����''��ס���ַ���
        if ((firstWord == "\'")) {
            //����''֮����ַ�
            if (!getString(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            condition.value2 = firstWord;
            //�����һ��'
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            //�������Ĳ���'
            if ((firstWord != "\'"))
                return;
        }
        //�������''��ס���ַ���
        else {
            condition.value2 = firstWord;
        }
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";")) {
            op = DELETE_WHERE_CLAUSE;
            return;
        }
        return;
    }

    else if ((firstWord == "insert") || (firstWord == "INSERT")) {
        op = INSERT_ERROR;
        getFirstWord(tmpCommand, firstWord);  // get "into"
        if ((firstWord != "into") && (firstWord != "INTO"))
            return;
        if (!getFirstWord(tmpCommand, firstWord))  // get tableName
            return;
        if ((firstWord == ";"))
            return;
        // �õ����������Ϣ
        tableName = firstWord;
        if (!catalog.existTable(tableName)) {
            op = TABLE_ERROR;
            return;
        }
        tableInfor = catalog.getTableInfor(tableName);
        if (!getFirstWord(tmpCommand, firstWord))  // get "values"
            return;
        if ((firstWord != "values") && (firstWord != "VALUES"))
            return;
        getFirstWord(tmpCommand, firstWord);
        if ((firstWord != "("))
            return;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        int count = 0;
        while ((firstWord != ")")) {
            if ((firstWord == ",")) {
                getFirstWord(tmpCommand, firstWord);
                continue;
            }
            if ((firstWord == "'")) {
                getString(tmpCommand, firstWord);
                tmpCommand.erase(0, 1);
                continue;
            }
            count++;
            recordData.columnList.push_back(firstWord);
            getFirstWord(tmpCommand, firstWord);
        }
        // cout << count << endl;
        // for (int i = 0; i < recordData.columnList.size(); i++)
        //{
        //    cout << recordData.columnList[i] << endl;
        //}
        if (count != tableInfor.attriNum)  //�����������������ڱ���������
        {
            op = INSERT_NUMBER_ERROR;
            return;
        }
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";")) {
            op = INSERT;
            return;
        }
    }

    else if ((firstWord == "drop") || (firstWord == "DROP")) {
        op = DROP_ERROR;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        if ((firstWord == "table") || (firstWord == "TABLE")) {
            op = DROP_TABLE_ERROR;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            tableName = firstWord;
            if (!catalog.existTable(tableName)) {
                op = TABLE_ERROR;
                return;
            }
            tableInfor = catalog.getTableInfor(tableName);
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";")) {
                op = DROP_TABLE;
                return;
            }
            return;
        } else if ((firstWord == "index") || (firstWord == "INDEX")) {
            op = DROP_INDEX_ERROR;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";"))
                return;
            indexName = firstWord;
            if (!catalog.existIndex(indexName)) {
                op = INDEX_ERROR;
                return;
            }
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if ((firstWord == ";")) {
                op = DROP_INDEX;
                return;
            }
            return;
        }
    }

    else if ((firstWord == "execfile") || (firstWord == "EXECFILE")) {
        op = EXECFILE_ERROR;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";"))
            return;
        fileName = firstWord;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if ((firstWord == ";")) {
            op = EXECFILE;
            return;
        }
        return;
    }
    op = TYPE_ERROR;
}
