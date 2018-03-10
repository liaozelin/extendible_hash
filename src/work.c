#include<stdio.h>
#include<string.h>
#include"work.h"
#include"setting.h"
#include"buffer_manager.h"
#include"IO_manager.h"
#include"hash.h"
#include"page.h"
#include"record.h"
#include"tracker.h"

record_t record_list[20];
char buffer[200];
int record_count = 0;

void init(char* bucket_path, char* index_path) {
    setting_init();
    tracker_init();
    buffer_init();
    IO_manager_init(bucket_path, index_path);
}

void insert_all_items(char* path) {
    printf("Start inserting items...\n");
    insert_start(); // tracker.h

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Open file failed: %s\n", path);
        return;
    }

    printf("open file success\n");

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    long record_num = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        record_num++;
        printf("Inserting item: %ld\n", record_num);
        insert_record_into_page(line);
    }

    fclose(file);

    printf("Insert Done...\n");
    insert_over(); // tracker.h
}

void insert_record_into_page(char* line) {
    record_t record;
    string2record(line, &record);
    int key = get_key(&record); // record.h
    int hash_value = get_hash_value(key, global_depth);  // hash.h
    int bucket_id = get_bucket_id_by_hash_value(hash_value); // page.h
    while (!has_enough_space(&record, bucket_id)) {
        split_page(bucket_id, hash_value);
        hash_value = get_hash_value(key, global_depth);
        bucket_id = get_bucket_id_by_hash_value(hash_value); 

    }
    write_record_into_page(&record, bucket_id);
}

void query_items(char* in_path, char* out_path) {
    printf("Start querying...\n");
    query_start(); // tracker.h

    FILE* in_file = fopen(in_path, "r");
    FILE* out_file = fopen(out_path, "w+");

    int n, key;
    fscanf(in_file, "%d", &n);
    while (n--) {
        fscanf(in_file, "%d", &key);
        query_item(key);

        sort_record_list();
        output_record_list(out_file);
    }

    fclose(in_file);
    fclose(out_file);

    printf("Query Done...\n");
    query_over(); // tracker.h
}

void query_item(int key) {
    record_count = 0;
    int hash_value = get_hash_value(key, global_depth);
    page_t* page = get_bucket_page_by_hash_value(hash_value);
    int size = get_size(page);
    record_t record;
    int offset = HEAD_LENGTH;
    for (int i = 0; i < size; ++i) {
        offset = read_record_from_page(page, offset, &record);
        if (record.orderkey == key) {
            record_list[record_count++] = record;
        }
    }
}

void sort_record_list() {
    for (int i = 0; i < record_count - 1; ++i) {
        for (int j = i + 1; j < record_count; ++j) {
            if (record_list[i].partkey < record_list[j].partkey) {
                record_t tmp = record_list[i];
                record_list[i] = record_list[j];
                record_list[j] = tmp;
            }
        }
    }
}

void output_record_list(FILE* out_file) {
    for (int i = 0; i < record_count; ++i) {
        record2string(&record_list[i], buffer);
        fprintf(out_file, "%s\n", buffer);
    }
    fprintf(out_file, "-1\n");
    printf("write result done.\n");
}

void output_hash_index(char* index_path) {
    FILE* file = fopen(index_path, "w");
    page_t* page;
    int index, bucket_id;
    int t;
    for (int i = 0; i < index_page_count; ++i) {
        page = get_index_page(i);
        t = i * index_pair_per_page;
        for (int j = 0; j < index_pair_per_page; ++j) {
            index = t + j;
            bucket_id = ((int*)page->head)[j];
            //printf("%d ==> %d\n", index, bucket_id);
            fprintf(file, "%d ==> %d\n", index, bucket_id);
        }
    }  
    /** int num = index_page_count * index_pair_per_page;
      * for (int i = 0; i < num; ++i) {
      *     bucket_id = get_bucket_id_by_hash_value(i);
      *     fprintf(file, "%d ==> %d\n", i, bucket_id);
      * }  */
    fclose(file);
}

void over() {
    save_pages(); // buffer_manager.h
    close_files(); // IO_manager.h

    tracker_info_output(); // tracker.h
}
