#pragma GCC diagnostic ignored "-Wunused-result"
#include "../../include/file_func.h"
#include "hash_table_func.h"
#include "serialize_func.h"
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

    file_list_t *file_list = NULL;
    process_sync_file(&file_list, sync_file_path);

    transfer_header_t transfer_header;
    memset(&transfer_header, 0, sizeof(transfer_header_t));

    transfer_header.data_type = 3;
    transfer_header.total_size = total_file_size_cal(file_list);
    transfer_header.file_count = HASH_COUNT(file_list);

    unsigned char *serialized_data = NULL;
    file_serialized(&serialized_data, file_list, transfer_header);
    change_state(file_list, 0);

    int server_socket_fd = 0;
    server_socket_fd = socket_create(AF_INET, SOCK_STREAM, PROTOCOL);

    int opt = 1;
    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    server_address_set(&address, AF_INET, PORT);

    socket_bind(server_socket_fd, &address);
    socket_listen(server_socket_fd, 50);

    fd_set readfds;
    int client_socket[MAX_CLIENTS];
    memset(client_socket, 0, sizeof(client_socket));

    int addrlen = sizeof(address);
    int max_sd = 0;
    int client_close;

    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));

    unsigned char *update_data = NULL;

    while (TRUE)
    {
        printf("checking\n");

        select_init(server_socket_fd, client_socket, &readfds, &max_sd, &timeout);

        select(max_sd + 1, &readfds, NULL, NULL, &timeout);
        // 새로운 연결 요청 확인
        if (FD_ISSET(server_socket_fd, &readfds))
        {
            int new_socket = 0;
            new_socket = socket_accept(server_socket_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

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

        change_state(file_list, -1);

        if (1 == update_check_sync_file(&file_list, sync_file_path))
        {

            transfer_header_t update_header;
            memset(&update_header, 0, sizeof(transfer_header_t));

            update_header_set(file_list, &update_header);

            if (0 != update_header.total_size)
            {
                update_data_serialized(file_list, update_header, &update_data);
                thread_create(&update_data, &update_header, client_socket);

                if (NULL != update_data)
                {
                    free(update_data);
                    update_data = NULL;
                }
            }
            if (NULL != serialized_data)
            {
                free(serialized_data);
                serialized_data = NULL;
            }
            transfer_header.data_type = 3;
            transfer_header.total_size = total_file_size_cal(file_list);
            transfer_header.file_count = HASH_COUNT(file_list);
            file_serialized(&serialized_data, file_list, transfer_header);
        }

        change_state(file_list, 0);
    }
    if (NULL != serialized_data)
    {
        free(serialized_data);
        serialized_data = NULL;
    }
    clear_file_list(&file_list);
    return 0;
}
