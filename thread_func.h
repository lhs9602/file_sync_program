#pragma once
#include "file_func.h"
#include "socket_func.h"
#include "serialize_func.h"
#include "compress_func.h"
#include <pthread.h>
#include <zlib.h>

struct thread_data
{
    int socket;
    unsigned char *serialized_data;
    unsigned long serialized_data_size;
};
typedef struct thread_data thread_data_t;

struct thread_server_data
{
    file_list_t *file_list;
    in_addr_t ip_addresses;
};
typedef struct thread_server_data thread_server_data_t;

typedef struct transfer_header transfer_header_t;

#ifdef __cplusplus
extern "C"
{
#endif
    void *file_send_thread(void *arg);
    void thread_create(unsigned char **update_data, transfer_header_t *update_header, int *client_socket);
    void master_server_thread(file_list_t *file_list, in_addr_t *ip_addresses, int ip_count);
    void *server_send_thread(void *arg);

#ifdef __cplusplus
}
#endif