#ifndef RECORD_H_
#define RECORD_H_
#include"page.h"

typedef struct __record {
    int orderkey;
    int partkey;
    int suppkey;
    int linenumber;
    float quantity;
    float extenedprice;
    float discount;
    float tax;
    char returnflag;
    char linestatus;
    int shipdate;
    int commitdate;
    int receiptdate;
    char shipinstruct[26];
    char shipmode[11];
    char comment[45];
} record_t;

// convert string to record
char* string2record(char* start, record_t* record);

// convert record to string
void record2string(record_t* record, char* start);

// convert int to date
int int2date(int y, int m, int d);

// convert date to int
void date2int(int date, int* dates);

// void* read_int(void* start, int* i);
// void* read_char(void* start, char* i);
// void* read_float(void* start, float* i);
// void* read_str(void* start, char* i);
/*  */
/* read a record from a page
 * the initial value of offset should be HEAD_LENGTH(see page.c)
 * @param page pointer to page
 * @param offset offset from page->head, pointer to start address of record
 * @param record store value into it
 *  */
int read_record_from_page(page_t* page, int offset, record_t* record);

/* void* write_int(void* start, int* i); */
// void* write_char(void* start, char* i);
// void* write_float(void* start, float* i);
// void* write_str(void* start, char* i);
/*  */
int write_record_into_page(record_t* record, int bucket_id);

int get_key(record_t* record);

int get_size_of_record(record_t* record);

/* test if a page has enough space to store the record
 * @param record target record to store
 * @param bucket_id the bucket page id
 *  */
int has_enough_space(record_t* record, int bucket_id);

#endif

