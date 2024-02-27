#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

int filedes[2];

void* readProc(void* arg) {
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
        ssize_t nr = read(filedes[0], buf, sizeof(buf));

        // вывести сообщение на экран;
        std::cout << buf << '\n';
        sleep(1);
    }
    std::cout << '\n' << "Читающий поток закончил работу." << '\n';
    pthread_exit((void*)1);
}

void* writeProc(void* arg) {
    std::cout << "write start" << '\n';
    // объявить буфер;
    char buf[10];
    int* curArg = (int*) arg;
    while(*curArg != 1) {
        std::cout << "write" << '\n';

        // вызвать функцию для получения данных;
        clock_t time = clock();

        // сформировать из данных сообщение в буфере;
        sprintf(buf, "%ld", time);
        
        // записать сообщение из буфера в неименованный канал;
        ssize_t nw = write(filedes[1], buf, sizeof(buf));
        // задержать на время 1 сек;
        sleep(1);
    }
    std::cout << '\n' << "Пишущий поток закончил работу." << '\n';
    pthread_exit((void*)1);
}   

int main() {
    pthread_t readId;
    pthread_t writeId;

    // создать неименованный канал;
    int rv = pipe(filedes);

    // создать поток из функции потока 1;
    int readFlag = 0;
    int readThread = pthread_create(&readId, NULL, readProc, &readFlag);

    // создать поток из функции потока 2;
    int writeFlag = 0;
    int writeThread = pthread_create(&writeId, NULL, writeProc, &writeFlag);

    // ждать нажатия клавиши;
    std::cout << '\n' << "Программа ждет нажатия клавиши." << '\n';
    getchar();
    std::cout << '\n' << "Клавиша нажата." << '\n';

    // установить флаг завершения потока 1;
    readFlag = 1;

    // установить флаг завершения потока 2;
    writeFlag = 1;

    // ждать завершения потока 1;
    int* exitcodeOfReadThread;
    pthread_join(readId, (void**)&exitcodeOfReadThread);
    std::cout << "exitcodeOfReadThread = " << exitcodeOfReadThread << '\n';

    // ждать завершения потока 2;
    int* exitcodeOfWriteThread;
    pthread_join(writeId, (void**)&exitcodeOfWriteThread);
    std::cout << "exitcodeOfWriteThread = " << exitcodeOfWriteThread << '\n';

    // закрыть неименованный канал (две операции);
    close(filedes[0]);
    close(filedes[1]);
    std::cout << '\n' << "Работа программы завершена." << '\n';
}
