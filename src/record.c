#include<stdio.h>
#include<string.h>
#include<assert.h>
#include"record.h"
#include"buffer_manager.h"
#include"IO_manager.h"
#include"page.h"
#include"hash.h"
#include"setting.h"

char* string2record(char* start, record_t* record) {
    if (DEBUG) printf("string2record: %s", start);

    int date[3][3];
    // about sscanf: http://www.cnblogs.com/lyq105/archive/2009/11/28/1612677.html
    sscanf(start,
            "%d|%d|%d|%d|%f|%f|%f|%f|%c|%c|%d-%d-%d|%d-%d-%d|%d-%d-%d|%[^|]|%[^|]|%[^|]|\n",
            &record->orderkey,
            &record->partkey,
            &record->suppkey,

            &record->linenumber,

            &record->quantity,
            &record->extenedprice,
            &record->discount,
            &record->tax,

            &record->returnflag,
            &record->linestatus,

            date[0], date[0] + 1, date[0] + 2,
            date[1], date[1] + 1, date[1] + 2,
            date[2], date[2] + 1, date[2] + 2,

            record->shipinstruct,
            record->shipmode,
            record->comment);

    record->shipdate = int2date(date[0][0], date[0][1], date[0][2]);
    record->commitdate = int2date(date[1][0], date[1][1], date[1][2]);
    record->receiptdate = int2date(date[2][0], date[2][1], date[2][2]);

    char* p = start;
    while (*p && *p != '\n') ++p;
    // if *p is undefined, reach end of file

    if (DEBUG) {
        printf("order: %d, %d, %d, %d, comment: %s\n", record->orderkey, record->shipdate, record->commitdate, record->receiptdate, record->comment);
        printf("string2record: DONE.\n");
    }
    return (*p ? ++p : p);
}

void record2string(record_t* record, char* start) {
    int date[3][3];
    // convert dates to arrays of int
    date2int(record->shipdate, date[0]);
    date2int(record->commitdate, date[1]);
    date2int(record->receiptdate, date[2]);
    sprintf(start,
            "%d|%d|%d|%d|%.2f|%.2f|%.2f|%.2f|%c|%c|%d-%d-%d|%d-%d-%d|%d|%d|%d|%s|%s|%s|\n",
            record->orderkey,
            record->partkey,
            record->suppkey,

            record->linenumber,

            record->quantity,
            record->extenedprice,
            record->discount,
            record->tax,

            record->returnflag,
            record->linestatus,

            date[0][0], date[0][1], date[0][2],
            date[1][0], date[1][1], date[1][2],
            date[2][0], date[2][1], date[2][2],

            record->shipinstruct,
            record->shipmode,
            record->comment);
}

int int2date(int y, int m, int d) {
    return (y << 8) + (m << 4) + d;
}

void date2int(int date, int* dates) {
    dates[0] = date >> 8;
    dates[1] = date >> 4 & 0xf;
    dates[2] = date & 0xf;
}

void* read_int(void* start, int* i) {
    *i = ((int*)start)[0];
    return (int*)start + 1;
}

void* read_char(void* start, char* i) {
    *i = ((char*)start)[0];
    return (char*)start + 1;
}

void* read_float(void* start, float* i) {
    *i = ((float*)start)[0];
    return (float*)start + 1;
}

void* read_str(void* start, char* i) {
    int len = ((int*)start)[0];
    start = (int*)start + 1;
    strncpy(i, (char*)start, len);
    i[len] = '\0';
    return (char*)start + len;
}

int read_record_from_page(page_t* page, int offset, record_t* record) {
    void* begin = page->head + offset;
    //void* end = get_free_end(page);
    //assert(end - begin >= get_size_of_record(record));
    //if (end - begin < get_size_of_record(record)) return -1;

    begin = read_int(begin, &record->orderkey);
    begin = read_int(begin, &record->partkey);
    begin = read_int(begin, &record->suppkey);
    begin = read_int(begin, &record->linenumber);

    begin = read_float(begin, &record->quantity);
    begin = read_float(begin, &record->extenedprice);
    begin = read_float(begin, &record->discount);
    begin = read_float(begin, &record->tax);

    begin = read_char(begin, &record->returnflag);
    begin = read_char(begin, &record->linestatus);

    begin = read_int(begin, &record->shipdate);
    begin = read_int(begin, &record->commitdate);
    begin = read_int(begin, &record->receiptdate);
    
    begin = read_str(begin, record->shipinstruct);
    begin = read_str(begin, record->shipmode);
    begin = read_str(begin, record->comment);

    if (begin >= get_free_begin(page))
        return -1;

    offset = begin - page->head;
    return offset;
}

void* write_int(void* start, int* i) {
    ((int*)start)[0] = *i;
    return (int*)start + 1;
}

void* write_char(void* start, char* i) {
    ((char*)start)[0] = *i;
    return (char*)start + 1;
}

void* write_float(void* start, float* i) {
    ((float*)start)[0] = *i;
    return (float*)start + 1;
}

void* write_str(void* start, char* i) {
    int len = strlen(i);
    ((int*)start)[0] = len;
    start = (int*)start + 1;
    strncpy((char*)start, i, len);
    return (char*)start + len;
}

int write_record_into_page(record_t* record, int bucket_id) {
    if (DEBUG) printf("write_record_into_page: %d\n", record->orderkey);

    page_t* page = get_bucket_page(bucket_id);
    assert(page->type == BUCKET);

    page->dirty = 1;

    void* begin = get_free_begin(page);
    void* end = get_free_end(page);
    assert(end - begin >= get_size_of_record(record));

    int key = get_key(record);
    int hash_value = get_hash_value(key, global_depth);
    int bucket_id2 = get_bucket_id_by_hash_value(hash_value);
    assert(bucket_id2 == bucket_id);
    /** if (bucket_id2 != bucket_id) {
      *    write_record_into_page(record, bucket_id2);
      *    return 1;
      * }   */

    begin = write_int(begin, &record->orderkey);
    begin = write_int(begin, &record->partkey);
    begin = write_int(begin, &record->suppkey);
    begin = write_int(begin, &record->linenumber);

    begin = write_float(begin, &record->quantity);
    begin = write_float(begin, &record->extenedprice);
    begin = write_float(begin, &record->discount);
    begin = write_float(begin, &record->tax);

    begin = write_char(begin, &record->returnflag);
    begin = write_char(begin, &record->linestatus);

    begin = write_int(begin, &record->shipdate);
    begin = write_int(begin, &record->commitdate);
    begin = write_int(begin, &record->receiptdate);

    begin = write_str(begin, record->shipinstruct);
    begin = write_str(begin, record->shipmode);
    begin = write_str(begin, record->comment);

    int offset = begin - page->head;
    set_free_begin_offset(page, offset); // page.h
    // after inserting a record, increase the size
    int size = get_size(page);
    set_size(page, size + 1);

    //assert(offset <= PAGE_SIZE);

    return 1;
}

int get_key(record_t* record) {
    return record->orderkey;
}

int get_size_of_record(record_t* record) {
    return (int)(46 + strlen(record->shipinstruct)
        + strlen(record->shipmode) + strlen(record->comment)
        + 12); 
    // 46 = 8 * 4 + 2 + 3 * 4
    // 12 : because before each string, we use an int to store its length
}

int has_enough_space(record_t* record, int bucket_id) {
    page_t* page = get_bucket_page(bucket_id);
    void* begin = get_free_begin(page);
    void* end = get_free_end(page);
    return (end - begin >= get_size_of_record(record));
}

