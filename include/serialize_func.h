#pragma once
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "file_func.h"

#define MAX_IPv4_LENGTH 16
#define COMPRESS_TYPE 10
#define COMPRESS_BOUNDARY 0
struct transfer_header
{
    int data_type;
    unsigned long total_size;
    int file_count;
};
typedef struct transfer_header transfer_header_t;

#ifdef __cplusplus

extern "C"
{
#endif
    void file_serialized(unsigned char **serialized_data, file_list_t *file_list, transfer_header_t transfer_header);
    int file_deserialized(unsigned char **serialized_data, int file_count, char *file_path);
#ifdef __cplusplus
}
#endif