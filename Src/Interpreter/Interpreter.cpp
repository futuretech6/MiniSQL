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
    } else if (!strcmp(firstWord.c_str(), ";")) {
        op = EMPTY_COMMAND;
        return;
    } else if (!strcmp(firstWord.c_str(), "quit") || !strcmp(firstWord.c_str(), "QUIT")) {
        op = QUIT;
        return;
    }

    else if (!strcmp(firstWord.c_str(), "select") || !strcmp(firstWord.c_str(), "SELECT")) {
        op = SELECT_ERROR;
        //���û�ܵõ���һ�����ʻ�����һ�������Ƿֺţ�op���ش���
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
            return;

        Attribute tmpAttr;
        tmpAttr.name = firstWord;
        attributes.push_back(tmpAttr);

        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //��select�������
        while (!strcmp(firstWord.c_str(), ",")) {
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            tmpAttr.name = firstWord;
            attributes.push_back(tmpAttr);
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
        }
        //�����һ������word����from��op���ش���
        if (strcmp(firstWord.c_str(), "from") && strcmp(firstWord.c_str(), "FROM"))
            return;
        //���catalog�б��Ƿ����
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
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
        if (!strcmp(firstWord.c_str(), ";")) {
            op = SELECT_NOWHERE_CLAUSE;
            return;
        }
        //�����û�н������������word����where
        if (strcmp(firstWord.c_str(), "where") && strcmp(firstWord.c_str(), "WHERE"))
            return;
        //����where��
        //�����һ������
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
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
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //�ж��ȽϷ���
        if (!strcmp(firstWord.c_str(), "<"))
            condition.op1 = Lt;
        else if (!strcmp(firstWord.c_str(), "<="))
            condition.op1 = Le;
        else if (!strcmp(firstWord.c_str(), ">"))
            condition.op1 = Gt;
        else if (!strcmp(firstWord.c_str(), ">="))
            condition.op1 = Ge;
        else if (!strcmp(firstWord.c_str(), "="))
            condition.op1 = Eq;
        else if (!strcmp(firstWord.c_str(), "<>"))
            condition.op1 = Ne;
        else
            return;
        //������Ƚ�ֵ
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //�����''��ס���ַ���
        if (!strcmp(firstWord.c_str(), "\'")) {
            //����''֮����ַ�
            if (!getString(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            condition.value1 = firstWord;
            //�����һ��'
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            //�������Ĳ���'
            if (strcmp(firstWord.c_str(), "\'"))
                return;
        }
        //�������''��ס���ַ���
        else {
            condition.value1 = firstWord;
        }
        //�ж��ڶ�����������û�еڶ�������������condition�󷵻�
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";")) {
            op                 = SELECT_WHERE_CLAUSE;
            condition.relation = None;
            return;
        }
        //����еڶ���������ӦΪand��or
        if (!strcmp(firstWord.c_str(), "and") || !strcmp(firstWord.c_str(), "AND"))
            condition.relation = And;
        else if (!strcmp(firstWord.c_str(), "or") || !strcmp(firstWord.c_str(), "OR"))
            condition.relation = Or;
        else
            return;
        //����ڶ�������
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
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
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //�ж��ȽϷ���
        if (!strcmp(firstWord.c_str(), "<"))
            condition.op2 = Lt;
        else if (!strcmp(firstWord.c_str(), "<="))
            condition.op2 = Le;
        else if (!strcmp(firstWord.c_str(), ">"))
            condition.op2 = Gt;
        else if (!strcmp(firstWord.c_str(), ">="))
            condition.op2 = Ge;
        else if (!strcmp(firstWord.c_str(), "="))
            condition.op2 = Eq;
        else if (!strcmp(firstWord.c_str(), "<>"))
            condition.op2 = Ne;
        else
            return;
        //������Ƚ�ֵ
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //�����''��ס���ַ���
        if (!strcmp(firstWord.c_str(), "\'")) {
            //����''֮����ַ�
            if (!getString(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            condition.value2 = firstWord;
            //�����һ��'
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            //�������Ĳ���'
            if (strcmp(firstWord.c_str(), "\'"))
                return;
        }
        //�������''��ס���ַ���
        else {
            condition.value2 = firstWord;
        }
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";")) {
            op = SELECT_WHERE_CLAUSE;
            return;
        }
    }

    else if (!strcmp(firstWord.c_str(), "create") || !strcmp(firstWord.c_str(), "CREATE")) {
        op = CREATE_ERROR;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //�����create table
        if (!strcmp(firstWord.c_str(), "table") || !strcmp(firstWord.c_str(), "TABLE")) {
            op = CREATE_TABLE_ERROR;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
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
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            //������Ĳ���(�����ش���
            if (strcmp(firstWord.c_str(), "("))
                return;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            if (!strcmp(firstWord.c_str(), "unique") || !strcmp(firstWord.c_str(), "primary") ||
                !strcmp(firstWord.c_str(), "UNIQUE") || !strcmp(firstWord.c_str(), "UNIQUE")) {
                return;
            }
            Attribute tmpAttr;
            tmpAttr.name = firstWord;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            //�ж���������
            if (!strcmp(firstWord.c_str(), "int") || !strcmp(firstWord.c_str(), "INT")) {
                tmpAttr.type   = INT;
                tmpAttr.length = INTLEN;
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if (!strcmp(firstWord.c_str(), ";"))
                    return;
                if (!strcmp(firstWord.c_str(), "unique") || !strcmp(firstWord.c_str(), "UNIQUE")) {
                    tmpAttr.isUnique = 1;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if (!strcmp(firstWord.c_str(), ";"))
                        return;
                }
                //ע���ʱ�����һ������
            } else if (!strcmp(firstWord.c_str(), "float") || !strcmp(firstWord.c_str(), "FLOAT")) {
                tmpAttr.type   = FLOAT;
                tmpAttr.length = FLOATLEN;
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if (!strcmp(firstWord.c_str(), ";"))
                    return;
                if (!strcmp(firstWord.c_str(), "unique") || !strcmp(firstWord.c_str(), "UNIQUE")) {
                    tmpAttr.isUnique = 1;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if (!strcmp(firstWord.c_str(), ";"))
                        return;
                }
            } else if (!strcmp(firstWord.c_str(), "char") || !strcmp(firstWord.c_str(), "CHAR")) {
                tmpAttr.type = CHAR;
                //����������
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if (!strcmp(firstWord.c_str(), ";"))
                    return;
                if (strcmp(firstWord.c_str(), "("))
                    return;
                //����char����
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if (!strcmp(firstWord.c_str(), ";"))
                    return;
                tmpAttr.length = atoi(firstWord.c_str()) + 1;
                if (tmpAttr.length > 255 || tmpAttr.length < 1) {
                    op = CHAR_BOUND;
                    return;
                }
                //����������
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if (!strcmp(firstWord.c_str(), ";"))
                    return;
                if (strcmp(firstWord.c_str(), ")"))
                    return;

                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if (!strcmp(firstWord.c_str(), ";"))
                    return;
                if (!strcmp(firstWord.c_str(), "unique") || !strcmp(firstWord.c_str(), "UNIQUE")) {
                    tmpAttr.isUnique = 1;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if (!strcmp(firstWord.c_str(), ";"))
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
            while (!strcmp(firstWord.c_str(), ",")) {
                if (!getFirstWord(tmpCommand, firstWord))
                    return;
                if (!strcmp(firstWord.c_str(), ";"))
                    return;
                //����primary key����
                if (!strcmp(firstWord.c_str(), "primary") || !strcmp(firstWord.c_str(), "PRIMARY")) {
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if (!strcmp(firstWord.c_str(), ";"))
                        return;
                    //������Ĳ���key
                    if (strcmp(firstWord.c_str(), "key") && strcmp(firstWord.c_str(), "KEY"))
                        return;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if (!strcmp(firstWord.c_str(), ";"))
                        return;
                    //������Ĳ���(
                    if (strcmp(firstWord.c_str(), "("))
                        return;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if (!strcmp(firstWord.c_str(), ";"))
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
                    if (!strcmp(firstWord.c_str(), ";"))
                        return;
                    //��δ����������
                    if (strcmp(firstWord.c_str(), ")"))
                        return;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if (!strcmp(firstWord.c_str(), ";"))
                        return;
                } else {
                    tmpAttr.name = firstWord;
                    if (!getFirstWord(tmpCommand, firstWord))
                        return;
                    if (!strcmp(firstWord.c_str(), ";"))
                        return;
                    //�ж���������
                    if (!strcmp(firstWord.c_str(), "int") || !strcmp(firstWord.c_str(), "INT")) {
                        tmpAttr.type   = INT;
                        tmpAttr.length = INTLEN;
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if (!strcmp(firstWord.c_str(), ";"))
                            return;
                        if (!strcmp(firstWord.c_str(), "unique") || !strcmp(firstWord.c_str(), "UNIQUE")) {
                            tmpAttr.isUnique = 1;
                            if (!getFirstWord(tmpCommand, firstWord))
                                return;
                            if (!strcmp(firstWord.c_str(), ";"))
                                return;
                        }
                        //ע���ʱ�����һ������
                    } else if (!strcmp(firstWord.c_str(), "float") || !strcmp(firstWord.c_str(), "FLOAT")) {
                        tmpAttr.type   = FLOAT;
                        tmpAttr.length = FLOATLEN;
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if (!strcmp(firstWord.c_str(), ";"))
                            return;
                        if (!strcmp(firstWord.c_str(), "unique") || !strcmp(firstWord.c_str(), "UNIQUE")) {
                            tmpAttr.isUnique = 1;
                            if (!getFirstWord(tmpCommand, firstWord))
                                return;
                            if (!strcmp(firstWord.c_str(), ";"))
                                return;
                        }
                    } else if (!strcmp(firstWord.c_str(), "char") || !strcmp(firstWord.c_str(), "CHAR")) {
                        tmpAttr.type = CHAR;
                        //����������
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if (!strcmp(firstWord.c_str(), ";"))
                            return;
                        if (strcmp(firstWord.c_str(), "("))
                            return;
                        //����char����
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if (!strcmp(firstWord.c_str(), ";"))
                            return;
                        tmpAttr.length = atoi(firstWord.c_str()) + 1;
                        if (tmpAttr.length > 255 || tmpAttr.length < 1) {
                            op = CHAR_BOUND;
                            return;
                        }
                        //����������
                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if (!strcmp(firstWord.c_str(), ";"))
                            return;
                        if (strcmp(firstWord.c_str(), ")"))
                            return;

                        if (!getFirstWord(tmpCommand, firstWord))
                            return;
                        if (!strcmp(firstWord.c_str(), ";"))
                            return;
                        if (!strcmp(firstWord.c_str(), "unique") || !strcmp(firstWord.c_str(), "UNIQUE")) {
                            tmpAttr.isUnique = 1;
                            if (!getFirstWord(tmpCommand, firstWord))
                                return;
                            if (!strcmp(firstWord.c_str(), ";"))
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
            if (strcmp(firstWord.c_str(), ")"))
                return;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";")) {
                tableInfor.attriNum    = tableInfor.attributes.size();
                tableInfor.totalLength = 0;
                for (unsigned int i = 0; i < tableInfor.attributes.size(); i++) {
                    tableInfor.totalLength += tableInfor.attributes[i].length;
                }
                op = CREATE_TABLE;
                return;
            }
        } else if (!strcmp(firstWord.c_str(), "index") || !strcmp(firstWord.c_str(), "INDEX")) {
            op = CREATE_INDEX_ERROR;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            indexName            = firstWord;
            indexInfor.indexName = firstWord;
            if (catalog.existIndex(indexName)) {
                op = INDEX_EXISTED;
                return;
            }
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            if (strcmp(firstWord.c_str(), "on") && strcmp(firstWord.c_str(), "ON"))
                return;

            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
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
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            if (strcmp(firstWord.c_str(), "("))
                return;

            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
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
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            if (strcmp(firstWord.c_str(), ")"))
                return;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";")) {
                op = CREATE_INDEX;
                return;
            }
        } else
            return;
    }

    else if (!strcmp(firstWord.c_str(), "delete") || !strcmp(firstWord.c_str(), "DELETE")) {
        op = DELETE_ERROR;  // �﷨����
        getFirstWord(tmpCommand, firstWord);
        if (strcmp(firstWord.c_str(), "from") && strcmp(firstWord.c_str(), "FROM"))
            return;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
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
        if (!strcmp(firstWord.c_str(), ";")) {
            op = DELETE_NOWHERE_CLAUSE;
            return;
        }
        //�����û�н������������word����where
        if (strcmp(firstWord.c_str(), "where") && strcmp(firstWord.c_str(), "WHERE"))
            return;
        //����where��
        //�����һ������
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
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
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //�ж��ȽϷ���
        if (!strcmp(firstWord.c_str(), "<"))
            condition.op1 = Lt;
        else if (!strcmp(firstWord.c_str(), "<="))
            condition.op1 = Le;
        else if (!strcmp(firstWord.c_str(), ">"))
            condition.op1 = Gt;
        else if (!strcmp(firstWord.c_str(), ">="))
            condition.op1 = Ge;
        else if (!strcmp(firstWord.c_str(), "="))
            condition.op1 = Eq;
        else if (!strcmp(firstWord.c_str(), "<>"))
            condition.op1 = Ne;
        else
            return;
        //������Ƚ�ֵ
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //�����''��ס���ַ���
        if (!strcmp(firstWord.c_str(), "\'")) {
            //����''֮����ַ�
            if (!getString(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            condition.value1 = firstWord;
            //�����һ��'
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            //�������Ĳ���'
            if (strcmp(firstWord.c_str(), "\'"))
                return;
        }
        //�������''��ס���ַ���
        else {
            condition.value1 = firstWord;
        }
        //�ж��ڶ�����������û�еڶ�������������condition�󷵻�
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";")) {
            op                 = DELETE_WHERE_CLAUSE;
            condition.relation = None;
            return;
        }
        //����еڶ���������ӦΪand��or
        if (!strcmp(firstWord.c_str(), "and") || !strcmp(firstWord.c_str(), "AND"))
            condition.relation = And;
        else if (!strcmp(firstWord.c_str(), "or") || !strcmp(firstWord.c_str(), "OR"))
            condition.relation = Or;
        else
            return;
        //����ڶ�������
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
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
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //�ж��ȽϷ���
        if (!strcmp(firstWord.c_str(), "<"))
            condition.op2 = Lt;
        else if (!strcmp(firstWord.c_str(), "<="))
            condition.op2 = Le;
        else if (!strcmp(firstWord.c_str(), ">"))
            condition.op2 = Gt;
        else if (!strcmp(firstWord.c_str(), ">="))
            condition.op2 = Ge;
        else if (!strcmp(firstWord.c_str(), "="))
            condition.op2 = Eq;
        else if (!strcmp(firstWord.c_str(), "<>"))
            condition.op2 = Ne;
        else
            return;
        //������Ƚ�ֵ
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        //�����''��ס���ַ���
        if (!strcmp(firstWord.c_str(), "\'")) {
            //����''֮����ַ�
            if (!getString(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            condition.value2 = firstWord;
            //�����һ��'
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            //�������Ĳ���'
            if (strcmp(firstWord.c_str(), "\'"))
                return;
        }
        //�������''��ס���ַ���
        else {
            condition.value2 = firstWord;
        }
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";")) {
            op = DELETE_WHERE_CLAUSE;
            return;
        }
        return;
    }

    else if (!strcmp(firstWord.c_str(), "insert") || !strcmp(firstWord.c_str(), "INSERT")) {
        op = INSERT_ERROR;
        getFirstWord(tmpCommand, firstWord);  // get "into"
        if (strcmp(firstWord.c_str(), "into") && strcmp(firstWord.c_str(), "INTO"))
            return;
        if (!getFirstWord(tmpCommand, firstWord))  // get tableName
            return;
        if (!strcmp(firstWord.c_str(), ";"))
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
        if (strcmp(firstWord.c_str(), "values") && strcmp(firstWord.c_str(), "VALUES"))
            return;
        getFirstWord(tmpCommand, firstWord);
        if (strcmp(firstWord.c_str(), "("))
            return;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        int count = 0;
        while (strcmp(firstWord.c_str(), ")")) {
            if (!strcmp(firstWord.c_str(), ",")) {
                getFirstWord(tmpCommand, firstWord);
                continue;
            }
            if (!strcmp(firstWord.c_str(), "'")) {
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
        if (!strcmp(firstWord.c_str(), ";")) {
            op = INSERT;
            return;
        }
    }

    else if (!strcmp(firstWord.c_str(), "drop") || !strcmp(firstWord.c_str(), "DROP")) {
        op = DROP_ERROR;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        if (!strcmp(firstWord.c_str(), "table") || !strcmp(firstWord.c_str(), "TABLE")) {
            op = DROP_TABLE_ERROR;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            tableName = firstWord;
            if (!catalog.existTable(tableName)) {
                op = TABLE_ERROR;
                return;
            }
            tableInfor = catalog.getTableInfor(tableName);
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";")) {
                op = DROP_TABLE;
                return;
            }
            return;
        } else if (!strcmp(firstWord.c_str(), "index") || !strcmp(firstWord.c_str(), "INDEX")) {
            op = DROP_INDEX_ERROR;
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";"))
                return;
            indexName = firstWord;
            if (!catalog.existIndex(indexName)) {
                op = INDEX_ERROR;
                return;
            }
            if (!getFirstWord(tmpCommand, firstWord))
                return;
            if (!strcmp(firstWord.c_str(), ";")) {
                op = DROP_INDEX;
                return;
            }
            return;
        }
    }

    else if (!strcmp(firstWord.c_str(), "execfile") || !strcmp(firstWord.c_str(), "EXECFILE")) {
        op = EXECFILE_ERROR;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";"))
            return;
        fileName = firstWord;
        if (!getFirstWord(tmpCommand, firstWord))
            return;
        if (!strcmp(firstWord.c_str(), ";")) {
            op = EXECFILE;
            return;
        }
        return;
    }
    op = TYPE_ERROR;
}
