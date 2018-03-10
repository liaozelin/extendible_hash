#include<stdio.h>
#include<string.h>
#include"work.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: lzldb [path to lineitem]\n");
        return 0;
    }

    char path[100];
    strcpy(path, argv[1]);
    int len = strlen(argv[1]);
    if (argv[1][len - 1] != '/')
        strcat(path, "/");

    char bucket_path[100];
    char index_path[100];
    strcpy(bucket_path, path);
    strcat(bucket_path, "bucket.out");
    strcpy(index_path, path);
    strcat(index_path, "index.out");

    init(bucket_path, index_path);

    char in_path[100];
    char out_path[50];
    char query_file[100];

    strcpy(in_path, path);
    strcat(in_path, "lineitem.tbl");

    insert_all_items(in_path);

    strcpy(out_path, path);
    strcat(out_path, "testoutput.out");

    strcpy(query_file, path);
    strcat(query_file, "testinput.in");

    query_items(query_file, out_path);

    char index_output_path[100];
    strcpy(index_output_path, path);
    strcat(index_output_path, "hashindex.out");

    output_hash_index(index_output_path);

    over(); // work.h

    return 0;
}
