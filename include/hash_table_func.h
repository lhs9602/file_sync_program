#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include "file_func.h"
#include "uthash.h"

struct file_list
{
    char *path;
    unsigned long file_path_size;
    unsigned long file_data_size;
    unsigned char check_sum[SHA256_DIGEST_LENGTH];
    long update_time;
    int state;
    UT_hash_handle hh;
};

typedef struct file_list file_list_t;

#ifdef __cplusplus
extern "C"
{
#endif
    void print_all_files();
    file_list_t *find_file_data(char *path);
    void add_path(char *path);
    void delete_file_data(char *path);
    void clear_file_list();

#ifdef __cplusplus
}
#endif