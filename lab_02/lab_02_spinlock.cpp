#include <iostream>
#include <unistd.h>
#include <pthread.h>
// g++ lab1.cpp -lpthread for compile!!!
// -pthread

pthread_spinlock_t global_spin;

struct parametrsOfThreat {
    pthread_t id;
    int flag;
    char sym;
    
    parametrsOfThreat(int flag, char symbol) {
        flag = flag;
        sym = symbol;
    }
};

void* proc1(void* arg) {
    std::cout << '\n' << "Поток 1 начал работу." << '\n';
    parametrsOfThreat *curArgs = (parametrsOfThreat*) arg; 
    sleep(1);
    while (curArgs->flag != 1) {
            pthread_spin_lock(&global_spin);
            std::cout << '\n' << "spin захвачен 1-ым потоком." << '\n';
            for (int i = 0; i < 10; i++) {  
                putchar(curArgs->sym);
                fflush(stdout);
                sleep(1);
            }
            pthread_spin_unlock(&global_spin);
            std::cout << '\n' << "spin освобожден 1-ым потоком." << '\n';
            sleep(1);
        }
    std::cout << '\n' << "Поток 1 закончил работу." << '\n';
    return nullptr;
    // pthread_exit((void*)1);
}


void* proc2(void* arg) {
    parametrsOfThreat *curArgs = (parametrsOfThreat*) arg;
    std::cout << '\n' << "Поток 2 начал работу." << '\n';
    sleep(1);
    while (curArgs->flag != 1) {
            pthread_spin_lock(&global_spin);
            std::cout << '\n' << "spin захвачен 2-ым потоком." << '\n';
            for (int i = 0; i < 10; i++) {
                putchar(curArgs->sym);
                fflush(stdout);
                sleep(1);
            }
            pthread_spin_unlock(&global_spin);
            std::cout << '\n' << "spin освобожден 2-ым потоком." << '\n';
            sleep(1);
        }
    std::cout << '\n' << "Поток 2 закончил работу." << '\n';
    return nullptr;
    // pthread_exit((void*)2);
}


int main() {
    pthread_spin_init(&global_spin, 0);

    std::cout << '\n' << "Программа начала работу." << '\n';
    parametrsOfThreat arg1(0, '1');
    parametrsOfThreat arg2(0, '2');
    int thread1 = pthread_create(&arg1.id, NULL, proc1, &arg1);
    int thread2 = pthread_create(&arg2.id, NULL, proc2, &arg2);
    std::cout << '\n' << "Программа ждет нажатия клавиши." << '\n';
    getchar();
    std::cout << '\n' << "Клавиша нажата." << '\n';
    arg1.flag = 1;
    arg2.flag = 1;
    int* exitcodeOfThreat1;
    int* exitcodeOfThreat2;
    pthread_join(arg1.id, (void**)&exitcodeOfThreat1);
    std::cout << "exitcodeOfThreat1 = " << exitcodeOfThreat1 << '\n';
    pthread_join(arg2.id, (void**)&exitcodeOfThreat2);
    std::cout << "exitcodeOfThreat2 = " << exitcodeOfThreat2 << '\n';
    std::cout << '\n' << "Работа программы завершена." << '\n';
    pthread_spin_destroy(&global_spin);

}