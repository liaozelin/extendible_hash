#ifndef WORK_H_
#define WORK_H_
#include<stdio.h>

void init(char* a, char* b);

void insert_all_items(char* path);

void query_items(char* in_path, char* out_path);

void insert_record_into_page(char* line);

void query_item(int key);

void sort_record_list();

void output_record_list(FILE* out_file);

void output_hash_index(char* index_path);

void over();

#endif

