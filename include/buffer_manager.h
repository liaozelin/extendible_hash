#ifndef BUFFER_MANAGER_H_
#define BUFFER_MANAGER_H_
#include"page.h"
/* 
 * 缓冲区管理
 * 缓冲区大小为P(有P个缓冲页),P的值是setting.h中定义的BUFFER_SIZE
 * 数据页和索引页在使用之前必须先从磁盘调入缓冲区(内存),
 * 此模块作用就是: 在需要时,返回一个用于保存新页面的缓冲区页
 * (至于页面内容的填充与操作,由其他模块完成)
 * 使用*时钟页面算法*进行页面置换
 *
 *  */

// how many (hash_key, bucker_id) tuple can a page store
// size of a buffer page is 8192 and a record is 4
#define index_pair_per_page (2048)

struct buffer_node_t {
    page_t* page;
    int R;
};


/************************* api ********************************/
/* init the buffer, before using
 *  */
void buffer_init();

/* get a page whose type is index, storing record data
 * @param buffer_id page id of a bucket page
 * @return pointer to a page to use
 *  */
page_t* get_bucket_page(int buffer_id);

/* get a record page by hash value
 * @param hash value to use
 * @return pointer to a page to use
 *  */
page_t* get_bucket_page_by_hash_value(int hash_value);

/* get a page whose type is index, storing the hash index
 * @param index_id page id of an index page
 * @return pointer to a page to use
 *  */
page_t* get_index_page(int index_id);


/************************** others *******************************/
/* find an avaliable position of buffer to store a page
 * If necessarily, swaping the page
 *  */
int find_avaliable_page_pos();

/* find an avaliable page of buffer
 * @return pointer to an avaliable page
 *  */
page_t* find_avaliable_page();

/* find a page in buffer according to page_id
 * if not found, load the page into buffer
 * @param page_id
 * @param type
 *  */
int find_page(int page_id, enum page_type type);

/* load a page from buffer
 * @param page_id id the page to use
 * @param type type of the page
 * @return pointer to a page to use
 *  */
page_t* load_page(int page_id, enum page_type type);

/* write a page into disk
 * @param pointer to the page to write out
 *  */
void write_page(page_t* page);

/* write data in buffer into disk, before closing buffer manager program
 * */
void save_pages();

#endif

