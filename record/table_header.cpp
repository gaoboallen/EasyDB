#include <stdio.h>
#include <cstring>
#include <string>
#include "table_header.h"
using namespace std;

table_header::table_header()
{
	record_cnt = 0;
    valid = false;
}

void table_header::create_new_header(const char* name, int name_len_, std::string* column_names_, int* column_types_, int column_cnt_)
{
    memset(table_name, 0, sizeof(table_name));

    column_cnt = column_cnt_;

    for(int i = 0; i < 16; i ++)
    {
        memset(column_names[i], 0, sizeof(column_names[i]));
    }

    memset(column_types, 0, sizeof(column_types));

    for(int i = 0; i < column_cnt; i ++)
    {
        memcpy(column_names[i], column_names_[i].c_str(), column_names_[i].size());
    }

    memcpy(column_types, column_types_, column_cnt_ * sizeof(int));

    record_len = sizeof(int) + sizeof(char);

    for(int i = 0; i < column_cnt; i ++)
    {
        if(column_types[i] >= 0)
            record_len += column_types[i];
        else if(column_types[i] == -1)
            record_len += sizeof(int);
    }

    memcpy(table_name, name, name_len_);

    record_cnt = 0;

    int record_padding_len = 0;
    for(int i = 0; i < 8192; i ++)
        if(8192 % (record_len + i) == 0)
        {
            record_padding_len = i;
            break;
        }
    record_len_padded = record_len + record_padding_len;
    record_cnt_per_page = 8192 / record_len_padded;

    next_rid = 0;

    memset(free_space_table, 0xFF, sizeof(free_space_table));

    valid = true;

    return;
}


void table_header::read_header(BufType page)
{
    char* page_ = (char*)page;

    memcpy(table_name, page_, sizeof(table_name));
    page_ += sizeof(table_name);

    memcpy(&record_cnt, page_, sizeof(record_cnt));
    page_ += sizeof(record_cnt);

    memcpy(&record_cnt_per_page, page_, sizeof(record_cnt_per_page));
    page_ += sizeof(record_cnt_per_page);

    memcpy(&record_len, page_, sizeof(record_len));
    page_ += sizeof(record_len);

    memcpy(&record_len_padded, page_, sizeof(record_len_padded));
    page_ += sizeof(record_len_padded);

    memcpy(&next_rid, page_, sizeof(next_rid));
    page_ += sizeof(next_rid);

    for(int i = 0; i < 16; i ++)
    {
        memcpy(column_names[i], page_, 64);
        page_ += 64;
    }

    memcpy(column_types, page_, sizeof(column_types));
    page_ += sizeof(column_types);

    memcpy(&column_cnt, page_, sizeof(column_cnt));
    page_ += sizeof(column_cnt);

    memcpy(free_space_table, page_, sizeof(free_space_table));

    valid = true;

    return;
}

void table_header::write_header(BufType page)
{
    char* page_ = (char*)page;

    memcpy(page_, table_name, sizeof(table_name));
    page_ += sizeof(table_name);

    memcpy(page_, &record_cnt, sizeof(record_cnt));
    page_ += sizeof(record_cnt);

    memcpy(page_, &record_cnt_per_page, sizeof(record_cnt_per_page));
    page_ += sizeof(record_cnt_per_page);

    memcpy(page_, &record_len, sizeof(record_len));
    page_ += sizeof(record_len);

    memcpy(page_, &record_len_padded, sizeof(record_len_padded));
    page_ += sizeof(record_len_padded);

    memcpy(page_, &next_rid, sizeof(next_rid));
    page_ += sizeof(next_rid);

    for(int i = 0; i < 16; i ++)
    {
        memcpy(page_, column_names[i], 64);
        page_ += 64;
    }

    memcpy(page_, column_types, sizeof(column_types));
    page_ += sizeof(column_types);

    memcpy(page_, &column_cnt, sizeof(column_cnt));
    page_ += sizeof(column_cnt);

    memcpy(page_, free_space_table, sizeof(free_space_table));

    valid = true;

    return;
}

void table_header::update_header(int page_changed, bool page_is_full, bool is_insertion)
{
    if(is_insertion)
    {
        next_rid ++;
        record_cnt ++;
    }
    else
        record_cnt --;

    page_changed --;
    int tmp_index = page_changed / 8;
    int tmp_i = page_changed % 8;
    if(page_is_full)
    {
        cout<< "Page is full!" << endl;
        char mask = ~ (0x01 << tmp_i);
        free_space_table[tmp_index] = (free_space_table[tmp_index] & mask);
    }
    else
    {
        char mask = (0x01 << tmp_i);
        free_space_table[tmp_index] = (free_space_table[tmp_index] | mask);
    }

    return;
}
