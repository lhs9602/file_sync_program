#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <regex.h>
#include <time.h>
#include <openssl/sha.h>
#include <limits.h>
#include <regex.h>
#include "hash_table_func.h"
#include "uthash.h"
#define TRUE 1
#define MAX_LENGTH 4096
#define MAX_DATA_SIZE 1000

#ifdef __cplusplus
extern "C"
{
#endif
    int file_path_check(char *path);
    int flie_name_remover(char *path);
    char *absolute_path_change(char *path);
    int relative_path_check(char *path);
    int check_sum_generater(char *path, unsigned char *check_sum, SHA256_CTX *sha256);
    void process_sync_file(char *sync_file_path);

#ifdef __cplusplus
}
#endif