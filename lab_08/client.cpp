#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/vfs.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <libgen.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>

long c = 0, s = 0;

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg);} while (0)

struct th_client_args
{
    int flag;
    int connection_socket;
    struct sockaddr_in name;
    rlimit msg;
};



void* pth_read(void* arg) {
    th_client_args* args = (th_client_args*)arg;
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(8001);

    ssize_t result;

    while (args->flag == 0) {
        socklen_t len;
        result = recvfrom(args->connection_socket, &(args->msg), sizeof(args->msg), 0, (struct sockaddr*)&serv, &len);
        if (result == -1) {
            handle_error_en(errno, "\n msgsnd()");
        } else if (result == 0)
            printf("ERROR\n");
        else {
            printf("\nThe request has been read: %ld. ", s + 1);
            printf("Read: %ld\n", args->msg);
            s++;
        }
        sleep(1);
    }
    pthread_exit(NULL);
}
void* pth_write(void* arg) {
    th_client_args* args = (th_client_args*)arg;
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(8001);

    ssize_t result;

    while (args->flag == 0) {
        if (c == s) {
            c++;
            result = sendto(args->connection_socket, &c, sizeof (c), 0, (struct sockaddr*)&serv, sizeof(struct sockaddr_in));
            if (result == -1) {
                handle_error_en(errno, "\n msgsnd()");
            }
            else
                printf("\nThe request has been sent: %ld", c);
            sleep(1);
        }
    }
    pthread_exit(NULL);

}

int main() {

    printf("Client has started working\n");
    pthread_t thread1;
    pthread_t thread2;
    struct th_client_args params;

    params.connection_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (params.connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&params.name, 0, sizeof(struct sockaddr_in));

    params.name.sin_family = AF_INET;
    params.name.sin_port = htons(8000);
    params.name.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = bind(params.connection_socket, (const struct sockaddr*)&params.name,
                   sizeof(struct sockaddr_in));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    fcntl(params.connection_socket, F_SETFL, O_NONBLOCK);

    params.flag = 0;

    pthread_create(&thread1, NULL, pth_read, &params);
    pthread_create(&thread2, NULL, pth_write, &params);
    printf("\nClient is waiting for the key to be pressed\n");

    getchar();
    params.flag = 1;

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    close(params.connection_socket);

    return 0;
}
