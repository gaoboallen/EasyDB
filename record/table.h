#ifndef table_h
#define table_h

#include "table_header.h"
#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "../utils/pagedef.h"
#include <string>
#include <vector>

class table
{
private:
    table_header* header;
    FileManager* fm;
    BufPageManager* bpm;
    std::string table_filename;
    int file_id;
    bool valid;
public:
    table();
    bool read_table_file(std::string table_name);
    bool create_table_file(std::string table_name, std::string* column_names_, int* column_types_, int column_cnt_);
    bool insert_record(const vector<char*> data, const int* data_len);
    char* find_record(int page_num, int num_within_page);
    void view_table_info(int page_num);
    void view_record(int page_num, int num_within_page);
    void delete_record(int page_num, int num_within_page);
    void update_record(int page_num, int num_within_page, const vector<char*> new_data, const int* data_len);
    void commit_changes();
    int get_record_cnt();
};

#endif /* table_h */
