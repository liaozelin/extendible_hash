#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include"setting.h"
#include"IO_manager.h"
#include"buffer_manager.h"
#include"hash.h"
#include"page.h"
#include"record.h"
#include"tracker.h"

FILE* bucket_file;
FILE* index_file;

// used for spliting bucket page
int temp_page_id; // always use a temp page for split page
page_t* page1;
page_t* page2;

void IO_manager_init(char* bucket_path, char* index_path) {
    printf("IO_manager_init...\n");

    bucket_file = fopen(bucket_path, "w+");
    index_file = fopen(index_path, "w+");
    if (bucket_file == NULL) printf("open bucket.txt failed...\n");
    else printf("open bucket.txt success...\n");
    if (index_file == NULL) printf("open index.txt failed...\n");
    else printf("open index.txt success...\n");

    temp_page_id = create_new_page(BUCKET, 1);
    // In the beginning, global_depth is 1 and
    // there are two bucket page and one index page
    int bucket_0 = create_new_page(BUCKET, 1); // create two initial bucket page
    int bucket_1 = create_new_page(BUCKET, 1);
    create_new_page(INDEX, -1); // create one initial index page
    //page_t* index_page = get_index_page(0);
    // set (hash_value, bucket_id) tuple of index page correctly
    set_bucket_id_by_hash_value(0, bucket_0);
    set_bucket_id_by_hash_value(1, bucket_1);
}

int create_new_page(enum page_type type, int depth) {
    page_t* new_page = find_avaliable_page(); // buffer_manager.h 
    new_page->type = type;
    if (type == BUCKET) {
        new_page->id = bucket_page_count;
        // init the empty page
        init_bucket_page(new_page, depth); // page.h
        bucket_page_count++;
    } else if (type == INDEX) {
        new_page->id = index_page_count;
        index_page_count++;
    }
    // after initing, the page has been dirty
    new_page->dirty = 1;
    return new_page->id;
}

void load_page_from_disk(page_t* page) {
    if (DEBUG) printf("load_page_from_disk...\n");
    FILE* file = page->type == BUCKET ? bucket_file : index_file;

    assert(file != NULL);
    
    fseek(file, page->id * PAGE_SIZE, SEEK_SET);
    fread(page->head, PAGE_SIZE, 1, file);
    page->dirty = 0;

    IO_in_increase(); // tracker.h
}

void write_page_into_disk(page_t* page) {
    FILE* file = page->type == BUCKET ? bucket_file : index_file;

    assert(file != NULL);
    
    fseek(file, page->id * PAGE_SIZE, SEEK_SET);
    fwrite(page->head, PAGE_SIZE, 1, file);
    page->dirty = 0;

    IO_out_increase(); // tracker.h
}

void double_index() {
    int n = (1 << global_depth); // n is current size of index
    int i, j;
    int bucket_id;

    j = (n << 1); // now j is target size of index
    while (j > index_pair_per_page * index_page_count) {
        // if total size of existed page is smaller than target size,
        // create a new one
        create_new_page(INDEX, -1);
    }

    // start double index
    // for example, 011 and 111 pointer to the same page 11
    for (i = 0; i < n; ++i) {
        j = n + i;
        bucket_id = get_bucket_id_by_hash_value(i);
        set_bucket_id_by_hash_value(j, bucket_id);
    }

    global_depth++;

    double_index_increase(); // tracker.h
}

void split_page(int bucket_id, int hash_value) {
    if (DEBUG) printf("split_page...\n");

    page_t* page = get_bucket_page(bucket_id);
    int depth = get_depth(page);

    if (depth == global_depth) {
        // if local depth == global_depth, must double index first
        double_index();
    }

    page = get_bucket_page(bucket_id);

    // the higher page, after split
    int page2_id = create_new_page(BUCKET, depth + 1);

    // low and high index, such as 011 and 111
    int old_index = ((1 << depth) - 1) & hash_value;
    int new_index = (1 << depth) + old_index;
    // note: 此处一定要将与hash_value相关的index设置正确
    int n = 1 << global_depth;
    int inc = 1 << depth;
    for (int i = old_index; i < n; i += inc) {
        if (get_bucket_id_by_hash_value(i) == bucket_id) {
            if (((i >> depth) & 1) == 1) {
                set_bucket_id_by_hash_value(i, page2_id);
            }
        }
    }

    depth++;
    set_depth(page, depth);
    page->depth = depth;

    // page1 is low page, page2 is high page
    // for example: page1 is 011 and page2 is 111
    //temp_page = find_avaliable_page(); // buffer_manager.h
    // 因为上一句可能会获取到缓冲区中正在此函数内被使用的页
    // 所以page1 = page这种写法极有可能出错
    // eg: temp_page获取的刚好是上面page指向的页,
    //     如果写page1 = page, page1和temp_page指向同一页
    //     因此正确的做法是重新调用get_bucket_page获取缓冲页
    page1 = get_bucket_page(bucket_id);
    page2 = get_bucket_page(page2_id);

    // copy page into temp_page
    page_t* temp_page = get_bucket_page(temp_page_id);
    copy_page(page, temp_page);
    // init page1 and page2 as empty pages, id won't change in init function
    init_bucket_page(page1, depth);
    init_bucket_page(page2, depth);

    // write records in temp_page into page1 and page2
    record_t record;
    int offset = HEAD_LENGTH; // page.h

    assert(get_size(page1) == 0);
    assert(get_size(page2) == 0);

    //for (int i = 0; i < total_size; ++i) {
    while (offset != -1) {
        offset = read_record_from_page(temp_page, offset, &record);

        int index = get_hash_value(get_key(&record), depth);
        assert(index == old_index || index == new_index);
        if (index == old_index) {
            write_record_into_page(&record, page1->id);
        } else if (index == new_index) {
            write_record_into_page(&record, page2->id);
        }
    }

    //assert(total_size == get_size(page1) + get_size(page2));

    split_page_increase(); // tracker.h
}

void close_files() {
    fclose(bucket_file);
    fclose(index_file);
}

