/**
 * @file sync_server_func.c
 * @brief sync_server_func 관련 라이브러리
 *
 */
#include "serialize_func.h"
#include "hash_table_func.h"

/**
 * @brief update_header_set
 * update_header를 설정하는 함수
 *
 * @param file_list_t *file_list
 *
 * @param transfer_header_t update_header
 *
 * @param unsigned char **update_data
 *
 * @return void
 */
void update_header_set(file_list_t *file_list, transfer_header_t *update_header, int data_type)
{
    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    unsigned long total_size = 0;
    int file_count = 0;
    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        if (1 == data_type)
        {
            total_size += current_file_data->file_path_size + sizeof(unsigned long);
            total_size += SHA256_DIGEST_LENGTH;
            file_count += 1;
        }
        else if (2 == data_type)
        {
            if (1 == current_file_data->state)
            {
                total_size += current_file_data->file_path_size + sizeof(unsigned long);
                file_count += 1;
            }
        }
        else if (3 == data_type)
        {
            if (1 == current_file_data->state)
            {
                total_size += current_file_data->file_path_size + sizeof(unsigned long);
                total_size += current_file_data->file_data_size + sizeof(unsigned long);
                file_count += 1;
            }
        }
    }

    update_header->total_size = total_size;
    update_header->file_count = file_count;
    update_header->data_type = data_type;
}

/**
 * @brief update_data_serialized
 * 변경된 파일을 직렬화하는 함수
 *
 * @param file_list_t *file_list
 *
 * @param transfer_header_t update_header
 *
 * @param unsigned char **update_data
 *
 * @return void
 */
void update_data_serialized(file_list_t *file_list, transfer_header_t update_header, unsigned char **update_data)
{
    if (NULL == file_list)
    {
        printf("update_data_serialized 매개변수가 올바르지 않습니다.\n");
        return;
    }

    *update_data = (unsigned char *)malloc(sizeof(transfer_header_t) + update_header.total_size);
    unsigned char *update_data_ptr = *update_data;

    // 헤더 저장 및 포인터 이동
    memcpy(update_data_ptr, &update_header, sizeof(transfer_header_t));
    update_data_ptr += sizeof(transfer_header_t);

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        if (1 == current_file_data->state)
        {
            // 파일의 경로 길이 저장 및 포인터 이동
            memcpy(update_data_ptr, &current_file_data->file_path_size, sizeof(unsigned long));
            update_data_ptr += sizeof(unsigned long);

            // 파일 크기 저장 및 포인터 이동
            memcpy(update_data_ptr, &current_file_data->file_data_size, sizeof(unsigned long));
            update_data_ptr += sizeof(unsigned long);

            // 파일의 경로 저장 및 포인터 이동
            snprintf(update_data_ptr, current_file_data->file_path_size + 1, "%s", current_file_data->file_name);
            update_data_ptr += current_file_data->file_path_size;

            FILE *file = NULL;
            file = fopen(current_file_data->path, "rb");

            // 파일 데이터 저장 및 포인터 이동
            fread(update_data_ptr, 1, current_file_data->file_data_size, file);
            update_data_ptr += current_file_data->file_data_size;

            fclose(file);
        }
    }
}

/**
 * @brief file_deserialized
 * 파일을 역직렬화하는 함수
 *
 * @param unsigned char **serialized_data
 *
 * @param int file_count
 *
 * @param char *file_path
 *
 * @return void
 */
