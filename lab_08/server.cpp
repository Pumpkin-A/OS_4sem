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
#include <sys/un.h>
#include <sys/socket.h>
#include<vector>
#include<pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>


std::vector <int> q;
pthread_mutex_t R;

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg);} while (0)

struct th_client_args
{
    int flag;
    int connection_socket;
    struct sockaddr_in name;
    rlimit msg;
};

void* pth_write(void* arg) {
    th_client_args* args = (th_client_args*)arg;
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    client.sin_port = htons(8000);
    ssize_t result;

    while (args->flag == 0) {


        pthread_mutex_lock(&R);
        if (!q.empty()) {
            q.pop_back();
            pthread_mutex_unlock(&R);
            struct rlimit rlim;
            int ret = getrlimit(RLIMIT_NOFILE, &rlim);
            if (ret == -1) {
                perror("getrlimit");
            }
            args->msg = rlim;
            result = sendto(args->connection_socket, &(args->msg), sizeof(args->msg), 0, (struct sockaddr*)&client, sizeof(client));
            if (result == -1) {
                handle_error_en(errno, "\n msgsnd()");
            }
            else
                printf("\nRequest processed. ");
            printf("Write: %ld\n", args->msg);
            sleep(1);
        }else { pthread_mutex_unlock(&R); }
    }
    pthread_exit(NULL);
}

void* pth_read(void* arg) {
    th_client_args* args = (th_client_args*)arg;
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    client.sin_port = htons(8000);
    ssize_t result;
    int ch;
    while (args->flag == 0) {

        socklen_t len;
        result = recvfrom(args->connection_socket, &ch, sizeof (ch), 0, (struct sockaddr*)&client, &len);
        if (result == -1) {
            handle_error_en(errno, "\n msgsnd()");
        }
        else if (result == 0)
            printf("ERROR\n");
        else {
            printf("\nRequest received: %d\n",ch);
            pthread_mutex_lock(&R);
            q.insert(q.begin(),ch);
            pthread_mutex_unlock(&R);
        }
        sleep(1);
    }
    pthread_exit(NULL);
}

int main() {
    printf("Server has started working\n");
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
    params.name.sin_port = htons(8001);
    params.name.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = bind(params.connection_socket, (const struct sockaddr*)&params.name,
                   sizeof(struct sockaddr_in));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    params.flag = 0;
    fcntl(params.connection_socket, F_SETFL, O_NONBLOCK);
    pthread_mutex_init(&R, NULL);
    pthread_create(&thread1, NULL, pth_write, &params);
    pthread_create(&thread2, NULL, pth_read, &params);
    printf("\nServer is waiting for the key to be pressed\n");


    getchar();
    params.flag = 1;

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_mutex_destroy(&R);
    close(params.connection_socket);

    return 0;
}
