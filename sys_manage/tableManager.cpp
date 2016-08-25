/*
 * tableManage.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: gaobo1993
 */

#include "DBManager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

using namespace std;



tableManager::tableManager(std::string DBname) {
	curDBname = DBname;
	TTable = new table();
	string tablename = pathPrefix+DBname+"/"+DBname+"tableList";
	TTable -> read_table_file(tablename);
	getTList();
}

void tableManager::getTList() {
	int TNum = TTable->get_record_cnt();
	cout << "getDBList: " << TNum << "\n\n";
	for (int i = 0; i < TNum; ++i) {
	    char* target_record = TTable->find_record(1, i);
	    target_record += sizeof(char);
	    target_record += sizeof(int);
		char buf[DBNAMELEN];
		memcpy(buf, target_record, DBNAMELEN);
		target_record += DBNAMELEN;
		TNameList[buf] = i;
	}
}

void tableManager::showAllT() {
	if (!TTable) {
		cout << "T table invalid" << endl;
		return;
	}
	int TNum = (int)TNameList.size();
	cout << "showAllT number: " << TNum << endl;
	for (std::map<string,int>::iterator it=TNameList.begin(); it!=TNameList.end(); ++it)
	    std::cout << it->first << ", " << it->second << '\n';
	cout << "\n";
}

void tableManager::showTTable() {
	if (!TTable) {
		cout << "T table invalid" << endl;
		return;
	}
	int DBNum = (int)TTable->get_record_cnt();
	cout << "showTTable number: " << DBNum << endl;
	for (int i = 0; i < DBNum; ++i)
		TTable->view_record(0, i);
	cout << "\n";
}

bool tableManager::createTable(std::string Tname, std::string* column_names_, int* column_types_, int column_cnt_) {
	if (TNameList.find(Tname)!=TNameList.end()) {
		cout << "createTable: Table " << Tname << " already exists" << endl;
		return false;
	}
	// create new table
	table* newTable = new table();
	string tablename = pathPrefix+curDBname+"/"+Tname;
	newTable -> create_table_file(tablename, column_names_, column_types_, column_cnt_);
	newTable -> commit_changes();

	int tDBNL = (int)TNameList.size();
	TNameList[Tname] = tDBNL;
	vector<char*> data_buf;
	for(int i = 0; i < 1; i ++)	{
		data_buf.push_back(new char[DBNAMELEN]); // column_types[i] is string length
	}
	int data_len[1];
	data_len[0] = (int)Tname.size();
	memcpy(data_buf[0], Tname.c_str(), data_len[0]);
	TTable->insert_record(data_buf, data_len);
	TTable->commit_changes();
	return true;
}

bool tableManager::dropTable(string Tname) {
	std::map<string,int>::iterator it;
	if ((it=TNameList.find(Tname))==TNameList.end()) {
		cout << "dropTable: Table " << Tname << " does not exists" << endl;
		return false;
	}
	string pathTname = pathPrefix+curDBname+"/"+Tname+".table";
	if (remove(pathTname.c_str())==-1) {
		cout << "dropDB failed\n";
		return false;
	}
	int deletedNum = it->second;
	TNameList.erase(it);
//	DBTable->delete_record(0, deletedNum);
	vector<char*> data_buf; data_buf.push_back(new char[DBNAMELEN]);
	int dataLen[1] = {DBNAMELEN};

	for (int i = deletedNum; i < (int)TNameList.size(); ++i) {
		char* target_record = TTable->find_record(1, i+1);
		target_record += sizeof(char)+sizeof(int);
		memcpy(data_buf[0], target_record, dataLen[0]);
		TTable->update_record(0, i, data_buf, dataLen);
		TNameList[string(data_buf[0])]--;
	}
	TTable->delete_record(0, (int)TNameList.size());
	TTable->commit_changes();
	return true;
}

void tableManager::showTable(string Tname) {
	std::map<string,int>::iterator it;
	if ((it=TNameList.find(Tname))==TNameList.end()) {
		cout << "showTable: Table " << Tname << " does not exists\n\n";
		return;
	}
	printf("Table info of %s:\n", Tname.c_str());
	table* newTable = new table();
	string tablename = pathPrefix+curDBname+"/"+Tname;
	newTable->read_table_file(tablename);
	newTable->view_table_info(0);
	printf("\n");
}

std::string tableManager::getCurDB() {
	return curDBname;
}
