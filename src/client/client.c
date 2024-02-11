#include "../../include/file_func.h"
#include "hash_table_func.h"
#include "../../include/socket_func.h"

#define DESTINATION_PATH "../../sync_dir/"

int main(int argc, char *argv[])
{
    dir_path_create(DESTINATION_PATH, 0);
    int client_soket_fd = client_action();
    char buffer[MAX_DATA_SIZE];

    while (TRUE)
    {
        read(client_soket_fd, buffer, MAX_DATA_SIZE);
        if (NULL != buffer)
        {
            printf("%s\n", buffer);
        }
    }
    close(client_soket_fd);
    return 0;
}