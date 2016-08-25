#ifndef table_header_h
#define table_header_h

#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "../utils/pagedef.h"
#include <string>

class table_header
{
private:
    char table_name[1024];
    int record_cnt;
    int record_cnt_per_page;
    int record_len;
    int record_len_padded;
    int next_rid;
    char column_names[16][64];
    int column_types[16];
    int column_cnt;
    char free_space_table[6058];
    bool valid;
public:
    table_header();
    void create_new_header(const char* name, int name_len_, std::string* column_names_, int* column_types, int column_cnt_);
    void read_header(BufType page);
    void write_header(BufType page);
    void update_header(int page_changed, bool page_is_full, bool is_insertion);
    char* get_table_name()
    {
        return table_name;
    }
    int get_record_cnt()
    {
        return record_cnt;
    }
    int get_record_cnt_per_page()
    {
        return record_cnt_per_page;
    }
    int get_record_len()
    {
        return record_len;
    }
    int get_record_len_padded()
    {
        return record_len_padded;
    }
    int get_next_rid()
    {
        return next_rid;
    }
    int get_column_type(int i)
    {
        return column_types[i];
    }
    int get_column_cnt()
    {
        return column_cnt;
    }
    char* get_column_name(int i)
    {
        return column_names[i];
    }
    int get_available_page()
    {
        for(int i = 0; i < (int)(sizeof(free_space_table) * sizeof(char)); i ++)
        {
            int tmp_index = i / 8;
            int tmp_i = i % 8;
            char mask = (0x01 << tmp_i);
            if((mask & free_space_table[tmp_index]) == mask)
                return i + 1;
        }
        return -1;
    }
};

#endif /* table_header_h */
