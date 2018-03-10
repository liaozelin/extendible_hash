#include"setting.h"

void setting_init() {
    global_depth = 1;
    index_page_count = 0;
    bucket_page_count = 0;
}

int get_global_depth() {
    return global_depth;
}

void increase_global_depth() {
    global_depth++;
}

int get_bucket_page_count() {
    return bucket_page_count;
}

void increase_bucket_page_count() {
    bucket_page_count++;
}

int get_index_page_count() {
    return index_page_count;
}

void increase_index_page_count() {
    index_page_count++;
}
