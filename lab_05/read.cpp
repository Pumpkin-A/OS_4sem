#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

// объявить флаг завершения потока;
// int readFlag = 0;
// объявить дескриптор именованного канала;
int fileDescriptor;




void* readProc(void* arg) {
    int exitFlag = 0;
std::cout << "read start" << '\n';
// объявить буфер;
char buf[10];
int* curArg = (int*) arg;
while(*curArg != 1) {
    std::cout << "read" << '\n';
    // очистить буфер;
    for (int i = 0; i < sizeof(buf); i++) {
        buf[i] = '\0'; // присвоение нулевого значения каждому элементу массива
    }
    // прочитать сообщение из неименованного канала в буфер;
    ssize_t nr = read(fileDescriptor, buf, sizeof(buf));
    if (nr == -1) {
        perror("read error");
    }
    else if (nr == 0 && exitFlag == 1) {
        std::cout << '\n' << "Читающий поток закончил работу." << '\n';
        exit(0);
    }
    else if (nr > 0){
    exitFlag = 1;
    // вывести сообщение на экран;
    std::cout << buf << '\n';
    }
    sleep(1);
    }
std::cout << '\n' << "Читающий поток закончил работу." << '\n';
pthread_exit((void*)1);
}


int main() {
const char *myFifo = "/tmp/my_fifo";
// объявить идентификатор потока;
pthread_t readId;
// создать именованный канал;
int fifo = mkfifo(myFifo, 0666);   

if(fifo == -1) { 
    perror("фифо уже создан");
}
printf("FIFO успешно создан\n");    
// открыть именованный канал для чтения;
fileDescriptor = open(myFifo, O_RDONLY | O_NONBLOCK, 0666);
// создать поток из функции потока;
int readFlag = 0;
int readThread = pthread_create(&readId, NULL, readProc, &readFlag);
// ждать нажатия клавиши;
std::cout << '\n' << "Программа ждет нажатия клавиши." << '\n';
getchar();
std::cout << '\n' << "Клавиша нажата." << '\n';
// установить флаг завершения потока;
readFlag = 1;
// ждать завершения потока;
int* exitcodeOfReadThread;
pthread_join(readId, (void**)&exitcodeOfReadThread);
std::cout << "exitcodeOfReadThread = " << exitcodeOfReadThread << '\n';
// закрыть именованный канал;
close(fileDescriptor);
// удалить именованный канал;
unlink(myFifo);
}