void file_deserialized(unsigned char **serialized_data, int file_count, char *file_path)
{
    if (NULL == serialized_data)
    {
        printf("file_deserialized 매개변수가 올바르지 않습니다.\n");
        return;
    }

    // 직렬화 데이터의 포인터 설정
    unsigned char *serialized_data_ptr = NULL;
    serialized_data_ptr = *serialized_data;

    unsigned long file_path_size = 0;
    unsigned long file_data_size = 0;

    long start_time = 0;
    long end_time = 0;
    double elapsed_time = 0;

    long start_sync_time = clock();

    // 직렬화 데이터 추출
    for (int path_index = 0; path_index < file_count; path_index++)
    {

        start_time = clock();

        // 파일 경로 길이 추출 및 포인터 이동
        file_path_size = 0;
        memcpy(&file_path_size, serialized_data_ptr, sizeof(file_path_size));
        serialized_data_ptr += sizeof(unsigned long);

        // 파일 크기 추출 및 포인터 이동
        file_data_size = 0;
        memcpy(&file_data_size, serialized_data_ptr, sizeof(file_data_size));
        serialized_data_ptr += sizeof(unsigned long);

        char path[MAX_LENGTH];
        memset(path, 0, sizeof(path));

        snprintf(path, MAX_LENGTH, "%s", serialized_data_ptr);
        serialized_data_ptr += file_path_size;

        char *file_name = NULL;
        // 마스터 서버와 슬레이브 서버의 통신인지, 서버(a)와 클라이언트(b)의 통신인지 구분
        if (NULL == file_path)
        {
            // 파일 저장 경로 생성 및 포인터 이동.
            dir_path_create(path, 1);
        }
        else
        {
            file_name = strrchr(path, '/');
            file_name++;
            snprintf(path, MAX_LENGTH, "%s%s", file_path, file_name);
            dir_path_create(path, 1);
        }
        FILE *file = NULL;
        file = fopen(path, "wb");
        if (NULL == file)
        {
            printf("%s 파일 생성 실패\n", path);
            serialized_data_ptr += file_data_size;
            continue;
        }

        //  파일 데이터 추출 및 포인터 이동
        fwrite(serialized_data_ptr, 1, file_data_size, file);
        serialized_data_ptr += file_data_size;

        end_time = clock();
        elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000.0;

        printf("파일명: %s\t파일크기: %ld\t전송 시간: %.3f 밀리초\n", file_name, file_data_size, elapsed_time);
        printf("-----------------------------------------------------------------\n");
        fclose(file);
    }

    end_time = clock();
    elapsed_time = ((double)(end_time - start_sync_time)) / CLOCKS_PER_SEC * 1000.0;
    printf("총 전송 시간 %.3f\n", elapsed_time);
}

/**
 * @brief file_serialized
 * 파일 데이터를 직렬화하는 함수
 *
 * @param unsigned char **serialized_data
 *
 * @param file_list_t *file_list
 *
 * @param transfer_header_t transfer_header
 *
 * @return unsigned long
 */
void file_serialized(unsigned char **serialized_data, file_list_t *file_list, transfer_header_t transfer_header)
{
    if (NULL == file_list)
    {
        printf("file_serialized 매개변수가 올바르지 않습니다.\n");
        return;
    }
    *serialized_data = (unsigned char *)malloc(sizeof(transfer_header_t) + transfer_header.total_size);
    unsigned char *serialized_data_ptr = *serialized_data;

    // 헤더 저장 및 포인터 이동
    memcpy(serialized_data_ptr, &transfer_header, sizeof(transfer_header_t));
    serialized_data_ptr += sizeof(transfer_header_t);

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;
    FILE *file = NULL;
    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        memcpy(serialized_data_ptr, &current_file_data->file_path_size, sizeof(unsigned long));
        serialized_data_ptr += sizeof(unsigned long);

        memcpy(serialized_data_ptr, &current_file_data->file_data_size, sizeof(unsigned long));
        serialized_data_ptr += sizeof(unsigned long);

        snprintf(serialized_data_ptr, current_file_data->file_path_size, "%s", current_file_data->path);
        serialized_data_ptr += current_file_data->file_path_size;
        printf("%s\n", current_file_data->path);
        file = fopen(current_file_data->path, "rb");
        if (NULL == file)
        {
            printf("%s 파일 열기 실패\n", current_file_data->path);
        }

        fread(serialized_data_ptr, 1, current_file_data->file_data_size, file);
        serialized_data_ptr += current_file_data->file_data_size;

        fclose(file);
    }
}
/**
 * @brief file_path_deserialized
 * 파일 경로를 역직렬화하는 함수
 *
 * @param file_info_t *file_list
 *
 * @param unsigned char **serialized_data
 *
 * @param int file_count
 *
 * @return void
 */
