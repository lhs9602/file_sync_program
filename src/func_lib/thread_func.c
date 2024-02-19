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

    int socket = 0;
    socket = data->socket;

    unsigned char *serialized_data = NULL;
    serialized_data = data->serialized_data;

    unsigned long serialized_data_size = 0;
    serialized_data_size = data->serialized_data_size;

    send(socket, serialized_data, sizeof(transfer_header_t) + serialized_data_size, 0);
    pthread_detach(pthread_self());
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
            thread_data[index].serialized_data = *update_data;
            thread_data[index].socket = client_socket[index];
            thread_data[index].serialized_data_size = update_header->total_size;

            // 스레드 생성 및 실행할 함수와 인수 전달
            pthread_create(&threads[index], NULL, file_send_thread, &thread_data[index]);
        }
    }

    // 생성된 스레드가 종료될 때까지 대기
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
/**
 * @brief master_server_thread
 * 마스터 서버 스레드를 생성하는 함수
 *
 * @param file_list_t *file_list,
 *
 * @param  in_addr_t *ip_addresses
 *
 * @param  int ip_count
 *
 * @return void
 */
void master_server_thread(file_list_t *file_list, in_addr_t *ip_addresses, int ip_count)
{

    if (NULL == file_list || NULL == ip_addresses)
    {
        printf("master_server_thread_create 매개변수 오류\n");
        return;
    }
    pthread_t server_threads[ip_count];
    memset(server_threads, 0, sizeof(server_threads));

    thread_server_data_t thread_server_data[ip_count];
    memset(thread_server_data, 0, sizeof(thread_server_data));

    for (int thread_index = 0; thread_index < ip_count; thread_index++)
    {
        thread_server_data[thread_index].file_list = file_list;
        thread_server_data[thread_index].ip_addresses = ip_addresses[thread_index];
        printf("스레드 %d\t생성:%s\n", thread_index, inet_ntoa(*(struct in_addr *)&ip_addresses[thread_index]));
        pthread_create(&server_threads[thread_index], NULL, server_send_thread, &thread_server_data[thread_index]);
    }
    for (int thread_index = 0; thread_index < ip_count; thread_index++)
    {
        pthread_join(server_threads[thread_index], NULL);
        printf("스레드 %d가 종료되었습니다.\n", thread_index);
    }
    printf("\n");
    printf("모든 스레드가 종료되었습니다.\n");
}

/**
 * @brief server_send_thread
 * 마스터 서버에서 슬레이브 서버로 직렬화 데이터를 전송,수신하는 스레드
 *
 * @param void *arg
 *
 * @return void
 */
void *server_send_thread(void *arg)
{
    thread_server_data_t *data;
    memset(&data, 0, sizeof(data));
    data = (thread_server_data_t *)arg;

    file_list_t *file_list = NULL;
    file_list = data->file_list;

    in_addr_t ip_addresses = 0;
    ip_addresses = data->ip_addresses;

    // 소켓 생성 및 슬레이브 서버와 연결
    int socket_fd = 0;
    socket_fd = master_server_connect(ip_addresses);

    unsigned char *serialized_data = NULL;
    long received_bytes = 0;
    if (0 != socket_fd)
    {

        // 파일 리스트 직렬화 전송

        // 기본 전송 헤더 설정
        transfer_header_t transfer_header;
        memset(&transfer_header, 0, sizeof(transfer_header_t));

        // 파일 리스트 직렬화

        update_header_set(file_list, &transfer_header, 1);
        file_list_serialized(&serialized_data, &transfer_header, file_list);

        // 직렬화 데이터 압축화
        if (transfer_header.total_size > COMPRESS_BOUNDARY)
        {
            transfer_header.total_size = serialized_data_compress(&serialized_data, &transfer_header, transfer_header.total_size);
        }

        // 파일 리스트 직렬화 데이터 전송
        send(socket_fd, serialized_data, sizeof(transfer_header_t) + transfer_header.total_size, 0);

        // 사용한 직렬화 데이터 해제
        if (NULL != serialized_data)
        {
            free(serialized_data);
            serialized_data = NULL;
        }

        // 업데이트 역직렬화 수신

        memset(&transfer_header, 0, sizeof(transfer_header_t));

        // 업데이트 리스트의 헤더 수신
        received_bytes = recv(socket_fd, &transfer_header, sizeof(transfer_header_t), 0);
        if (0 == received_bytes)
        {
            // 상대방이 연결을 종료한 경우 처리
            printf("상대방이 연결을 종료했습니다.\n");

            close(socket_fd);
            pthread_detach(pthread_self());
            pthread_exit(NULL);
        }
        // 수신 받을 직렬화 데이터 할당
        serialized_data = (unsigned char *)malloc(transfer_header.total_size);

        // 업데이트 리스트의 직렬화 데이터 수신
        received_bytes = recv(socket_fd, serialized_data, transfer_header.total_size, 0);
        if (0 == received_bytes)
        {
            // 상대방이 연결을 종료한 경우 처리
            printf("상대방이 연결을 종료했습니다.\n");
            if (NULL != serialized_data)
            {
                free(serialized_data);
                serialized_data = NULL;
            }
            close(socket_fd);
            pthread_detach(pthread_self());
            pthread_exit(NULL);
        }

        // 압축 데이터의 경우 압축 해제
        if (transfer_header.data_type > COMPRESS_TYPE)
        {
            serialized_data_decompress(&serialized_data, &transfer_header);
        }
        // 업데이트 리스트의 직렬화 데이터 역작렬화
        file_path_deserialized(file_list, &serialized_data, transfer_header.file_count);
        if (NULL != serialized_data)
        {
            free(serialized_data);
            serialized_data = NULL;
        }

        // 파일 직렬화 전송
        //  업데이트 리스트가 존재할 경우에만 실행
        if (transfer_header.file_count > 0)
        {
            update_header_set(file_list, &transfer_header, 3);
            file_serialized(&serialized_data, file_list, transfer_header);

            // 직렬화 데이터 압축화
            if (transfer_header.total_size > COMPRESS_BOUNDARY)
            {
                transfer_header.total_size = serialized_data_compress(&serialized_data, &transfer_header, transfer_header.total_size);
            }
            send(socket_fd, serialized_data, sizeof(transfer_header_t) + transfer_header.total_size, 0);
            if (NULL != serialized_data)
            {
                free(serialized_data);
                serialized_data = NULL;
            }
        }
    }

    // 연결 종료
    close(socket_fd);
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}