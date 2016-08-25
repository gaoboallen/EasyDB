#include <stdio.h>
#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "../utils/pagedef.h"
#include "table.h"
#include "table_header.h"
#include <string>
#include <cstring>
#include <vector>
using namespace std;

table::table()
{
	file_id = 0;
    fm = NULL;
    bpm = NULL;
    header = NULL;
    valid = false;
}

bool table::create_table_file(std::string table_name, std::string* column_names_, int* column_types_, int column_cnt_)
{
    table_filename = table_name + ".table";

    header = new table_header();
    header -> create_new_header(table_filename.c_str(), (int)table_filename.size(), column_names_, column_types_, column_cnt_);

    fm = new FileManager();

    if (!fm -> createFile(table_filename.c_str())) {
		cout << "Create table file " << table_filename << " failed";
		return false;
	}

    if (!fm -> openFile(table_filename.c_str(), file_id)) {
		cout << "Open table file " << table_filename << " file failed";
		return false;
	}

    bpm = new BufPageManager(fm);

    valid = true;

    return true;
}

bool table::read_table_file(std::string table_name)
{
    table_filename = table_name + ".table";

    header = new table_header();

    fm = new FileManager();
    if (!fm -> openFile(table_filename.c_str(), file_id)) {
    	cout << "Open table file " << table_filename << " file failed";
    	return false;
    }

    bpm = new BufPageManager(fm);

    int index = 0;
    BufType header_page = bpm -> getPage(file_id, 0, index);
    header -> read_header(header_page);
    bpm -> markDirty(index);

    valid = true;

    return true;
}

bool table::insert_record(const vector<char*> data, const int* data_len)
{
    int target = header -> get_available_page();
    if(target == -1)
        return false;

    cout << "Available page: " << target << endl;

    int index = 0;
    BufType target_page = bpm -> getPage(file_id, target, index);
    char* target_page_ = (char*)target_page;
    bpm -> markDirty(index);
    int offset = 0;
    int available_cnt = 0;
    for(int i = header -> get_record_cnt_per_page() - 1 ; i > -1; i --)
    {
        char valid_flag = *(target_page_ + i * header -> get_record_len_padded());
        if(valid_flag != 0x11)
        {
            offset = i * header -> get_record_len_padded();
            available_cnt ++;
        }
    }
    char* insert_location = (char*)target_page_ + offset;

    cout << "Available count: " << available_cnt << endl;

    memset(insert_location, 0, header -> get_record_len_padded());

    // Store occupied flag
    char tmp_flag = 0x11;
    memcpy(insert_location, &tmp_flag, sizeof(char));
    insert_location += sizeof(char);

    // Store RID
    int tmp_rid = header -> get_next_rid();
    memcpy(insert_location, &tmp_rid, sizeof(int));
    insert_location += sizeof(int);

    // Store the record itself
    for(int i = 0; i < header -> get_column_cnt(); i ++)
    {
        memcpy(insert_location, data[i], data_len[i]);
        if(header -> get_column_type(i) == -1)
            insert_location += sizeof(int);
        else
            insert_location += header -> get_column_type(i);
    }

    header -> update_header(target, (available_cnt == 1), true);

    return true;
}

char* table::find_record(int page_num, int num_within_page)
{
    int index = 0;
    BufType target_page = bpm -> getPage(file_id, page_num, index);
    char* record_location = (char*)target_page + num_within_page * header -> get_record_len_padded();
    bpm -> markDirty(index);
    return record_location;
}

void table::view_table_info(int page_num) {
    page_num ++;
    printf("%-4s", "RID");
    for(int i = 0; i < header -> get_column_cnt(); i ++) {
        printf("|%-15s", header -> get_column_name(i));
        if (header->get_column_type(i)==-1) printf("(int)");
        else printf("(%d)", header->get_column_type(i));
    }
    printf("\n");
}

void table::view_record(int page_num, int num_within_page)
{
    page_num ++;
    printf("%-4s", "RID");
    for(int i = 0; i < header -> get_column_cnt(); i ++)
        printf("|%-20s", header -> get_column_name(i));
    printf("\n");

    char* target_record = find_record(page_num, num_within_page);
    target_record += sizeof(char);
    printf("%-4d", *(int*)target_record);
    target_record += sizeof(int);
    for(int i = 0; i < header -> get_column_cnt(); i ++)
    {
        if(header -> get_column_type(i) == -1)
        {
            printf("|%-20d", *(int*)target_record);
            target_record += sizeof(int);
        }
        else
        {
            char buf[header -> get_column_type(i)];
            memcpy(buf, target_record, header -> get_column_type(i));
            target_record += header -> get_column_type(i);
            printf("|%-20s", buf);
        }
    }
    printf("\n\n");

    return;
}

void table::delete_record(int page_num, int num_within_page)
{
    page_num ++;
    char* target_record = find_record(page_num, num_within_page);
    memset(target_record, 0, header -> get_record_len_padded());
    header -> update_header(page_num, false, false);
    return;
}

void table::update_record(int page_num, int num_within_page, const vector<char*> new_data, const int* data_len)
{
    page_num ++;
    char* target_record = find_record(page_num, num_within_page);
    target_record += sizeof(char);
    target_record += sizeof(int);

    memset(target_record, 0, header -> get_record_len_padded() - sizeof(int) - sizeof(char));
    for(int i = 0; i < header -> get_column_cnt(); i ++)
    {
        if(header -> get_column_type(i) == -1)
            memset(target_record, 0, sizeof(int));
        else
            memset(target_record, 0, header -> get_column_type(i));
        memcpy(target_record, new_data[i], data_len[i]);
        if(header -> get_column_type(i) == -1)
            target_record += sizeof(int);
        else
            target_record += header -> get_column_type(i);
    }

    return;
}

void table::commit_changes()
{
    int index = 0;
    BufType header_page = bpm -> getPage(file_id, 0, index);
    header -> write_header(header_page);
    bpm -> markDirty(index);

    bpm -> close();

    return;
}

int table::get_record_cnt() {
	if (!valid) return -1;
	return this->header->get_record_cnt();
}
