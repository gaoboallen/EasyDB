/*
 * tableManage.h
 *
 *  Created on: Nov 15, 2015
 *      Author: gaobo1993
 */

#ifndef SYS_MANAGE_TABLEMANAGER_H_
#define SYS_MANAGE_TABLEMANAGER_H_

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

class tableManager {
private:
	table *TTable;
	std::string curDBname;
	std::map<std::string, int> TNameList; // <DBName, num_within_page>
public:
	tableManager(std::string DBname);
	void getTList();
	void showAllT();
	void showTTable();
	bool createTable(std::string Tname, std::string* column_names_, int* column_types_, int column_cnt_);
	bool dropTable(std::string Tname);
	void showTable(std::string Tname);
	std::string getCurDB();
};


#endif /* SYS_MANAGE_TABLEMANAGER_H_ */