void file_path_deserialized(file_list_t *file_list, unsigned char **serialized_data, int file_count)
{
    if (NULL == *serialized_data && 0 == file_count)
    {
        printf("file_path_deserialized 매개변수가 올바르지 않습니다.\n");
        return;
    }
    // 직렬화 데이터의 포인터 설정
    unsigned char *serialized_data_ptr = NULL;
    serialized_data_ptr = *serialized_data;

    file_list_t *current_file_data = NULL;
    printf("---------------------------업데이트 대상----------------------\n");

    // 직렬화 데이터 추출
    for (int path_index = 0; path_index < file_count; path_index++)
    {
        // 파일 경로 길이 추출 및 포인터 이동
        unsigned long file_path_size = 0;
        memcpy(&file_path_size, serialized_data_ptr, sizeof(file_path_size));
        serialized_data_ptr += sizeof(unsigned long);

        char path[MAX_LENGTH];
        memset(path, 0, sizeof(path));

        // 파일 경로 추출 및 포인터 이동
        snprintf(path, file_path_size, "%s", serialized_data_ptr);
        serialized_data_ptr += file_path_size;

        current_file_data = find_file_data(file_list, path);
        if (NULL == current_file_data)
        {
            continue;
        }
        else
        {
            printf("업데이트: %s\n", current_file_data->path);
            current_file_data->state = 1;
        }
    }
}

/**
 * @brief file_path_serialized
 * 파일 경로를 직렬화하는 함수
 *
 * @param unsigned char **serialized_data
 *
 * @param transfer_header_t *transfer_header
 *
 * @param file_list_t *file_list
 *
 * @return void
 */
void file_path_serialized(unsigned char **serialized_data, transfer_header_t *transfer_header, file_list_t *file_list)
{
    if (NULL == file_list)
    {
        printf("file_list_serialized 매개변수가 올바르지 않습니다.\n");
        return;
    }

    // 직렬화 데이터 동적 할당
    *serialized_data = (unsigned char *)malloc(sizeof(transfer_header_t) + transfer_header->total_size);

    // 직렬화 데이터의 포인터 설정
    unsigned char *serialized_data_ptr = *serialized_data;

    // 헤더 저장 및 포인터 이동
    memcpy(serialized_data_ptr, transfer_header, sizeof(transfer_header_t));
    serialized_data_ptr += sizeof(transfer_header_t);

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        if (1 == current_file_data->state)
        {
            memcpy(serialized_data_ptr, &current_file_data->file_path_size, sizeof(unsigned long));
            serialized_data_ptr += sizeof(unsigned long);

            snprintf(serialized_data_ptr, current_file_data->file_path_size, "%s", current_file_data->path);
            serialized_data_ptr += current_file_data->file_path_size;
        }
    }
}

/**
 * @brief file_list_deserialized
 * 파일 리스트를 역직렬화하는 함수
 *
 * @param unsigned char **serialized_data
 *
 * @param file_list_t *file_list
 *
 * @param int file_count
 *
 * @return void
 */
void file_list_deserialized(unsigned char **serialized_data, file_list_t *file_list, int file_count, char *sync_file_path)
{
    if (NULL == serialized_data || NULL == file_list)
    {
        printf("file_list_deserialized 매개변수가 올바르지 않습니다.\n");
        return;
    }
    // 직렬화 데이터의 포인터 설정
    unsigned char *serialized_data_ptr = NULL;
    serialized_data_ptr = *serialized_data;

    file_list_t *current_file_data;

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    // FILE *file = NULL;
    // file = fopen(sync_file_path, "w");
    // 직렬화 데이터 추출
    for (int path_index = 0; path_index < file_count; path_index++)
    {
        // 파일 경로 길이 추출 및 포인터 이동
        unsigned long file_path_size = 0;
        memcpy(&file_path_size, serialized_data_ptr, sizeof(file_path_size));
        serialized_data_ptr += sizeof(file_path_size);

        char path[MAX_LENGTH];
        memset(path, 0, sizeof(path));

        // 파일 경로 추출 및 포인터 이동
        snprintf(path, file_path_size, "%s", serialized_data_ptr);
        serialized_data_ptr += file_path_size;

        // fprintf(file, "%s\n", path);

        unsigned char check_sum[SHA256_DIGEST_LENGTH];
        memset(check_sum, 0, sizeof(check_sum));

        // 파일 체크썸 추출 및 포인터 이동
        memcpy(check_sum, serialized_data_ptr, SHA256_DIGEST_LENGTH);
        serialized_data_ptr += SHA256_DIGEST_LENGTH;

        current_file_data = NULL;
        current_file_data = find_file_data(file_list, path);
        if (NULL == current_file_data)
        {
            add_path(&file_list, path);
        }
        else if (0 != strncmp(check_sum, current_file_data->check_sum, SHA256_DIGEST_LENGTH))
        {
            check_sum_generater(path, current_file_data->check_sum, &sha256);
            current_file_data->state = 1;
        }
    }
}

/**
 * @brief file_list_serialized
 * 파일 리스트를 직렬화하는 함수
 *
 * @param unsigned char **serialized_data
 *
 * @param transfer_header_t *transfer_header
 *
 * @param file_list_t *file_list
 *
 * @return void
 */
