#include "bufmanager/BufPageManager.h"
#include "fileio/FileManager.h"
#include "utils/pagedef.h"
#include "record/table.h"
#include "sys_manage/DBManager.h"
#include "sys_manage/tableManager.h"
#include <iostream>
#include <cstring>
#include <string>
#include <vector>

using namespace std;

int main() {

//	// create new DB
//    table* test_table = new table();
//    string tablename = "./myDB/DBlist";
//    string column_names[1];
//    column_names[0] = "DBName";
//    int column_types[] = {30};
//    test_table -> create_table_file(tablename, column_names, column_types, 1);
//	test_table -> commit_changes();

	DBManager *test_DBM = new DBManager();
	test_DBM->showAllDB();
	test_DBM->showDBTable();
	test_DBM->createDB("FirstDB");
	test_DBM->createDB("SecondDB");
	test_DBM->createDB("ThirdDB");
	test_DBM->createDB("FourthDB");
	test_DBM->showDBTable();
	test_DBM->showAllDB();

	test_DBM->dropDB("SecondDB");
	test_DBM->dropDB("SecondDB");
	test_DBM->showDBTable();
	test_DBM->showAllDB();

	test_DBM->showDB("FirstDB");

	tableManager *test_TM = new tableManager("FirstDB");
	test_TM->showAllT();
	test_TM->showTTable();

	string column_names[3];
	column_names[0] = "attr_0_string";
	column_names[1] = "attr_1_int";
	column_names[2] = "attr_2_string";
	int column_types[] = {30, -1, 20};
	test_TM->createTable("FirstT", column_names, column_types, 3);
	test_TM->createTable("SecondT", column_names, column_types, 3);
	test_TM->createTable("ThirdT", column_names, column_types, 3);
	test_TM->showAllT();
	test_TM->showTTable();

	test_TM->dropTable("SecondT");
	test_TM->dropTable("SecondT");
	test_TM->showAllT();
	test_TM->showTTable();

	test_TM->showTable("FirstT");
	test_TM->showTable("SecondT");
	test_TM->showTable("ThirdT");

    return 0;
}
