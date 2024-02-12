#include "../../include/file_func.h"
#include "hash_table_func.h"
#include "../../include/socket_func.h"
#include "serialize_func.h"

#define DESTINATION_PATH "../../sync_dir/"

int main(int argc, char *argv[])
{
    dir_path_create(DESTINATION_PATH, 0);
    int client_soket_fd = client_action();

    while (TRUE)
    {
        static char file_path[MAX_LENGTH];
        memset(file_path, 0, sizeof(file_path));

        transfer_header_t transfer_header;
        memset(&transfer_header, 0, sizeof(transfer_header_t));

        unsigned char *serialized_data = NULL;
        long received_bytes = 0;
        received_bytes = recv(client_soket_fd, &transfer_header, sizeof(transfer_header_t), 0);
        if (0 == received_bytes)
        {
            printf("상대편 프로세스가 연결을 종료했습니다.\n");
            break;
        }

        // 수신 받을 직렬화 데이터 할당
        serialized_data = (unsigned char *)malloc(transfer_header.total_size);

        received_bytes = recv(client_soket_fd, serialized_data, transfer_header.total_size, 0);
        if (0 == received_bytes)
        {
            printf("상대편 프로세스가 연결을 종료했습니다.\n");
            if (NULL != serialized_data)
            {
                free(serialized_data);
                serialized_data = NULL;
            }
            break;
        }

        // 파일 역직렬화
        file_deserialized(&serialized_data, transfer_header.file_count, DESTINATION_PATH);

        if (NULL != serialized_data)
        {
            free(serialized_data);
            serialized_data = NULL;
        }
    }

    close(client_soket_fd);
    return 0;
}