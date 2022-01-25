#include <iostream>
#include <fstream>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/sem.h>

using namespace std;

int main(int argc,char* argv[]){

    int delayTime           = atoi(argv[1]);
    int numReaderProcess    = atoi(argv[2]);
    int mainSemID;

    ifstream fileStream;
    string str;

    struct sembuf   operation1 = {0,-1,0},       // Проверка состояния писателя
                    operation2 = {0, 1,0},       // Возвращение к свободному состоянию писателя
                    operation3 = {1, 1,0},       // Вход в критическую секцию читателем
                    operation4 = {1,-1,0},       // Выход из критической секции читателем
                    operation5 = {2, 1,0},       // Семафор, который отвечает за добавление рабочих процессов + отвечает за завершения работы программы
                    operation6 = {2,-1,0};       // Семафор, который отвечает за уменьшение кол-ва рабочих процессов

    if (argc != 3){
        cout << "Нужно 2 аргумента!" << endl;
        return 1;
    }

    if ((mainSemID = semget(1810,
                           3,
                           0666 | IPC_CREAT | IPC_EXCL)) == -1){
        if ((mainSemID = semget(1810,
                               3,
                               IPC_CREAT)) == -1){
            cout << "ОШИБКА: Множественный семафор не подключён!" << endl;
            return 1;
        }
        else
            cout << "Множественный семафор был успешно подключен!" << endl;
    }
    else{
        cout << "Множественный семафор был успешно создан!" << endl;

        semctl(mainSemID, 0, SETVAL, 1);
        semctl(mainSemID, 1, SETVAL, 0);
        semctl(mainSemID, 2, SETVAL, 0);
    }

    semop(mainSemID, &operation5, 1);      // добавление рабочего процесса
    semop(mainSemID, &operation1, 1);      // проверка состояния писателя
    semop(mainSemID,&operation2,1);        // возвращает к свободному состоянию писателя
    semop(mainSemID, &operation3, 1);      // вход в критическую секцию читателем

    fileStream.open("output.txt");

    while(getline(fileStream, str) && !fileStream.eof()) {
        cout << "(ЧИТАТЕЛЬ " << numReaderProcess << ") прочитал: "<< str << endl;
        sleep(delayTime);
    }

    fileStream.close();

    semop(mainSemID, &operation4, 1);      // выход из критической секции
    semop(mainSemID, &operation6, 1);      // уменьшение количества рабочих процессов

    if (semctl(mainSemID, 2, GETVAL) == 0) {
        semctl (mainSemID, 0, IPC_RMID, 0);         //удаление семафора читателя
    }

    return 0;
}