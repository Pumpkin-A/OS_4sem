#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <poll.h>

#define SEM_NAME "/semName" 
#define FILE_NAME "text.txt"

int main() {

    sem_t* sem;
    int fd;
    char symbol = '2';
    int flag_exit = 0;
    int i;

    sem = sem_open(SEM_NAME, O_CREAT, 0777, 1);
    if (sem == SEM_FAILED) {
        perror("Ошибка при создании семафора");
        exit(EXIT_FAILURE);
    } else {
        printf("Семафор создан\n");
         usleep(700000);
    }

    fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("Ошибка при создании файла");
        exit(EXIT_FAILURE);
    } else {
        printf("Файл открыт\n");
         usleep(700000);
    }

    while (1) {

        if (sem_wait(sem) == -1) {
            perror("Ошибка при захвате семафора");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < 10; i++) {
            write(fd, &symbol, 1);
            putchar(symbol);
            fflush(stdout);
            usleep(700000);
        }

        if (sem_post(sem) == -1) {
            perror("Ошибка при освобождении семафора");
            exit(EXIT_FAILURE);
        }
   	//Закрытие только по нажатию на Enter
        fd_set fds;
        struct timespec tv;
        int retval;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec = 0;
        tv.tv_nsec = 0;
        retval = pselect(STDIN_FILENO + 1, &fds, NULL, NULL, &tv, NULL);
        if (retval == -1)
        {
            perror("select()");
            exit(EXIT_FAILURE);
        }
        else if (retval)
        {
            int c = getchar();
            if (c == '\n')
            {
            	printf("\n");
                break;
            }
        }

        sleep(1);
    }

    if (close(fd) == -1) {
        perror("Ошибка при закрытии файла");
        exit(EXIT_FAILURE);
    } else {
        printf("Файл закрыт\n");
         usleep(700000);
    }
    if (sem_close(sem) == -1) {
        perror("Ошибка при закрытии семафора");
        exit(EXIT_FAILURE);
    } else {
        printf("Семафор закрыт\n");
         usleep(700000);
    }
    
    if (sem_unlink(SEM_NAME) == -1) {
        perror("Ошибка при удалении семафора или он уже удален");
        exit(EXIT_FAILURE);
    } else {
        printf("Семафор удален\n");
         usleep(700000);
    }

    return 0;
}
