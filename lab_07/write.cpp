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
    char buf[buffSize];
    int  i = 0; 
    while (flag == 0) {
        // выполнить заданную функцию;
        struct rlimit rlim;
        int ret = getrlimit(RLIMIT_NOFILE, &rlim);
        if (ret == -1) {
            perror("getrlimit");
        }
        
        // вывести результат работы функции на экран;
        sprintf(buf, "%ld", rlim.rlim_cur);

        // записать результат работы функции в очередь сообщений;
        struct timespec time;
        clock_gettime(CLOCK_REALTIME, &time);
        time.tv_sec += 5;
        // mq_getattr(mqd, &attr);
        // printf("Количество сообщений в очереди: %ld\n", attr.mq_curmsgs);
        int N = strlen(buf) + 1;
        if (mq_timedsend(mqd, buf, N, 0, &time) == -1) {
            perror("Error sending message");
            exit(EXIT_FAILURE);
        }
        else
            printf("Отправлено сообщение: %s\n", buf);

        // задержать на время 1 сек;
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

    mqd = mq_open(queueName, O_CREAT | O_WRONLY | O_NONBLOCK, 0644, &attr);

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

