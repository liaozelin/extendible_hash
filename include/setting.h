#ifndef SETTING_H_
#define SETTING_H_

// by set these two define value to make different version
#define BUFFER_SIZE  (128) // the size of buffer page
#define LOW_HASH (1)

//const char* DATA_PATH = "data/";
//const char* INDEX_PATH = "data/";

#define DEBUG (0)

int global_depth;
int bucket_page_count;
int index_page_count;

void setting_init();

int get_global_depth();

void increase_global_depth();

int get_bucket_page_count();

void increase_bucket_page_count();

int get_index_page_count();

void increase_index_page_count();

#endif
