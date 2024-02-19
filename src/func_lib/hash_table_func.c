/**
 * @file hash_table_func.c
 * @brief 해쉬 테이블 관련 라이브러리
 *
 */
#include "hash_table_func.h"

/**
 * @brief print_all_files
 * 해쉬 테이블에 있는 모든 맴버을 출력하는 함수
 *
 * @param file_list_t **file_list
 *
 * @return void
 */
void print_all_files(file_list_t **file_list)
{

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;
    HASH_ITER(hh, *file_list, current_file_data, tmp)
    {
        printf("Path: %s\n", current_file_data->path);
        // printf("File_Name: %s\n", current_file_data->file_name);
        //   printf("File Path Size: %lu\n", current_file_data->file_path_size);
        //  printf("File Data Size: %lu\n", current_file_data->file_data_size);
        //  printf("Update Time: %ld\n", current_file_data->update_time);
        // printf("State: %d\n", current_file_data->state);

        // check_sum은 바이트 배열이므로, 각 바이트를 16진수 형태로 출력합니다.
        // printf("Check Sum: ");
        // for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        // {
        //     printf("%02x", current_file_data->check_sum[i]);
        // }
        printf("\n");
    }
}

/**
 * @brief find_file_data
 * 해쉬 테이블에서 특정 경로의 데이터를 찾는 함수
 *
 * @param file_list_t **file_list
 *
 * @param char *path
 *
 * @return file_list_t
 */
file_list_t *find_file_data(file_list_t *file_list, char *path)
{
    if (NULL == path)
    {
        printf("find_file_data의 매개변수가 올바르지 않습니다.\n");
        return NULL;
    }

    file_list_t *current_file_data = NULL;

    HASH_FIND_STR(file_list, path, current_file_data);

    if (NULL == current_file_data)
    {
        printf("%s를 찾을 수 없습니다.\n", path);
    }

    return current_file_data;
}

/**
 * @brief add_path
 * 해쉬 테이블에서 특정 경로의 데이터를 추가하는 함수
 *
 * @param file_list_t **file_list
 *
 * @param char *path
 *
 * @return void
 */
void add_path(file_list_t **file_list, char *path)
{
    if (NULL == path)
    {
        printf("add_path의 매개변수가 올바르지 않습니다.\n");
        return;
    }

    file_list_t *current_file_data = NULL;

    HASH_FIND_STR(*file_list, path, current_file_data);

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

    char *file_name = NULL;
    file_name = strrchr(path, '/');
    file_name++;

    current_file_data = (file_list_t *)malloc(sizeof(file_list_t));

    current_file_data->path = strndup(path, MAX_LENGTH);
    current_file_data->file_path_size = strlen(path) + 1;
    current_file_data->file_name = strndup(file_name, MAX_LENGTH);
    current_file_data->update_time = file_data.st_mtime;
    current_file_data->file_data_size = file_data.st_size;
    // 이후 해쉬테이블을 관리하기 위한 맴버
    // 0은 평상시 상태, 1은 변경사항이 발생해 전송할 상태, -1은 삭제해야하는 상태
    // add_path가 실행되면 초기 동기화든, 변경사항 동기화든 전송할 필요가 있기에 1로 설정
    current_file_data->state = 1;
    check_sum_generater(path, current_file_data->check_sum, &sha256);

    HASH_ADD_KEYPTR(hh, *file_list, current_file_data->path, strlen(current_file_data->path), current_file_data);
    printf("%s가 추가되었습니다.\n", path);
}

/**
 * @brief delete_file_data
 * 해쉬 테이블에서 특정 경로의 데이터를 제거하는 함수
 *
 * @param file_list_t **file_list
 *
 * @param char *path
 *
 * @return void
 */
void delete_file_data(file_list_t **file_list, char *path)
{
    if (NULL == path)
    {
        printf("delete_file_data의 매개변수가 올바르지 않습니다.\n");
        return;
    }

    file_list_t *current_file_data = find_file_data(*file_list, path);

    if (NULL != current_file_data)
    {
        HASH_DEL(*file_list, current_file_data);
        printf("%s가 삭제되었습니다.\n", path);
        free(current_file_data->path);
        free(current_file_data->file_name);
        free(current_file_data);

        return;
    }

    printf("%s를 삭제하지 못했습니다.\n", path);
}

/**
 * @brief delete_state_clear
 * 해쉬 테이블에서 제거 상태(state값이 -1)인 요소들을 제거하는 함수
 *
 * @param file_list_t **file_list
 *
 * @return void
 */
void delete_state_clear(file_list_t **file_list)
{
    if (NULL == *file_list)
    {
        printf("delete_state_clear의 매개변수가 올바르지 않습니다.\n");
        return;
    }
    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    unsigned long total_size = 0;
    int file_count = 0;
    HASH_ITER(hh, *file_list, current_file_data, tmp)
    {
        if (-1 == current_file_data->state)
        {
            printf("제거 경로: %s\n", current_file_data->path);
            delete_file_data(file_list, current_file_data->path);
        }
    }
}

/**
 * @brief clear_file_list
 * 해쉬 테이블에 있는 모든 맴버을 제거하는 함수
 *
 * @param file_list_t **file_list
 *
 * @return void
 */
void clear_file_list(file_list_t **file_list)
{

    file_list_t *current_file_data, *tmp;
    HASH_ITER(hh, *file_list, current_file_data, tmp)
    {
        delete_file_data(file_list, current_file_data->path);
    }
    printf("file_list가 삭제되었습니다.\n");
}

/**
 * @brief total_file_size_cal
 * 해쉬 테이블에 있는 모든 파일과 경로의 크기의 합을 구하는 함수
 *
 * @param file_list_t **file_list
 *
 * @return unsigned long
 */
unsigned long total_file_size_cal(file_list_t *file_list)
{

    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    unsigned long total_size = 0;
    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        total_size += current_file_data->file_path_size + sizeof(unsigned long);
        total_size += current_file_data->file_data_size + sizeof(unsigned long);
    }

    return total_size;
}

/**
 * @brief check_path
 * 파일이 업데이트 되었는지 체크하는 함수
 *
 * @param file_list_t *current_file_data
 *
 * @param char *path
 *
 * @return int
 */
int check_path(file_list_t *current_file_data, char *path)
{
    struct stat file_data;
    memset(&file_data, 0, sizeof(file_data));
    stat(path, &file_data);
    printf("path: %s", current_file_data->path);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    if (current_file_data->update_time == file_data.st_mtime)
    {
        current_file_data->state = 0;
    }
    else
    {
        printf(" 변경\n");
        current_file_data->update_time = file_data.st_mtime;
        current_file_data->file_data_size = file_data.st_size;

        check_sum_generater(path, current_file_data->check_sum, &sha256);
        current_file_data->state = 1;
    }

    return current_file_data->state;
}

/**
 * @brief change_state
 * 경로들의 상태를 일괄 변경하는 함수
 *
 * @param file_list_t *file_list
 *
 * @param int state
 *
 * @return void
 */
void change_state(file_list_t *file_list, int state)
{
    file_list_t *current_file_data = NULL;
    file_list_t *tmp = NULL;

    HASH_ITER(hh, file_list, current_file_data, tmp)
    {
        current_file_data->state = state;
    }
}
