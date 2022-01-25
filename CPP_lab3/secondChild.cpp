#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>

void printPidInfo(pid_t pid, std::string fileName, int num) {

    std::ofstream file(fileName, std::ofstream::out | std::ofstream::app);
    std::string output = "";

    switch(num){
        case 1:
            output = "|       РОДИТЕЛЬСКИЙ ПРОЦЕСС       |";
            break;
        case 2:
            output = "|       ДОЧЕРНИЙ ПРОЦЕСС №1        |";
            break;
        case 3:
            output = "|       ДОЧЕРНИЙ ПРОЦЕСС №2        |";
            break;
        default:
            break;
    };

    if(file.is_open()) {
        file << "+----------------------------------+"  << std::endl;
        file <<                 output                  << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 1 | ID процесса                  | " << pid << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 2 | ID предка                    | " << getppid() << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 3 | ID сессии процесса           | " << getsid(pid) << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 4 | ID группы процессов          | " << getpgid(pid) << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 5 | Реальный ID пользователя     | " << getuid() << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 6 | Эффективный ID пользователя  | " << geteuid() << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 7 | Реальный групповой ID        | " << getgid() << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 8 | Эффективный групповой ID     | " << getegid() << std::endl;
        file << "+---+------------------------------+"  << std::endl;

        file.close();
    }
    else std::cout << "ПРОЦЕСС НЕ МОЖЕТ ОТКРЫТЬ ФАЙЛ!" << std::endl;
}

int main(int arc, char* argv[]) {

    char* endPointer    = nullptr;
    long delay          = std::strtol(argv[1], &endPointer, 10);
    char* fileNameInput = argv[2];
    std::string fileName(fileNameInput);

    sleep(delay);

    pid_t childPid = getpid();
    printPidInfo(childPid, fileName, 3);

    return EXIT_SUCCESS;
}