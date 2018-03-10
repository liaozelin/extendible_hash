#include<stdio.h>
#include<string.h>
#include"page.h"
#include"setting.h"
#include"buffer_manager.h"

void init_bucket_page(page_t* page, int depth) {
    page->type = BUCKET;
    page->depth = depth;

    ((int*)page->head)[DEPTH_POS] = depth;
    ((int*)page->head)[SIZE_POS] = 0;
    ((int*)page->head)[FREE_BEGIN_OFFSET] = HEAD_LENGTH;
}

int get_depth(page_t* page) {
    return ((int*)page->head)[DEPTH_POS];
}

void set_depth(page_t* page, int depth) {
    ((int*)page->head)[DEPTH_POS] = depth;
}

int get_size(page_t* page) {
    return ((int*)page->head)[SIZE_POS];
}

void set_size(page_t* page, int size) {
    ((int*)page->head)[SIZE_POS] = size;
}

void* get_free_begin(page_t* page) {
    int offset = ((int*)page->head)[FREE_BEGIN_OFFSET];
    return page->head + offset;
}

void set_free_begin_offset(page_t* page, int offset) {
    ((int*)page->head)[FREE_BEGIN_OFFSET] = offset;
}

void* get_free_end(page_t* page) {
    return page->head + PAGE_SIZE;
}

void copy_page(page_t* src, page_t* dest) {
    memcpy(dest->head, src->head, PAGE_SIZE);
    dest->type = src->type;
    dest->depth = src->depth;
    // since dest is just a temp page, it shouldn't be written into disk
    // set it's id as -1, then it wouldn't be find and must be swap out
    // when swap out, since dest->dirty is false, it wouldn't be written into disk
    dest->id = -1;
    dest->dirty = 0;
}

/******************************index page*********************************/
void set_bucket_id_by_hash_value(int hash_value, int bucket_id) {
    page_t* index_page = get_index_page(hash_value / index_pair_per_page); // buffer_manager.c
    ((int*)index_page->head)[hash_value % index_pair_per_page] = bucket_id;
    // data had been modified, set dirty as true
    index_page->dirty = 1;
}

int get_bucket_id_by_hash_value(int hash_value) {
    if (DEBUG) printf("get_bucket_id_by_hash_value, hash_value: %d\n", hash_value);

    page_t* index_page = get_index_page(hash_value / index_pair_per_page); // buffer_manager.c
    return ((int*)index_page->head)[hash_value % index_pair_per_page];
}

