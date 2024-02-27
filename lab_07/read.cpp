#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <errno.h>
#include <sys/ipc.h>
#include <cstring>
#include <signal.h>
#include <sys/resource.h>
#include <mqueue.h>
#include <time.h>

// объявить флаг завершения потока;
int flag = 0;
// объявить идентификатор очереди сообщений;
// struct mq_attr attr;
mqd_t mqd;
char queueName[] = {"/example_queue"};
int buffSize = 32;


void* func(void* args) {
    char buf[buffSize] = {'\0'};
    int  status;

    while (flag == 0) {
        struct timespec time;
        // очистить буфер сообщения;
        memset(buf, 0, buffSize);
        clock_gettime(CLOCK_REALTIME, &time);
        time.tv_sec += 5;
        status = mq_timedreceive(mqd, buf, buffSize, NULL, &time);
        if (status > 0) {
            printf("receive: %s\n", buf);
            fflush(stdout);
        }
        else if (status == -1) {
            printf("receive error: %s\n", strerror(errno));
            fflush(stdout);
        }
        sleep(1);
    }
    printf("поток записи завершен.\n");
    pthread_exit(NULL);
}

int main() {
    printf("Программа 1 начала работу.\n");
    // создать (или открыть, если существует) очередь сообщений;
    struct mq_attr attr;

    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = buffSize;
    attr.mq_curmsgs = 0;

    mqd = mq_open(queueName, O_CREAT | O_RDONLY | O_NONBLOCK, 0644, &attr);

    if (mqd == (mqd_t)-1) {
        perror("Error opening message queue");
        exit(EXIT_FAILURE);
    }
    
    // объявить идентификатор потока;
        pthread_t id;
    // создать поток из функции потока;
        pthread_create(&id, NULL, func, NULL);
    // ждать нажатия клавиши;
        printf("Программа 1 ожидает нажатия клавиши.\n");
        getchar();
        printf("Клавиша нажата.\n");
    // установить флаг завершения потока;
        flag = 1;
    // ждать завершения потока;
        pthread_join(id, NULL);
    // закрыть очередь сообщений;
        mq_close(mqd);
    // удалить очередь сообщений;
        mq_unlink(queueName);  

    return 0;
}

