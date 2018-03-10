#include"tracker.h"
#include"setting.h"

void tracker_init() {
    IO_in_count = IO_out_count = 0;
    double_index_count = 0;
    split_count = 0;
    insert_start_time = insert_over_time = 0;
    query_start_time = query_over_time = 0;
}

void IO_in_increase() {
    IO_in_count++;
}

void IO_out_increase() {
    IO_out_count++;
}

void double_index_increase() {
    double_index_count++;
}

void split_page_increase() {
    split_count++;
}

void insert_start() {
    //time(insert_start_time);
    insert_start_time = time(0);
}

void insert_over() {
    insert_over_time = time(0);
}

void query_start() {
    query_start_time = time(0);
}

void query_over() {
    query_over_time = time(0);
}

void tracker_info_output() {
    printf("IO count:\n");
    printf("\tread in: %d, write out: %d, total: %d\n",
            IO_in_count, IO_out_count, IO_in_count + IO_out_count);
    printf("Page count:\n");
    printf("\tbucket: %d, index: %d\n",
            bucket_page_count, index_page_count);
    printf("Double index count: %d\n", double_index_count);
    printf("Split count: %d\n", split_count);
    printf("\nInsert time: %ld seconds\n", insert_over_time - insert_start_time);
    printf("Query time: %ld seconds\n", query_over_time - query_start_time);
}

