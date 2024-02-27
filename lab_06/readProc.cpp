#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <poll.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <signal.h>

#define SEM_READ_NAME "/semReadName" 
#define SEM_WRITE_NAME "/semWriteName" 
// объявить флаг завершения потока;
int flag = 0;
// объявить идентификатор семафора записи;
sem_t* writeId;
// объявить идентификатор семафора чтения;
sem_t* readId;
// объявить идентификатор разделяемой памяти;
int shmId;
// объявить локальный адрес;

void* func(void* args) {
    while (flag == 0) {
    // ждать семафора записи;
    if(sem_wait(writeId) == 0) {
            printf("Semaphore acquired!\n");
    } else {
        printf("sem_wait error : %s\n", strerror(errno));
        exit(1);
    }
    
    // скопировать данные из разделяемой памяти в локальную переменную;
    char* data = (char*)shmat(shmId, NULL, 0);
    if (data == (char*)(-1)) {
        perror("shmat");
    }
    std::cout << "Данные прочитаны: " << data << '\n';
    // освободить семафор чтения;
        sem_post(readId);
    // задержать на время 1 сек;
        sleep(1);
    }
    printf("поток чтения завершен.\n");
    pthread_exit(NULL);
}

int main() {
    printf("Программа 1 начала работу.\n");
// объявить идентификатор потока;
    pthread_t id;
// создать (или открыть, если существует) разделяемую память;
// обрезать разделяемую память до требуемого размера;
// отобразить разделяемую память на локальный адрес;
    key_t key = ftok("shmfile", 65);
    if ((shmId = shmget(key, 32, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }

// создать (или открыть, если существует) семафор записи;
    writeId = sem_open(SEM_WRITE_NAME, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1);
    if (writeId == SEM_FAILED) {
        printf("Error: %s\n", strerror(errno));
        exit(errno);
    }
// создать (или открыть, если существует) семафор чтения;
    readId = sem_open(SEM_READ_NAME, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1);
    if (readId == SEM_FAILED) {
        printf("Error: %s\n", strerror(errno));
        exit(errno);
    }
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
// закрыть семафор чтения;
    sem_close(readId);
// удалить семафор чтения;
    sem_unlink(SEM_READ_NAME);
// закрыть семафор записи;
    sem_close(writeId);
// удалить семафор записи;
    sem_unlink(SEM_WRITE_NAME);
// закрыть отображение разделяемой памяти на локальный адрес;
// Удаляем разделяемую память
    if (shmctl(shmId, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        return 1;
    }
    return 0;
}