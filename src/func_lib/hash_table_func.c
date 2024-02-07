/**
 * @file hash_table_func.c
 * @brief 해쉬 테이블 관련 라이브러리
 *
 */
#include "hash_table_func.h"

file_list_t *file_list;

/**
 * @brief print_all_files
 * 해쉬 테이블에 있는 모든 맴버을 출력하는 함수
 *
 * @return void
 */
void print_all_files()
{
    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;
    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        printf("Path: %s\n", current_file_data->path);
        printf("File Path Size: %lu\n", current_file_data->file_path_size);
        printf("File Data Size: %lu\n", current_file_data->file_data_size);
        printf("Update Time: %ld\n", current_file_data->update_time);
        printf("State: %d\n", current_file_data->state);

        // check_sum은 바이트 배열이므로, 각 바이트를 16진수 형태로 출력합니다.
        printf("Check Sum: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            printf("%02x", current_file_data->check_sum[i]);
        }
        printf("\n\n");
    }
}

/**
 * @brief find_file_data
 * 해쉬 테이블에서 특정 경로의 데이터를 찾는 함수
 *
 * @param char *path
 *
 * @return file_list_t
 */
file_list_t *find_file_data(char *path)
{
    if (NULL == path)
    {
        printf("find_file_data의 매개변수가 올바르지 않습니다.\n");
        return NULL;
    }
    file_list_t *current_file_data = NULL;
    HASH_FIND_STR(file_list, path, current_file_data);
    return current_file_data;
}

/**
 * @brief add_path
 * 해쉬 테이블에서 특정 경로의 데이터를 추가하는 함수
 *
 * @param char *path
 *
 * @return void
 */
void add_path(char *path)
{
    if (NULL == path)
    {
        printf("add_path의 매개변수가 올바르지 않습니다.\n");
        return;
    }

    file_list_t *current_file_data = NULL;

    // 이미 존재하는 경로인지 확인합니다.
    HASH_FIND_STR(file_list, path, current_file_data);

    // 존재하지 않는 경우, 새로 요소를 동적으로 할당하고 해시 테이블에 추가합니다.
    if (NULL != current_file_data)
    {
        printf("%s는 이미 존재하는 경로입니다.\n", path);
        return;
    }

    struct stat file_data;
    memset(&file_data, 0, sizeof(file_data));
    stat(path, &file_data);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    current_file_data = (file_list_t *)malloc(sizeof *current_file_data);
    current_file_data->path = strdup(path);
    current_file_data->file_path_size = strlen(path) + 1;
    current_file_data->update_time = file_data.st_mtime;
    current_file_data->file_data_size = file_data.st_size;
    current_file_data->state = 0;
    check_sum_generater(path, current_file_data->check_sum, &sha256);

    HASH_ADD_KEYPTR(hh, file_list, current_file_data->path, strlen(path), current_file_data);
    printf("%s가 추가되었습니다.\n", path);
}

/**
 * @brief delete_file_data
 * 해쉬 테이블에서 특정 경로의 데이터를 제거하는 함수
 *
 * @param char *path
 *
 * @return void
 */
void delete_file_data(char *path)
{
    if (NULL == path)
    {
        printf("delete_file_data의 매개변수가 올바르지 않습니다.\n");
        return;
    }
    file_list_t *current_file_data = find_file_data(path);
    if (NULL != current_file_data)
    {
        HASH_DEL(file_list, current_file_data);
        free(current_file_data->path);
        free(current_file_data);
        printf("%s가 삭제되었습니다.\n", path);
        return;
    }
    printf("%s를 삭제하지 못했습니다.\n", path);
}

/**
 * @brief clear_file_list
 * 해쉬 테이블에 있는 모든 맴버을 제거하는 함수
 *
 * @return void
 */
void clear_file_list()
{
    file_list_t *current_file, *tmp;
    HASH_ITER(hh, file_list, current_file, tmp)
    {
        HASH_DEL(file_list, current_file);
        free(current_file->path);
        free(current_file);
    }
    printf("file_list가 삭제되었습니다.\n");
}