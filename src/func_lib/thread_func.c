/**
 * @file thread_func.c
 * @brief 스레드 관련 라이브러리
 *
 */

#include "thread_func.h"

/**
 * @brief file_send_thread
 * 파일들을 전송하는 스레드 함수
 *
 * @param void *arg
 *
 * @return void
 */
void *file_send_thread(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;

    int thread_id = 0;
    thread_id = data->thread_id;

    int socket = 0;
    socket = data->socket;

    unsigned char *serialized_data = NULL;
    serialized_data = data->serialized_data;

    unsigned long serialized_data_size = 0;
    serialized_data_size = data->serialized_data_size;

    send(socket, serialized_data, sizeof(transfer_header_t) + serialized_data_size, 0);

    // 작업이 완료되면 bitmask에서 자신의 비트 0으로 설정
    pthread_exit(NULL);
}

/**
 * @brief thread_create
 * 멀티 스레드를 생성하는 함수
 *
 * @param unsigned char **update_data
 *
 * @param transfer_header_t *update_header
 *
 * @param int *client_socket
 *
 * @return void
 */
void thread_create(unsigned char **update_data, transfer_header_t *update_header, int *client_socket)
{
    if (NULL == update_data)
    {
        printf("thread_create 매개변수 오류\n");
        return;
    }

    // 스레드 생성
    pthread_t threads[MAX_CLIENTS];
    memset(threads, 0, sizeof(threads));

    thread_data_t thread_data[MAX_CLIENTS];
    memset(thread_data, 0, sizeof(thread_data));

    for (int index = 0; index < MAX_CLIENTS; index++)
    {
        if (0 == client_socket[index])
        {
            continue;
        }
        else
        {
            // 각 스레드 구성
            thread_data[index].thread_id = index;
            thread_data[index].serialized_data = *update_data;
            thread_data[index].socket = client_socket[index];
            thread_data[index].serialized_data_size = update_header->total_size;

            pthread_create(&threads[index], NULL, file_send_thread, &thread_data[index]);
        }
    }

    for (int thread_index = 0; thread_index < MAX_CLIENTS; thread_index++)
    {
        pthread_join(threads[thread_index], NULL);
    }

    if (NULL != *update_data)
    {
        free(*update_data);
        *update_data = NULL;
    }
}
