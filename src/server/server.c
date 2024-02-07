#pragma GCC diagnostic ignored "-Wunused-result"
#include "../../include/file_func.h"
#include "hash_table_func.h"

char base_path[MAX_LENGTH];
file_list_t *file_list = NULL;

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

    process_sync_file(sync_file_path);

    return 0;
}
