#ifndef PAGE_H_
#define PAGE_H_

#define PAGE_SIZE 8192
#define BUFFER_OFFSET 20

//#define ID_POS 0
#define DEPTH_POS 0
#define SIZE_POS 1
#define FREE_BEGIN_OFFSET 2
#define HEAD_LENGTH 16

// how many (hash_key, bucker_id) tuple can a page store
// size of a buffer page is 8192 and a record is 4
#define index_pair_per_page (2048)

enum page_type {INDEX, BUCKET};

// if type is INDEX, depth is global depth,
// otherwise, depth is local depth
// head is pointer to data storing in page
// once data in page is modified, set dirty = 1
typedef struct __page {
    enum page_type type;
    int id;
    int depth;
    int dirty;
    void* head;
} page_t;

void init_bucket_page(page_t* page, int depth);

//void init_index_page(page_t* page);

/* add a record into a pge
 * @param page target page
 * @param record the record to insert
 *  */
//bool add_record(page_t* page, record_t* record);

//int get_page_id(page_t* page);


int get_depth(page_t* page);

void set_depth(page_t* page, int depth);

int get_size(page_t* page);

void set_size(page_t* page, int size);

void* get_free_begin(page_t* page);

void set_free_begin_offset(page_t* page, int offset);

void* get_free_end(page_t* page);

/* copy a page from src to dest
 * @param src source page
 * @param destination page
 *  */
void copy_page(page_t* src, page_t* dest);

/***********************index page*******************************/
/* set_bucket_id_by_hash_value
 * @param hash_value
 * @param bucket_id the bucket id to store
 *  */
void set_bucket_id_by_hash_value(int hash_value, int bucket_id);

/* get_bucket_id_by_hash_value
 * @param hash_value 
 * @return bucket id with hash value hash_value
 *  */
int get_bucket_id_by_hash_value(int hash_value);

#endif

