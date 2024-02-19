#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "file_func.h"
#include "hash_table_func.h"

struct transfer_header
{
    int data_type;
    unsigned long total_size;
    int file_count;
};
typedef struct transfer_header transfer_header_t;
typedef struct file_list file_list_t;

#ifdef __cplusplus

extern "C"
{
#endif
    void update_header_set(file_list_t *file_list, transfer_header_t *update_header, int data_type);
    void update_data_serialized(file_list_t *file_list, transfer_header_t update_header, unsigned char **update_data);
    void file_serialized(unsigned char **serialized_data, file_list_t *file_list, transfer_header_t transfer_header);
    void file_deserialized(unsigned char **serialized_data, int file_count, char *file_path);
    void file_list_serialized(unsigned char **serialized_data, transfer_header_t *transfer_header, file_list_t *file_list);
    void file_list_deserialized(unsigned char **serialized_data, file_list_t *file_list, int file_count, char *sync_file_path);
    void file_path_serialized(unsigned char **serialized_data, transfer_header_t *transfer_header, file_list_t *file_list);
    void file_path_deserialized(file_list_t *file_list, unsigned char **serialized_data, int file_count);
    void slave_server_action(int master_server_socket, file_list_t *file_list, char *sync_file_path);

#ifdef __cplusplus
}
#endif