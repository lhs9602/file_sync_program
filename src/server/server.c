#pragma GCC diagnostic ignored "-Wunused-result"
#include "../../include/file_func.h"
#include "hash_table_func.h"
#include "serialize_func.h"
#include "compress_func.h"
#include "../../include/socket_func.h"
#include <sys/select.h>
#include "thread_func.h"

char base_path[MAX_LENGTH];

int main(int argc, char *argv[])
{
    char sync_file_path[MAX_LENGTH];
    memset(sync_file_path, 0, sizeof(sync_file_path));

    // 인자 미입력시 입력
    if (0 == file_path_check(argv[1]))
    {
        do
        {
            printf("파일 리스트를 입력하시오:");
            scanf("%s", sync_file_path);
        } while (0 == file_path_check(sync_file_path));
    }
    else
    {
        snprintf(sync_file_path, MAX_LENGTH, "%s", argv[1]);
    }

    memset(base_path, 0, sizeof(base_path));
    snprintf(base_path, MAX_LENGTH, "%s", sync_file_path);
    flie_name_remover(base_path);

    char sync_server_path[MAX_LENGTH];
    memset(sync_server_path, 0, sizeof(sync_server_path));

    if (1 == file_path_check(argv[2]))
    {
        strncpy(sync_server_path, argv[2], sizeof(sync_file_path));
    }

    // sync_server_path의 길이로 메인과 슬레이브 서버 구분
    int sync_server_path_len = 0;
    sync_server_path_len = strlen(sync_server_path);
    printf("sync_server_path_len = %d\n", sync_server_path_len);

    file_list_t *file_list = NULL;
    process_sync_file(&file_list, sync_file_path);

    transfer_header_t transfer_header;
    memset(&transfer_header, 0, sizeof(transfer_header_t));

    transfer_header.data_type = 3;
    transfer_header.total_size = total_file_size_cal(file_list);
    transfer_header.file_count = HASH_COUNT(file_list);

    // 초기 데이터 할당
    unsigned char *serialized_data = NULL;
    file_serialized(&serialized_data, file_list, transfer_header);

    if (transfer_header.total_size > COMPRESS_BOUNDARY)
    {
        transfer_header.total_size = serialized_data_compress(&serialized_data, &transfer_header, transfer_header.total_size);
    }

    change_state(file_list, 0);

    int server_socket_fd = 0;
    server_socket_fd = socket_create(AF_INET, SOCK_STREAM, PROTOCOL);

    int opt = 1;
    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_socket_address;
    server_address_set(&server_socket_address, AF_INET, PORT);

    socket_bind(server_socket_fd, &server_socket_address);
    socket_listen(server_socket_fd, 50);

    fd_set readfds;

    int client_socket[MAX_CLIENTS];
    memset(client_socket, 0, sizeof(client_socket));

    int server_socket_addrlen = sizeof(server_socket_address);
    int max_sd = 0;
    int client_close;

    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));

    unsigned char *update_data = NULL;

    int slave_server_socket = 0;

    // 슬레이브 서버 로직
    if (0 == sync_server_path_len)
    {
        slave_server_socket = socket_create(AF_INET, SOCK_STREAM, PROTOCOL);
        setsockopt(slave_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in slave_server_address;
        server_address_set(&slave_server_address, AF_INET, SERVER_PORT);

        socket_bind(slave_server_socket, &slave_server_address);
        socket_listen(slave_server_socket, 5);
    }
    while (TRUE)
    {

        printf("checking\n");

        select_init(server_socket_fd, slave_server_socket, client_socket, &readfds, &max_sd, &timeout);

        select(max_sd + 1, &readfds, NULL, NULL, &timeout);
        // 새로운 연결 요청 확인
        if (FD_ISSET(server_socket_fd, &readfds))
        {
            int new_socket = 0;
            new_socket = socket_accept(server_socket_fd, (struct sockaddr *)&server_socket_address, (socklen_t *)&server_socket_addrlen);

            // 새 소켓을 클라이언트 소켓 배열에 추가
            client_add(new_socket, client_socket);

            send(new_socket, serialized_data, sizeof(transfer_header_t) + transfer_header.total_size, 0);
            continue;
        }

        client_close = 0;
        for (int index = 0; index < MAX_CLIENTS; index++)
        {
            if (0 == client_socket[index])
            {
                continue;
            }
            if (FD_ISSET(client_socket[index], &readfds))
            {
                client_close = 1;
                client_socket[index] = client_connect_check(client_socket[index]);
            }
        }

        if (1 == client_close)
        {
            continue;
        }

        // 슬레이브 서버 로직
        if (0 == sync_server_path_len)
        {
            printf(" 슬레이브 서버 로직\n");

            if (FD_ISSET(slave_server_socket, &readfds))
            {
                int master_server_socket = 0;
                master_server_socket = socket_accept(slave_server_socket, (struct sockaddr *)&server_socket_address, (socklen_t *)&server_socket_addrlen);
                slave_server_action(master_server_socket, file_list, sync_file_path);
            }
        }

        // 기존의 상태를 -1로 변경
        change_state(file_list, -1);

        // 변경 사항 확인
        if (1 == update_check_sync_file(&file_list, sync_file_path))
        {
            transfer_header_t update_header;
            memset(&update_header, 0, sizeof(transfer_header_t));
            update_header_set(file_list, &update_header, 3);

            if (0 != update_header.total_size)
            {
                printf("변경 데이터 전송\n");
                update_data_serialized(file_list, update_header, &update_data);
                if (update_header.total_size > COMPRESS_BOUNDARY)
                {
                    update_header.total_size = serialized_data_compress(&update_data, &update_header, update_header.total_size);
                }
                thread_create(&update_data, &update_header, client_socket);
            }

            // 기존 초기 데이터 할당 해제
            if (NULL != serialized_data)
            {
                free(serialized_data);
                serialized_data = NULL;
            }

            // 초기 전송 데이터에 변경사항을 반영하여 다시 생성
            transfer_header.data_type = 3;
            transfer_header.total_size = total_file_size_cal(file_list);
            transfer_header.file_count = HASH_COUNT(file_list);

            file_serialized(&serialized_data, file_list, transfer_header);

            if (transfer_header.total_size > COMPRESS_BOUNDARY)
            {
                transfer_header.total_size = serialized_data_compress(&serialized_data, &transfer_header, transfer_header.total_size);
            }
        }
        // 상태를 0으로 초기화
        change_state(file_list, 0);

        //  마스터 서버 로직
        if (0 != sync_server_path_len)
        {
            master_server_action(file_list, sync_server_path);
        }
    }

    if (NULL != serialized_data)
    {
        free(serialized_data);
        serialized_data = NULL;
    }
    clear_file_list(&file_list);
    close(slave_server_socket);
    close(server_socket_fd);
    return 0;
}
