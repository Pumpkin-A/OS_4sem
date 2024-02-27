#include <iostream>
#include <unistd.h>

int main(int count, char* const* args) {
    std::cout << "Дочерний процесс начал работу." << '\n';

    std::cout << "Id дочернего процесса: " << getpid() << '\n';
    std::cout << "Id процесса-родителя дочернего процесса: " << getppid() << '\n';
    for (int i = 0; i < count; i++){
        std::cout << "Вывод аргумента: " << args[i] << '\n';
        sleep(1);
    }
    int current_proc = getpid();
    int parent_proc = getppid();
    std::cout << current_proc << ' ' << parent_proc << '\n';

    std::cout << "Дочерний процесс закончил работу." << '\n';
    exit(1);
}
