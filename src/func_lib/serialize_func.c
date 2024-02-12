/**
 * @file sync_server_func.c
 * @brief sync_server_func 관련 라이브러리
 *
 */
#include "serialize_func.h"
#include "hash_table_func.h"

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
 * @return int
 */
int file_deserialized(unsigned char **serialized_data, int file_count, char *file_path)
{
    if (NULL == serialized_data)
    {
        printf("file_deserialized 매개변수가 올바르지 않습니다.\n");
        return 0;
    }

    // 직렬화 데이터의 포인터 설정
    unsigned char *serialized_data_ptr = NULL;
    serialized_data_ptr = *serialized_data;

    unsigned long file_path_size = 0;
    unsigned long file_data_size = 0;

    // 직렬화 데이터 추출
    for (int path_index = 0; path_index < file_count; path_index++)
    {
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

        // 마스터 서버와 슬레이브 서버의 통신인지, 서버(a)와 클라이언트(b)의 통신인지 구분
        if (NULL == file_path)
        {
            // 파일 저장 경로 생성 및 포인터 이동.
            snprintf(path, MAX_LENGTH, "%s", serialized_data_ptr);
            serialized_data_ptr += file_path_size;
            dir_path_create(path, 1);
        }
        else
        {
            // 파일 저장 경로 생성 및 포인터 이동. b의 저장 경로와 합친 경로 생성
            snprintf(path, MAX_LENGTH, "%s%s", file_path, serialized_data_ptr);
            serialized_data_ptr += file_path_size;
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

        printf("path:%s\tfile_path_size:%ld\tfile_data_size:%ld\n", path, file_path_size, file_data_size);
        fclose(file);
    }

    return 1;
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

    int total_size = 0;
    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        // 파일의 경로 길이 저장 및 포인터 이동
        memcpy(serialized_data_ptr, &current_file_data->file_path_size, sizeof(unsigned long));
        serialized_data_ptr += sizeof(unsigned long);

        // 파일 크기 저장 및 포인터 이동
        memcpy(serialized_data_ptr, &current_file_data->file_data_size, sizeof(unsigned long));
        serialized_data_ptr += sizeof(unsigned long);

        // 파일의 경로 저장 및 포인터 이동
        snprintf(serialized_data_ptr, current_file_data->file_path_size + 1, "%s", current_file_data->path);
        serialized_data_ptr += current_file_data->file_path_size;

        FILE *file = NULL;
        file = fopen(current_file_data->path, "rb");

        // 파일 데이터 저장 및 포인터 이동
        fread(serialized_data_ptr, 1, current_file_data->file_data_size, file);
        serialized_data_ptr += current_file_data->file_data_size;

        fclose(file);
    }
}
