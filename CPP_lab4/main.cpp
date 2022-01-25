/**
 * Ислам Тайсумов, группа 8308
 *
 * Компиляция программы:
 * 1. g++ main.cpp -lpthread
 * 2. ./a.out
**/

#include <pthread.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sched.h>

using namespace std;

//структура, облегчающая работу с файловым потоком
struct fileStream
{
    fstream file;
    string filename;
};

//потоковая функция
void* threadFunc(void* thread_data){
    fstream& streamFile = static_cast<fileStream*>(thread_data)->file;
    string   stringName = static_cast<fileStream*>(thread_data)->filename;
    string   temp;

    //sched_setscheduler, sched_getscheduler - устанавливает или получает алгоритм планировщика (и его параметры)
    int classPriority = sched_getscheduler(getpid()),
        policy;

    cout << "++==================++" << endl;
    cout << "|| SCHEDULE POLICY: ||" << endl;
    cout << "++==================++" << endl;

    //предварительно устаналиваем алгоритм планирования
    switch (classPriority){
        case 0:
            cout << "||    SCHED_FIFO    ||" << endl;
            cout << "++==================++" << endl;
            policy = SCHED_FIFO;
            break;
        case 1:
            cout << "||     SCHED_RR     ||" << endl;
            cout << "++==================++" << endl;
            policy = SCHED_RR;
            break;
        case 2:
            cout << "||   SCHED_OTHER    ||" << endl;
            cout << "++==================++" << endl;
            policy = SCHED_OTHER;
            break;
        default:
            break;
    }

    //Структура, описывающая параметры планирования
    struct sched_param param;

    //Получение в поле int sched_priority структуры
    //статического приоритета процесса.
    sched_getparam(getpid(), &param);

    //Получение максимального (минимального) разрешённого значения
    //статического приоритета для класса планирования.
    int minPriority     = sched_get_priority_min(policy),
        maxPriority     = sched_get_priority_max(policy),
        currentPriority = param.sched_priority;

    cout << "|| CUR PRIORITY: " << currentPriority  << (currentPriority/10==0 ? " " : "") << " ||" << endl;
    cout << "|| MIN PRIORITY: " << minPriority      << (minPriority/10==0 ? " " : "") << " ||" << endl;
    cout << "|| MAX PRIORITY: " << maxPriority      << (maxPriority/10==0 ? " " : "") << " ||" << endl;

    //сообщаем об ошибке при открытии файла
    if (!streamFile) {
        cout << "ERROR OPENING FILE!!!";
        exit(1);
    }

    //открываем файл на чтение
    streamFile.open(stringName);

    //построчно выводим информацию из файла
    cout << "++==================++" << endl;
    cout << "||    FILE DATA:    ||" << endl;
    cout << "++==================++" << endl;

    //выводим данные из файла
    while (getline(streamFile, temp)) {
        cout << temp << endl ;
    }

    //закрываем файл (по заданию для проверки можно закомментировать следующую строку)
    streamFile.close();

    //завершаем поток
    pthread_exit(0);
}

int main(){
    //задаём структуру и имя входного файла, а также создаем указатель на структуру
    fileStream fileS;

    fileS.filename    = "/home/taisumov/CLionProjects/CPP_lab4/1.txt";
    void* thread_data = &fileS;

    //создаем идентификатор потока
    pthread_t thread;

    //создаем поток по идентификатору thread и функции потока threadFunc
    //и передаем потоку указатель на данные thread_data
    pthread_create(&thread, NULL, threadFunc, thread_data);

    //ждем завершения потока
    pthread_join(thread, NULL);

    //проверяем, открыт ли файл
    if (fileS.file.is_open()){
        cout << endl
             << "++=============================================++" << endl
             << "||  FILE IS STILL OPENED, WE'RE CLOSING IT...  ||" << endl
             << "++=============================================++" << endl;
        fileS.file.close();
    } else{
        cout << endl
             << "++===========================++" << endl
             << "||  FILE IS ALREADY CLOSED!  ||" << endl
             << "++===========================++" << endl;
    }

    return 0;
}