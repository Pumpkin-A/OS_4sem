#include <iostream>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

int main() {
    std::cout << "Программа начала работу." << '\n';

    std::cout << "Id процесса программы: " << getpid() << '\n';
    std::cout << "Id процесса-родителя программы: " << getppid() << '\n';
    pid_t pid =	fork();
    char *const args[6] = {"1", "2", "3", "4", "5", NULL};
    int res;
    if (pid == 0) {
        char buf[50];
        realpath("child", buf);
        int code = execv("child", args);
        if (code == -1)
            std::cout << "Ошибка exec: %s\n" << strerror(errno) << '\n';
    }
    else if (pid > 0) {
        int childExitCode = waitpid(pid, &res, WNOHANG);
        //родительский
        while (childExitCode == 0) { 
            std::cout << "Ожидание завершения дочернего процесса." << '\n';
            sleep(1);
            childExitCode = waitpid(pid, &res, WNOHANG);
        }
        if (childExitCode == -1)
            std::cout << "Error: %s\n" << strerror(errno) << '\n';
        else if (WIFEXITED(res))
            std::cout << "Код завершения дочернего процесса: " << WEXITSTATUS(res) << '\n';
        else if (WIFSIGNALED(res))
            std::cout << "Дочерний процесс был прерван сигналом " << WTERMSIG(res) << '\n';
        else
            std::cout << "Ошибка." << '\n';
    }
    else {
        perror("Error fork");
    }
    std::cout << "Программа закончила работу." << '\n';
};
