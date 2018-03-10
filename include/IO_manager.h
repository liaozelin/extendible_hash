#ifndef IO_MANAGER_H_
#define IO_MANAGER_H_
#include"page.h"

/* init IO manager
 * it is the origin of the whole program:
 * create two initial bucket page and one initial index page
 *  */
void IO_manager_init(char* bucket_path, char* index_path);

/* create a new page, set 
 * @param type the type of new page
 * @param depth initial depth of new page
 * @return page id of new page
 *  */
int create_new_page(enum page_type type, int depth);

void load_page_from_disk(page_t* page);

void write_page_into_disk(page_t* page);

/* double index number
 *  */
void double_index();

/* split a page, in fact, the page must be bucket
 *  */
void split_page(int bucket_id, int hash_value);

void close_files();



#endif

