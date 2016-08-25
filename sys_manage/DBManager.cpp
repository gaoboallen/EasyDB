/*
 * DBManage.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: gaobo1993
 */

/*
 * TODO: map<string, int num_within_page> DBNameList; Done
 * TODO: create a table for table when create a DB; Done
 */

#include "DBManager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>

using namespace std;


DBManager::DBManager() {
	curManager = NULL;
	curDBname.clear();
	DBTable = new table();
    string tablename = "./myDB/DBlist";
    DBTable -> read_table_file(tablename);
    getDBList();
}

void DBManager::getDBList() {
	int DBNum = DBTable->get_record_cnt();
	cout << "getDBList: " << DBNum << "\n\n";
	for (int i = 0; i < DBNum; ++i) {
	    char* target_record = DBTable->find_record(1, i);
	    target_record += sizeof(char);
	    target_record += sizeof(int);
		char buf[DBNAMELEN];
		memcpy(buf, target_record, DBNAMELEN);
		target_record += DBNAMELEN;
		DBNameList[buf] = i;
	}
}

void DBManager::showAllDB() {
	if (!DBTable) {
		cout << "DB table invalid" << endl;
		return;
	}
	int DBNum = (int)DBNameList.size();
	cout << "showAllDB number: " << DBNum << endl;
	for (std::map<string,int>::iterator it=DBNameList.begin(); it!=DBNameList.end(); ++it)
	    std::cout << it->first << ", " << it->second << '\n';
	cout << "\n";
}

void DBManager::showDBTable() {
	if (!DBTable) {
		cout << "DB table invalid" << endl;
		return;
	}
	int DBNum = (int)DBTable->get_record_cnt();
	cout << "showDBTable number: " << DBNum << endl;
	for (int i = 0; i < DBNum; ++i)
		DBTable->view_record(0, i);
	cout << "\n";
}

bool DBManager::createDB(string DBname) {
	if (DBNameList.find(DBname)!=DBNameList.end()) {
		cout << "createDB: DB " << DBname << " already exists" << endl;
		return false;
	}
	string pathDBname = pathPrefix+DBname;
	if (mkdir(pathDBname.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==-1) {
		cout << "createDB failed\n";
		return false;
	}
	// create table for new DB
	table* newTable = new table();
	string tablename = pathPrefix+DBname+"/"+DBname+"tableList";
	string column_names[1];
	column_names[0] = "TableName";
	int column_types[] = {DBNAMELEN};
	newTable -> create_table_file(tablename, column_names, column_types, 1);
	newTable -> commit_changes();

	int tDBNL = (int)DBNameList.size();
	DBNameList[DBname] = tDBNL;
	vector<char*> data_buf;
	for(int i = 0; i < 1; i ++)	{
		data_buf.push_back(new char[DBNAMELEN]); // column_types[i] is string length
	}
	int data_len[1];
	data_len[0] = (int)DBname.size();
	memcpy(data_buf[0], DBname.c_str(), data_len[0]);
	DBTable->insert_record(data_buf, data_len);
	DBTable->commit_changes();
	return true;
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    int rv = remove(fpath);
    if (rv) perror(fpath);
    return rv;
}
bool DBManager::dropDB(string DBname) {
	std::map<string,int>::iterator it;
	if ((it=DBNameList.find(DBname))==DBNameList.end()) {
		cout << "dropDB: DB " << DBname << " does not exists" << endl;
		return false;
	}
	if (curManager!=NULL && DBname == curManager->getCurDB()) {
		delete curManager;
	}
	string pathDBname = pathPrefix+DBname;
//	if (remove((pathDBname+"/").c_str())==-1) {
	if (nftw((pathDBname).c_str(), unlink_cb, 64, FTW_DEPTH | FTW_PHYS)==-1) {
		cout << "dropDB failed\n";
		return false;
	}
	int deletedNum = it->second;
	DBNameList.erase(it);
//	DBTable->delete_record(0, deletedNum);
	vector<char*> data_buf; data_buf.push_back(new char[DBNAMELEN]);
	int dataLen[1] = {DBNAMELEN};

	for (int i = deletedNum; i < (int)DBNameList.size(); ++i) {
		char* target_record = DBTable->find_record(1, i+1);
		target_record += sizeof(char)+sizeof(int);
		memcpy(data_buf[0], target_record, dataLen[0]);
		DBTable->update_record(0, i, data_buf, dataLen);
		DBNameList[string(data_buf[0])]--;
	}
	DBTable->delete_record(0, (int)DBNameList.size());
	DBTable->commit_changes();
	return true;
}

bool DBManager::useDB(std::string DBname) {
	std::map<string,int>::iterator it;
	if ((it=DBNameList.find(DBname))==DBNameList.end()) {
		cout << "useDB: DB " << DBname << " does not exists" << endl;
		return false;
	}
	curManager = new tableManager(DBname);
	return true;
}

void DBManager::showDB(std::string DBname) {
	useDB(DBname);
	cout << "Show DB " << DBname << "\n\n";
	curManager->showAllT();
}
