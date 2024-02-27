#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <signal.h>
#include <sys/resource.h> 



// объявить флаги завершения потоков;
int writeFlag = 0; 
int openFlag = 0;
// объявить дескриптор именованного канала;
int fileDescriptor;

pthread_t writeId;

void sig_handler(int signo)
{
    if (signo == SIGPIPE) {
        printf("Received SIGPIPE signal\n");
        
        // Здесь можно закрыть соответствующий дескриптор файла
        close(fileDescriptor);
        
        // // Здесь можно завершить процесс
        exit(1);
    }
}


void* writeProc(void* arg);

void* openProc(void* arg) {
    std::cout << "openProc start" << '\n';
    int* curArg = (int*) arg;
    while(*curArg != 1) {
        std::cout << "open file" << '\n';
        fileDescriptor = open("/tmp/my_fifo", O_WRONLY | O_NONBLOCK, 0666);
        if (fileDescriptor == -1) {
            perror("open error");
            sleep(1);
        }
        else {
            // создать поток передачи;
            int writeThread = pthread_create(&writeId, NULL, writeProc, &writeFlag);
            // завершить текущий поток;
            std::cout << '\n' << "Поток открытия закончил работу." << '\n';
            pthread_exit((void*)1);
        }
    }
}



void* writeProc(void* arg) {
    std::cout << "write start" << '\n';
    // объявить буфер;
    char buf[10];
    int* curArg = (int*) arg;
    while(*curArg != 1) {
        std::cout << "write" << '\n';
        // сформировать сообщение в буфере;
        // Определяем ограничения на максимальное количество открытых файловых дескрипторов.
        struct rlimit rlim;
        int ret = getrlimit(RLIMIT_NOFILE, &rlim);
        if (ret == -1) {
            perror("getrlimit");
        }
        // std::cout << "Максимальное количество открытых файловых дескрипторов: " << rlim.rlim_cur << '\n';
        sprintf(buf, "%ld", rlim.rlim_cur);
        // записать сообщение из буфера в именованный канал;
        ssize_t nr = write(fileDescriptor, buf, sizeof(buf));
        sleep(1);
        }
    std::cout << '\n' << "Пишущий поток закончил работу." << '\n';
    pthread_exit((void*)1);
}   


int main() {
const char *myFifo = "/tmp/my_fifo";
signal(SIGPIPE, sig_handler);
// объявить идентификатор потока;
pthread_t openId;
// создать именованный канал;
int fifo = mkfifo(myFifo, 0666);   
if(fifo == -1) { 
    perror("фифо уже создан");
}
std::cout << "FIFO успешно создан" << '\n';
// создать поток из функции потока открытия;
int openThread = pthread_create(&openId, NULL, openProc, &openFlag);
// ждать нажатия клавиши;
std::cout << '\n' << "Программа ждет нажатия клавиши." << '\n';
getchar();
std::cout << '\n' << "Клавиша нажата." << '\n';
// установить флаги завершения потоков;
writeFlag = 1; 
openFlag = 1;
// ждать завершения потока открытия;
int* exitcodeOfOpenThread;
pthread_join(openId, (void**)&exitcodeOfOpenThread);
std::cout << "exitcodeOfReadThread = " << exitcodeOfOpenThread << '\n';
// ждать завершения потока передачи;
int* exitcodeOfWriteThread;
pthread_join(writeId, (void**)&exitcodeOfWriteThread);
std::cout << "exitcodeOfReadThread = " << exitcodeOfWriteThread << '\n';
// закрыть именованный канал;
close(fileDescriptor);
// удалить именованный канал;
unlink(myFifo);
}
