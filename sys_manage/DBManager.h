/*
 * DBManage.h
 *
 *  Created on: Nov 15, 2015
 *      Author: gaobo1993
 */

#ifndef SYS_MANAGE_DBMANAGER_H_
#define SYS_MANAGE_DBMANAGER_H_

#include "tableManager.h"
#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "../utils/pagedef.h"
#include "../record/table_header.h"
#include "../record/table.h"
#include <cstring>
#include <string>
#include <vector>
#include <map>

#define DBNAMELEN 30
#define pathPrefix "./myDB/"

class DBManager {
private:
	table *DBTable;
	std::string curDBname;
	tableManager *curManager;
	std::map<std::string, int> DBNameList; // <DBName, num_within_page>
public:
	DBManager();
	void getDBList();
	void showAllDB();
	void showDBTable();
	bool createDB(std::string DBname);
	bool dropDB(std::string DBname);
	bool useDB(std::string DBname);
	void showDB(std::string DBname);  // will call useDB(DBname) first
};

#endif /* SYS_MANAGE_DBMANAGER_H_ */