void file_list_serialized(unsigned char **serialized_data, transfer_header_t *transfer_header, file_list_t *file_list)
{
    if (NULL == file_list)
    {
        printf("file_list_serialized 매개변수가 올바르지 않습니다.\n");
        return;
    }

    // 직렬화 데이터 동적 할당
    *serialized_data = (unsigned char *)malloc(sizeof(transfer_header_t) + transfer_header->total_size);

    // 직렬화 데이터의 포인터 설정
    unsigned char *serialized_data_ptr = *serialized_data;

    // 헤더 저장 및 포인터 이동
    memcpy(serialized_data_ptr, transfer_header, sizeof(transfer_header_t));
    serialized_data_ptr += sizeof(transfer_header_t);

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        memcpy(serialized_data_ptr, &current_file_data->file_path_size, sizeof(unsigned long));
        serialized_data_ptr += sizeof(unsigned long);

        snprintf(serialized_data_ptr, current_file_data->file_path_size, "%s", current_file_data->path);
        serialized_data_ptr += current_file_data->file_path_size;

        memcpy(serialized_data_ptr, current_file_data->check_sum, SHA256_DIGEST_LENGTH);
        serialized_data_ptr += SHA256_DIGEST_LENGTH;
    }
}

/**
 * @brief slave_server_action
 * 슬레이브 서버의 동작 함수
 *
 * @param int master_server_socket
 *
 * @param file_info_t *file_list
 *
 * @param char *sync_file_path
 *
 * @return void
 */
void slave_server_action(int master_server_socket, file_list_t *file_list, char *sync_file_path)
{
    if (NULL == file_list || master_server_socket <= 0)
    {
        printf("slave_server_action 매개변수가 올바르지 않습니다.\n");
        return;
    }
    unsigned char *serialized_data = NULL;

    // 마스터 서버와의 연결이 없으면 종료. 기존의 변경사항 체크로 넘어감
    while (TRUE)
    {
        //***********************************************************************파일 리스트 역직렬화 수신***********************************************************************

        transfer_header_t transfer_header;
        memset(&transfer_header, 0, sizeof(transfer_header_t));

        // 파일 리스트의 헤더 수신
        long received_bytes = 0;
        received_bytes = recv(master_server_socket, &transfer_header, sizeof(transfer_header_t), 0);
        if (0 == received_bytes)
        {
            // 상대방이 연결을 종료한 경우 처리
            printf("상대방이 연결을 종료했습니다.\n");
            break;
        }

        if (NULL != serialized_data)
        {
            free(serialized_data);
            serialized_data = NULL;
        }
        serialized_data = (unsigned char *)malloc(transfer_header.total_size);

        received_bytes = recv(master_server_socket, serialized_data, transfer_header.total_size, 0);

        if (0 == received_bytes)
        {
            // 상대방이 연결을 종료한 경우 처리
            printf("상대방이 연결을 종료했습니다.\n");
            break;
        }

        if (transfer_header.data_type > COMPRESS_TYPE)
        {
            serialized_data_decompress(&serialized_data, &transfer_header);
            transfer_header.data_type -= COMPRESS_TYPE;
        }

        if (transfer_header.data_type == 1)
        {
            file_list_deserialized(&serialized_data, file_list, transfer_header.file_count, sync_file_path);
            if (NULL != serialized_data)
            {
                free(serialized_data);
                serialized_data = NULL;
            }
            update_header_set(file_list, &transfer_header, 2);

            // 전송할 파일 경로가 없으면 종료
            if (0 == transfer_header.file_count)
            {
                break;
            }

            file_path_serialized(&serialized_data, &transfer_header, file_list);

            // 직렬화 데이터 압축화
            if (transfer_header.total_size > COMPRESS_BOUNDARY)
            {
                transfer_header.total_size = serialized_data_compress(&serialized_data, &transfer_header, transfer_header.total_size);
            }

            send(master_server_socket, serialized_data, sizeof(transfer_header_t) + transfer_header.total_size, 0);
        }
        //**********************파일 역직렬화 수신**********************
        else if (transfer_header.data_type == 1)
        {

            file_deserialized(&serialized_data, -1, NULL);
            break;
        }
    }

    if (NULL != serialized_data)
    {
        free(serialized_data);
        serialized_data = NULL;
    }
    close(master_server_socket);
}