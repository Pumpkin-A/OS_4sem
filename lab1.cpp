#include <iostream>
#include <unistd.h>
#include <pthread.h>
// g++ lab1.cpp -lpthread for compile!!!

typedef struct {
    int flag;
    char sym;
}parametrs;


void* proc1(void* arg) {
    std::cout << '\n' << "Поток 1 начал работу." << '\n';
    parametrs *curArgs = (parametrs*) arg; //передаваемые параметры //приведем к типу targs
    sleep(1);
    while (curArgs->flag != 1) {
        putchar(curArgs->sym);
        fflush(stdout);
        sleep(1);
    }
    std::cout << '\n' << "Поток 1 закончил работу." << '\n';
    pthread_exit((void*)1);
}


void* proc2(void* arg) {
    std::cout << '\n' << "Поток 2 начал работу." << '\n';
    parametrs *curArgs = (parametrs*) arg;
    sleep(1);
    while (curArgs->flag != 1) {
        putchar(curArgs->sym);
        fflush(stdout);
        sleep(1);
    }
    std::cout << '\n' << "Поток 2 закончил работу." << '\n';
    pthread_exit((void*)2);
}


int main() {
    std::cout << '\n' << "Программа начала работу." << '\n';
    parametrs arg1;
    parametrs arg2;
    arg1.flag = 0;
    arg2.flag = 0;
    arg1.sym = '1';
    arg2.sym = '2';
    pthread_t id1;
    pthread_t id2;
    int* exitcodeOfThreat1;
    int* exitcodeOfThreat2;
    int thread1 = pthread_create(&id1, NULL, proc1, &arg1);
    int thread2 = pthread_create(&id2, NULL, proc2, &arg2);
    std::cout << '\n' << "Программа ждет нажатия клавиши." << '\n';
    getchar();
    std::cout << '\n' << "Клавиша нажата." << '\n';
    arg1.flag = 1;
    arg2.flag = 1;
    pthread_join(id1, (void**)&exitcodeOfThreat1);
    std::cout << "exitcodeOfThreat1 = " << exitcodeOfThreat1 << '\n';
    pthread_join(id2, (void**)&exitcodeOfThreat2);
    std::cout << "exitcodeOfThreat2 = " << exitcodeOfThreat2 << '\n';
    std::cout << '\n' << "Работа программы завершена." << '\n';

}