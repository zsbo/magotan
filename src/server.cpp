#include <stdio.h>
#include <stdlib.h> // exit()
#include <stdint.h> // uint8_t
#include <string.h> // memset(), memcpy(), strcpy(), strerror()
#include <unistd.h> // close()
#include <sys/socket.h> // socket(), bind(), listen(), accept(), recv(), send()
#include <arpa/inet.h> // htons(), htons(), inet_addr()
#include <errno.h> // errno

#define BUF_SIZE 140

int main()
{
    int server_sockfd;
    server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sockfd == -1) {
        perror("Faild to create socket.");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof (struct sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof (struct sockaddr_in)) == -1) {
        perror("Faild to bind socket.");
        exit(1);
    }

    if (server_addr.sin_port == 0) {
        socklen_t server_addrlen = sizeof (struct sockaddr_in);
        getsockname(server_sockfd, (struct sockaddr *)&server_addr, &server_addrlen);
        printf("real port: %hu\n", ntohs(server_addr.sin_port));
        // %hu 用于显示无符号短整型
    }

    if (listen(server_sockfd, SOMAXCONN) == -1) {
        perror("Faild to listen socket.");
        exit(1);
    }

    int client_sockfd;
    client_sockfd = accept(server_sockfd, NULL, NULL);
    if (client_sockfd == -1) {
        perror("Failed to accept a new connection on a socket.");
        exit(1);
    }

    int buflen;
    char buf[BUF_SIZE];

    do {
        buflen = recv(client_sockfd, buf, BUF_SIZE, 0);
        if (buflen > 0) {
            buf[buflen] = '\0';
            printf("recv: %d: %s", buflen, buf);
        } else if (buflen == 0) {
            printf("Connection closed by foreign host.\n");
            break;
        } else if (buflen == -1) {
            fprintf(stderr, "Failed to receive bytes from socket %d: %s\n",
                client_sockfd, strerror(errno));
            exit(1);
        }
    } while (strncasecmp(buf, "quit", 4) != 0);

    close(client_sockfd);
    close(server_sockfd);
    return 0;
}