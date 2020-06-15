#ifndef RECORD_H
#define RECORD_H
#include "MiniSQL.h"
#include "BufferManager.h"

extern BufferManager buf;

class RecordManager 
{
private:
	void splitRow(const Table &tableInfor, string &record, Row &row);
	void combineRow(const Table &tableInfor, string &record, Row &row);

public:
	int comparator(const Table &tableInfor, const Row &row, const Condition &condition);
	void selectRecords(const Table &tableInfor, Data &returnData);
	void selectRecords(const Table &tableInfor, Data &returnData, const Condition &condition);
	void insertRecords(Table &tableInfor, Row &insertRow);
	int deleteRecords(const Table &tableInfor);
	int deleteRecords(const Table &tableInfor, const Condition &condition);
	void dropTable(Table &tableInfor);
	void createTable(Table &tableInfor);
};

#endif