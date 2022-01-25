/**
 * Ислам Тайсумов, группа 8308
 *
 * Компиляция программы:
 * 1. g++ -o main main.cpp
 * 2. g++ -o firstChild firstChild.cpp
 * 3. g++ -o secondChild secondChild.cpp
 * 4. ./main
 *
**/

#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>

using namespace std;

int main(int argc, char* argv[]) {

    // Тип данных sigset_t служит для представления набора сигналов
    sigset_t set;

    // Делает пустым набор сигналов, на который указывает set
    // (никаких сигналов в set представлено не будет)
    // RETURN INT
    sigemptyset(&set);

    // Добавляет сигнал signo в набор set
    // RETURN INT
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGQUIT);

    // sigprocmask используется для того, чтобы изменить
    // список блокированных в данный момент сигналов
    //
    // SIG_BLOCK:
    // Набор блокируемых сигналов - объединение текущего набора и аргумента set.
    //
    // Если значение поля oldset не равно нулю, то предыдущее значение маски сигналов
    // записывается в oldset.
    sigprocmask(SIG_BLOCK,
                &set,
                nullptr);

    // Массив для открытия каналов
    int arrayForPipes[2];

    // Открытие каналов
    if (pipe(arrayForPipes) == -1) {
        exit(EXIT_FAILURE);
    }

    // Запрет блокировки чтения или записи
    fcntl(arrayForPipes[0], F_SETFL, O_NONBLOCK);

    int firstProcess = fork();
    int secondProcess;

    if(firstProcess != 0)
        secondProcess = fork();

    if (firstProcess == 0) {
        // Закрытие канала и запуск первой программы
        close(arrayForPipes[1]);
        execl("./firstChild", "./firstChild", &arrayForPipes[0], nullptr);

    } else if (secondProcess == 0) {
        // Закрытие канала и запуск второй программы
        close(arrayForPipes[1]);
        execl("./secondChild", "./secondChild", &arrayForPipes[0], nullptr);

    } else {
        ifstream fileStream("data.txt");

        if (fileStream.is_open()) {
            close(arrayForPipes[0]);

            string bufferString;

            while(getline(fileStream, bufferString)){
                write(arrayForPipes[1], &bufferString[0], bufferString.size());
                sleep(2);
            }
            fileStream.close();

            kill(0, SIGQUIT);

            waitpid(firstProcess,
                    nullptr,
                    0);
            waitpid(secondProcess,
                    nullptr,
                    0);
            close(arrayForPipes[1]);

        }
    }

    return 0;
}