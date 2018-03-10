#ifndef TRACKER_H_
#define TRACKER_H_
#include<stdio.h>
#include<time.h>

int IO_in_count, IO_out_count;
int double_index_count;
int split_count;
time_t insert_start_time, insert_over_time;
time_t query_start_time, query_over_time;

void tracker_init();

void IO_in_increase();

void IO_out_increase();

void double_index_increase();

void split_page_increase();

void insert_start();

void insert_over();

void query_start();

void query_over();

void tracker_info_output();

#endif

