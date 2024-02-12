/**
 * @file file_func.c
 * @brief 파일 관련 라이브러리
 *
 */
#include "file_func.h"

char base_path[MAX_LENGTH];
/**
 * @brief dir_path_create
 * 디렉터리 경로 생성 함수
 *
 * @param const char *path
 *
 * @param int flag
 *
 * @return void
 */
void dir_path_create(char *path, int flag)
{
    if (NULL == path)
    {
        printf("dir_path_create의 매개변수가 올바르지 않습니다.\n");
        return;
    }
    char *token = NULL;
    char *delimiter = "/";
    char *path_copy = NULL;

    path_copy = strdup(path);

    if (1 == flag)
    {
        flie_name_remover(path_copy);
    }

    char current_path[MAX_LENGTH];
    memset(current_path, 0, sizeof(current_path));

    char relative_path[MAX_LENGTH];
    memset(relative_path, 0, sizeof(relative_path));

    token = strtok(path_copy, delimiter);

    while (2 == relative_path_check(token))
    {
        strcat(relative_path, token);
        strcat(relative_path, "/");
        token = strtok(NULL, delimiter);
    }
    realpath(relative_path, current_path);

    while (NULL != token)
    {

        strcat(current_path, "/");
        strcat(current_path, token);

        // 디렉터리 생성
        if (-1 == mkdir(current_path, 0777))
        {
        }

        token = strtok(NULL, delimiter);
    }

    if (NULL != path_copy)
    {
        free(path_copy);
        path_copy = NULL;
    }
}
/**
 * @brief check_sum_generater
 * 파일의 체크썸을 생성하는 함수
 *
 * @param char *path
 *
 * @param unsigned char *check_sum
 *
 * @param SHA256_CTX *sha256
 *
 * @return int
 */
int check_sum_generater(char *path, unsigned char *check_sum, SHA256_CTX *sha256)
{
    if (NULL == path)
    {
        printf("check_sum_generater의 매개변수가 올바르지 않습니다.\n");
        return -1;
    }

    FILE *file = NULL;
    file = fopen(path, "rb");

    if (NULL == file)
    {
        printf("%s의 파일을 열 수 없습니다.\n", path);
        return -1;
    }

    unsigned char buffer[MAX_DATA_SIZE];
    memset(buffer, 0, sizeof(buffer));

    unsigned long bytesRead = 0;

    while ((bytesRead = fread(buffer, 1, MAX_DATA_SIZE, file)))
    {
        SHA256_Update(sha256, buffer, bytesRead);
    }

    SHA256_Final(check_sum, sha256);

    fclose(file);

    return 1;
}

/**
 * @brief absolute_path_change
 * 상대 경로를 절대경로로 변환하는 함수
 *
 * @param char *path
 *
 * @return void
 */
char *absolute_path_change(char *path)
{
    if (NULL == path)
    {
        printf("absolute_path_change의 매개변수가 올바르지 않습니다.\n");
        return NULL;
    }
    char buffer[MAX_LENGTH * 2];
    memset(buffer, 0, sizeof(buffer));
    if (base_path[0] == '\0')
    {
        realpath(path, buffer);
    }
    else
    {
        snprintf(buffer, MAX_LENGTH * 2, "%s/%s", base_path, path);
    }

    char *token = NULL;
    token = strtok(buffer, "\n");

    return token;
}

/**
 * @brief relative_path_check
 * 상대 경로인지를 체크하는 정규식 함수
 *
 * @param char *path
 *
 *
 * @return int result
 */
int relative_path_check(char *path)
{
    if (NULL == path)
    {
        printf("relative_path_check의 매개변수가 올바르지 않습니다.\n");
        return -1;
    }
    const char *pattern = "(\\./|\\.\\./|\\.\\.)";
    int check_result = 0;

    regex_t regex;
    memset(&regex, 0, sizeof(regex));

    int reti = 0;
    reti = regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE);

    // 정규식 매치 여부 확인
    reti = regexec(&regex, path, 0, NULL, 0);
    if (REG_NOMATCH == reti)
    {
        // 절대 경로
        check_result = 1;
    }
    else
    {
        // 상대 경로
        check_result = 2;
    }
    regfree(&regex);
    return check_result;
}

/**
 * @brief file_path_check
 * 유효한 파일 경로인지 검증하는 함수
 *
 * @param char *path
 *
 * @return int
 */
int file_path_check(char *path)
{
    if (NULL == path)
    {
        return 0;
    }

    struct stat file_data;
    memset(&file_data, 0, sizeof(file_data));

    int result = 0;
    result = stat(path, &file_data);

    if (0 == result)
    {
        return 1;
    }
    else
    {
        printf("path: %s에 파일이 존재하지 않습니다.\n", path);
        return 0;
    }
}

/**
 * @brief flie_name_remover
 * 경로에서 파일의 이름을 지워주는 함수
 *
 * @param char *path
 *
 * @return int
 */
int flie_name_remover(char *path)
{
    if (NULL == path)
    {
        printf("flie_name_remover의 매개변수가 올바르지 않습니다.\n");
        return -1;
    }

    // 문자열에서 마지막 '/' 문자를 찾음
    char *lastSlashPos = strrchr(path, '/');

    if (NULL != lastSlashPos)
    {
        *(lastSlashPos) = '\0'; // '/' 다음 문자를 null 문자로 설정
    }

    return 1;
}

/**
 * @brief process_sync_file
 * 동기화 파일을 읽고, 해쉬 테이블에 저장하는 함수
 *
 * @param file_list_t **file_list
 *
 * @param char *sync_file_path
 *
 * @return void
 */
void process_sync_file(file_list_t **file_list, char *sync_file_path)
{
    if (NULL == sync_file_path)
    {
        printf("process_sync_file의 매개변수가 올바르지 않습니다.\n");
        return;
    }
    FILE *file = fopen(sync_file_path, "r");
    static char buffer[MAX_LENGTH];
    memset(buffer, 0, sizeof(buffer));

    char *token = NULL;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (2 == relative_path_check(buffer))
        {
            token = absolute_path_change(buffer);
        }
        else
        {
            token = strtok(buffer, " \n");
        }

        if (1 == file_path_check(token))
        {
            add_path(file_list, token);
        }
    }
    print_all_files(file_list);

    fclose(file);
}

/**
 * @brief update_check_sync_file
 * 동기화 파일을 읽고, 해쉬 테이블에 저장하는 함수
 *
 * @param file_list_t **file_list
 *
 * @param char *sync_file_path
 *
 * @return int
 */
int update_check_sync_file(file_list_t **file_list, char *sync_file_path)
{

    FILE *file = fopen(sync_file_path, "r");
    static char buffer[MAX_LENGTH];
    memset(buffer, 0, sizeof(buffer));

    int result = 0;

    char *token = NULL;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (2 == relative_path_check(buffer))
        {
            token = absolute_path_change(buffer);
        }
        else
        {
            token = strtok(buffer, " \n");
        }

        if (1 == file_path_check(token))
        {
            file_list_t *current_file_data = find_file_data(*file_list, token);

            if (NULL == current_file_data)
            {
                result = 1;
                add_path(file_list, token);
            }
            else if (1 == check_path(current_file_data, token))
            {
                result = 1;
            }
        }
    }
    fclose(file);
    print_all_files(file_list);

    return result;
}
