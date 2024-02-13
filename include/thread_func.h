#pragma once
#include "file_func.h"
#include "socket_func.h"
#include "serialize_func.h"
#include <pthread.h>
#include <zlib.h>
struct thread_data
{
    int thread_id;
    int socket;
    unsigned char *serialized_data;
    unsigned long serialized_data_size;
};
typedef struct thread_data thread_data_t;

// 조건 변수와 뮤텍스
extern pthread_mutex_t mutex;

#ifdef __cplusplus
extern "C"
{
#endif
    void *file_send_thread(void *arg);
    void thread_create(unsigned char **update_data, transfer_header_t *update_header, int *client_socket);
#ifdef __cplusplus
}
#endif