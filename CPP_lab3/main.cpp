/**
 * Ислам Тайсумов, группа 8308
 *
 * Компиляция программы:
 * 1. g++ -o main main.cpp
 * 2. g++ -o secondChild secondChild.cpp
 * 3. ./main a x y z
 *    ,где  a = имя выходного файла (в формате text.txt)
 *          x = задержка родительского процесса
 *          y = задержка первого дочернего процесса
 *          z = задержка второго дочернего процесса
 *
**/

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>

void printPidInfo(pid_t pid, std::string fileName, int num) {

    //Функция вывода информации
    std::ofstream file(fileName, std::ofstream::out | std::ofstream::app);
    std::string output = "";

    //Добавление заголовка к таблице с данными
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

    //Открываем файл как поток и печатаем данные
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

    //Переменные типа char*
    char* endPointer = nullptr;
    char* fileNameInput = argv[1];

    //Переменные типа long
    long delayParent = 0,
         delayFirstChild = 0,
         delaySecondChild = 0;

    //Переменные типа string
    std::string fileName = "./" + std::string(fileNameInput);

    //Переменные задержек
    delayParent         = std::strtol(argv[2], &endPointer, 10);
    delayFirstChild     = std::strtol(argv[3], &endPointer, 10);
    delaySecondChild    = std::strtol(argv[4], &endPointer, 10);

    //Переменные типа pid_t
    pid_t   pidParent = getpid();
    pid_t   pidFirstChild,
            pidSecondChild;

    //Вывод задержек
    std::ofstream file(fileName, std::ofstream::out | std::ofstream::app);
    if(file.is_open()) {
        file << "+----------------------------------+"  << std::endl;
        file << "|             ЗАДЕРЖКИ             |"  << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 1 | Задержка родителя            |"  << delayParent   << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 2 | Задержка первого потомка     |"  << delayFirstChild << std::endl;
        file << "+---+------------------------------+"  << std::endl;
        file << "| 3 | Задержка второго потомка     |"  << delaySecondChild << std::endl;
        file << "+----------------------------------+"  << std::endl;
        file.close();
    }

    //Создание потомка
    pidFirstChild = fork();

    if(getpid() == pidParent)           //Если данную программу выполняет родитель, то
        pidSecondChild = vfork();       //он создаёт ещё одного потомка.

    if(pidFirstChild == -1 || pidSecondChild == -1) {
        std::cout << "ОШИБКА СОЗДАНИЯ ПРОЦЕССА!" << std::endl;
        return EXIT_FAILURE;
    }
    if(pidFirstChild > 0 && pidSecondChild > 0) {

        //Ветвь родителя
        sleep(delayParent);

        //Выводим информацию
        printPidInfo(pidParent, fileName, 1);

    }
    if(pidFirstChild == 0) {

        //Ветвь первого потомка
        sleep(delayFirstChild);

        //Выводим информацию
        pid_t firstChildPid = getpid();
        printPidInfo(firstChildPid, fileName, 2);

        return EXIT_SUCCESS;
    }
    if (pidSecondChild == 0) {

        //Ветвь второго потомка
        execl("/home/taisumov/CLionProjects/CPP_lab3/secondChild",  //Перенаправляем второй дочерний процесс
               "/home/taisumov/CLionProjects/CPP_lab3/secondChild",  //на выполнение другой программы.
                    argv[4], fileNameInput, nullptr);

        return EXIT_SUCCESS;
    }
    /* if (pidFirstChild == 0 && pidSecondChild == 0) {
        std::cout << "???" << std::endl;
    } */

        return 0;
}