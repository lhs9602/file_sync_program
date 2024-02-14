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
    char *file_name;
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
    void print_all_files(file_list_t **file_list);
    file_list_t *find_file_data(file_list_t *file_list, char *path);
    void add_path(file_list_t **file_list, char *path);
    void delete_file_data(file_list_t **file_list, char *path);
    void delete_state_clear(file_list_t **file_list);
    void clear_file_list(file_list_t **file_list);
    unsigned long total_file_size_cal(file_list_t *file_list);
    int check_path(file_list_t *current_file_data, char *path);
    void change_state(file_list_t *file_list, int state);
#ifdef __cplusplus
}
#endif