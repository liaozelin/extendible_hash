#include<stdio.h>
#include<malloc.h>
#include"buffer_manager.h"
#include"IO_manager.h"
#include"page.h"
#include"setting.h"

struct buffer_node_t buffer_list[BUFFER_SIZE]; // list of buffer page

int clock_size; // when clock_size == BUFFER_SIZE, the buffer is full,
                // and should swap out old page
int clock_head; // while swaping page, find an index started from clock_head

void buffer_init() {
    printf("buffer_init...\n");
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        buffer_list[i].page = NULL;
        buffer_list[i].R = 0;
    }
    clock_size = 0;
    clock_head = 0;
}

page_t* get_bucket_page(int buffer_id) {
    return load_page(buffer_id, BUCKET); 
}

page_t* get_bucket_page_by_hash_value(int hash_value) {
    int bucket_id = get_bucket_id_by_hash_value(hash_value); // page.c
    return load_page(bucket_id, BUCKET);
}

page_t* get_index_page(int index_id) {
    return load_page(index_id, INDEX);
}

int find_avaliable_page_pos() {
    int pos;
    if (clock_size < BUFFER_SIZE) { // the buffer isn't full
        pos = clock_size++;
        buffer_list[pos].page = (page_t*)malloc(sizeof(page_t));
        buffer_list[pos].page->head = malloc(PAGE_SIZE); // page.h
    } else {
        while(buffer_list[clock_head].R == 1) {
            buffer_list[clock_head].R = 0;
            clock_head = (clock_head + 1) % BUFFER_SIZE;
        }
        // before using the buffer page, swap out old page
        write_page(buffer_list[clock_head].page);
        pos = clock_head;
    }
    return pos;
}

page_t* find_avaliable_page() {
    int pos = find_avaliable_page_pos();
    buffer_list[pos].R = 1;
    return buffer_list[pos].page;
}

int find_page(int page_id, enum page_type type) {
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        if (buffer_list[i].page != NULL &&
                buffer_list[i].page->id == page_id && buffer_list[i].page->type == type)
            return i;
    }
    return -1;
}

page_t* load_page(int page_id, enum page_type type) {
    if (DEBUG) printf("load_page...\n");
    int pos = find_page(page_id, type);
    if (pos != -1) { // find the page in buffer
        // find a page in buffer, set R = 1
        buffer_list[pos].R = 1;
        return buffer_list[pos].page;
    } else { // can't find the page in buffer, swap in the new page
        pos = find_avaliable_page_pos();
        // swap in a new page, set R = 1
        buffer_list[pos].R = 1;
        buffer_list[pos].page->id = page_id;
        buffer_list[pos].page->type = type;
        load_page_from_disk(buffer_list[pos].page); // IO_manager.h
        return buffer_list[pos].page;
    }
}

void write_page(page_t* page) {
    if (page->dirty)
        write_page_into_disk(page); // IO_manager.h
}

void save_pages() {
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        write_page(buffer_list[i].page);
        free(buffer_list[i].page->head);
        free(buffer_list[i].page);
    }
}

