/**
 * @file socket_func.c
 * @brief socket관련 라이브러리
 *
 */
#include "socket_func.h"
#include <errno.h>

/**
 * @brief socket_create
 * 소켓 생성 함수
 * @param int domain
 *
 * @param int type
 *
 * @param int protocol
 *
 * @return int socket_fd
 */
int socket_create(int domain, int type, int protocol)
{
    if (0 == domain)
    {
        printf("socket_create의 매개변수가 올바르지 않습니다.\n");
        return -1;
    }
    int socket_fd = 0;
    socket_fd = socket(domain, type, protocol);
    if (socket_fd < 0)
    {
        printf("소켓 생성에 실패했습니다.\n");

        printf("오류 코드: %d\n", errno);
        printf("오류 메시지: %s\n", strerror(errno));
    }
    return socket_fd;
}

/**
 * @brief server_address_set
 * 서버 주소 설정 함수
 * @param struct sockaddr_in *address
 *
 * @param int domain
 *
 * @param int port
 *
 * @return void
 */
void server_address_set(struct sockaddr_in *address, int domain, int port)
{
    if (0 == domain)
    {
        printf("server_address_set의 매개변수가 올바르지 않습니다.\n");
        return;
    }
    memset(address, 0, sizeof(*address));
    address->sin_family = domain;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);
}

/**
 * @brief socket_connect
 * socket 연결 시도 함수
 *
 * @param int socket_fd
 *
 * @param struct sockaddr_in *address
 *
 * @return int connect_result
 */
int socket_connect(int socket_fd, struct sockaddr_in *address)
{
    if (0 == socket_fd || NULL == address)
    {
        printf("socket_connect의 매개변수가 올바르지 않습니다.\n");
        return -1;
    }
    int connect_result = connect(socket_fd, (struct sockaddr *)address, sizeof(*address));
    if (-1 == connect_result)
    {
        printf("연결에 실패했습니다.\n");

        printf("오류 코드: %d\n", errno);
        printf("오류 메시지: %s\n", strerror(errno));
    }
    return connect_result;
}

/**
 * @brief socket_bind
 * socket을 주소에 바인딩하는 함수
 *
 * @param int socket_fd
 *
 * @param struct sockaddr_in *address
 *
 * @return void
 */
void socket_bind(int socket_fd, struct sockaddr_in *address)
{
    if (-1 == socket_fd || NULL == address)
    {
        printf("socket_bind의 매개변수가 올바르지 않습니다.\n");
        return;
    }
    if (-1 == bind(socket_fd, (struct sockaddr *)address, sizeof(*address)))
    {
        printf("바인딩에 실패했습니다.\n");
        printf("오류 코드: %d\n", errno);
        printf("오류 메시지: %s\n", strerror(errno));

        close(socket_fd);
    }
}

/**
 * @brief socket_listen
 * 연결 대기 함수
 *
 * @param int socket_fd
 *
 * @param int backlog
 *
 * @return void
 */
void socket_listen(int socket_fd, int backlog)
{
    if (-1 == socket_fd)
    {
        printf("socket_listen의 매개변수가 올바르지 않습니다.\n");
        return;
    }
    if (-1 == listen(socket_fd, backlog))
    {
        printf("연결 대기에 실패했습니다.\n");
        printf("오류 코드: %d\n", errno);
        printf("오류 메시지: %s\n", strerror(errno));

        close(socket_fd);
    }
}

/**
 * @brief socket_accept
 * 클라이언트 소켓 연결 허가 함수
 *
 *@param int socket_fd
 *
 *@param struct sockaddr  *address
 *
 *@param socklen_t *addrlen
 *
 *@return int client_socket_fd
 *
 */
int socket_accept(int socket_fd, struct sockaddr *address, socklen_t *addrlen)
{
    if (-1 == socket_fd || NULL == addrlen)
    {
        printf("socket_accept의 매개변수가 올바르지 않습니다.\n");
        close(socket_fd);

        return -1;
    }
    int client_socket_fd = 0;
    client_socket_fd = accept(socket_fd, address, addrlen);
    if (-1 == client_socket_fd)
    {
        printf("\n");
        printf("연결에 실패했습니다.\n");
        printf("오류 코드: %d\n", errno);
        printf("오류 메시지: %s\n", strerror(errno));

        close(socket_fd);
    }

    return client_socket_fd;
}

/**
 * @brief socket_select
 * 클라이언트 소켓 연결 허가 함수
 *
 *@param int slave_server_socket
 *
 *@return int master_server_socket
 *
 */
int socket_select(int slave_server_socket)
{
    if (-1 == slave_server_socket)
    {
        printf("socket_select의 매개변수가 올바르지 않습니다.\n");
        return -1;
    }
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(slave_server_socket, &read_fds);

    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = WAIT_TIME;
    timeout.tv_usec = 0;

    // 마스터 서버와의 통신 연결과 기존의 대기 동작을 select로 한번에 처리
    int ready = select(slave_server_socket + 1, &read_fds, NULL, NULL, &timeout);

    // 마스터 서버와의 연결이 없으면 종료
    if (0 == ready)
    {
        return -1;
    }

    if (FD_ISSET(slave_server_socket, &read_fds))
    {
        // 클라이언트 연결 수락
        struct sockaddr_in master_server_addr;
        memset(&master_server_addr, 0, sizeof(master_server_addr));

        socklen_t master_server_len = 0;
        master_server_len = sizeof(master_server_addr);

        int master_server_socket = 0;
        master_server_socket = accept(slave_server_socket, (struct sockaddr *)&master_server_addr, &master_server_len);
        if (-1 == master_server_socket)
        {
            perror("연결 수락 실패");
        }
        printf("연결 성공\n");

        return master_server_socket;
    }
}

