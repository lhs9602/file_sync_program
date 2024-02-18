#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>
#include "file_func.h"
#include "hash_table_func.h"
#include "thread_func.h"

#define PORT 12345
#define SERVER_PORT 12346 // 서버 포트 번호
#define PROTOCOL 0
#define BUF_SIZE 1024
#define INTERVAL_TIME 1 // 요청 주기 (초)
#define WAIT_TIME 5
#define MAX_CLIENTS 20
#define MAX_IPS 20    // 최대 IP 주소 저장 가능 개수
#define MAX_IP_LEN 16 // IPv4 주소 최대 길이

typedef struct file_list file_list_t;

#ifdef __cplusplus
extern "C"
{
#endif

    int socket_create(int domain, int type, int protocol);
    void server_address_set(struct sockaddr_in *address, int domain, int port);
    int socket_connect(int socket_fd, struct sockaddr_in *address);
    void socket_bind(int socket_fd, struct sockaddr_in *address);
    void socket_listen(int socket_fd, int backlog);
    int socket_accept(int socket_fd, struct sockaddr *address, socklen_t *addrlen);
    int socket_select(int slave_server_socket);
    int client_action();
    void select_init(int server_socket_fd, int *client_socket, fd_set *readfds, int *max_sd, struct timeval *timeout);
    void client_add(int new_socket, int *client_socket);
    int client_connect_check(int client_socket);
    int master_server_connect(in_addr_t ip_addresses);
    void master_server_action(file_list_t *file_list, char *sync_server_path);

#ifdef __cplusplus
}
#endif