/**
 * @brief client_action
 * 클라이언트 측 소켓 함수 집합
 *
 *@return int socket_fd
 *
 */
int client_action()
{
    int socket_fd = 0;
    socket_fd = socket_create(AF_INET, SOCK_STREAM, PROTOCOL);
    // 서버 주소 설정
    struct sockaddr_in address;
    server_address_set(&address, AF_INET, PORT);
    // 서버에 연결 시도
    if (-1 == socket_connect(socket_fd, &address))
    {
        return -1;
    }
    return socket_fd;
}

/**
 * @brief select_init
 * 서버에서 selcet함수에 초기 설정을 하는 함수
 *
 * @param   int server_socket_fd
 *
 * @param   int slave_server_socket
 *
 * @param   int *client_socket
 *
 * @param   fd_set *readfds
 *
 * @param   int *max_sd
 *
 * @param   struct timeval *timeout
 *
 * @return void
 *
 */
void select_init(int server_socket_fd, int slave_server_socket, int *client_socket, fd_set *readfds, int *max_sd, struct timeval *timeout)
{
    FD_ZERO(readfds);
    FD_SET(server_socket_fd, readfds);
    if (slave_server_socket > 0)
    {
        FD_SET(slave_server_socket, readfds);
    }

    *max_sd = server_socket_fd;

    if (server_socket_fd > slave_server_socket)
    {
        *max_sd = server_socket_fd;
    }
    else
    {
        *max_sd = slave_server_socket;
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_socket[i] > 0)
        {
            FD_SET(client_socket[i], readfds);
            if (client_socket[i] > *max_sd)
            {
                *max_sd = client_socket[i];
            }
        }
    }

    timeout->tv_sec = WAIT_TIME;
    timeout->tv_usec = 0;
}

/**
 * @brief client_add
 * 클라이언트 소켓 배열에 새로 연결되는 소켓을 저장하는 함수
 *
 * @param   int new_socket
 *
 * @param   int *client_socket
 *
 * @return void
 *
 */
void client_add(int new_socket, int *client_socket)
{
    for (int index = 0; index < MAX_CLIENTS; index++)
    {
        if (0 == client_socket[index])
        {
            printf("새 클라이언트로부터의 요청 %d\n", index);
            client_socket[index] = new_socket;
            break;
        }
    }
}

/**
 * @brief client_connect_check
 * 연결이 종료된 소켓을 정리하는 함수
 *
 * @param   int client_socket
 *
 * @return  int
 *
 */
int client_connect_check(int client_socket)
{
    int valread = 0;
    char buffer[MAX_DATA_SIZE];
    memset(buffer, 0, sizeof(buffer));

    if (0 == read(client_socket, buffer, MAX_DATA_SIZE))
    {
        printf("기존 클라이언트가 연결이 종료되었습니다\n");
        close(client_socket);
        client_socket = 0;
    }

    return client_socket;
}

/**
 * @brief master_server_action
 * 동기화 서버 리스트를 읽고,스레드를 생성하는 함수
 *
 * @param file_list_t *file_list
 *
 * @param char *sync_server_path
 *
 * @return void
 */
void master_server_action(file_list_t *file_list, char *sync_server_path)
{
    if (NULL == file_list)
    {
        printf("sync_server_transfer_action 매개변수가 올바르지 않습니다.\n");
        return;
    }
    // IP 주소를 저장할 배열
    in_addr_t ip_addresses[MAX_IPS];
    memset(ip_addresses, 0, sizeof(ip_addresses));

    int ip_count = 0;
    ip_count = process_sync_server(sync_server_path, ip_addresses);

    for (int index = 0; index < MAX_IPS; index++)
    {
        if (0 == ip_addresses[index])
        {
            continue;
        }
        printf("유효한 주소:%d. %s\n", index, inet_ntoa(*(struct in_addr *)&ip_addresses[index]));
    }
    if (ip_count != 0)
    {
        master_server_thread(file_list, ip_addresses, ip_count);
    }
}
/**
 * @brief master_server_connect
 * 마스터 서버에서 서브에게 통신 요청을 하는 함수
 *
 *@param in_addr_t ip_addresses
 *
 *@return int socket_fd
 *
 */
int master_server_connect(in_addr_t ip_addresses)
{
    if (0 == ip_addresses)
    {
        printf("master_server_action의 매개변수가 올바르지 않습니다.\n");
        return -1;
    }

    int socket_fd = 0;
    socket_fd = socket_create(AF_INET, SOCK_STREAM, PROTOCOL);

    // 서버 주소 설정
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = ip_addresses;
    server_addr.sin_port = htons(SERVER_PORT);

    struct timeval timeout;
    timeout.tv_sec = 60;
    timeout.tv_usec = 0;

    setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    // 슬레이브 서버와 연결.
    int connect_result = socket_connect(socket_fd, &server_addr);

    if (-1 == connect_result)
    {
        return -1;
    }

    printf("연결 성공\n");
    return socket_fd;
}