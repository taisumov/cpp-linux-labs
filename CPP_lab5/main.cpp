/**
 * Ислам Тайсумов, группа 8308
 *
 * Компиляция программы:
 * 1. g++ -o main main.cpp
 * 2.1 ./main 1 (для деления на ноль)
 * 2.2 ./main 2 (для ошибки памяти)
**/

#include <iostream>
#include <signal.h>

using namespace std;

void catchError(int err) {
    cout << "Ошибка: ";
    switch(err){
        case SIGFPE:
            cout << "при делении на ноль!" << endl;
            exit(1);
            break;
        case SIGSEGV:
            cout << "при выделении памяти!" << endl;
            exit(2);
            break;
        default:
            break;
    }
}

int main(int arc, char* argv[]) {
    //SIGFPE(8) - неверная операция (переполнение, деление на 0)
    signal(SIGFPE, catchError);
    //SIGSEGV(11) - нарушение защиты памяти
    signal(SIGSEGV, catchError);

    int numOfError = atoi(argv[1]);
    int err;
    char* c = "Hello";

    switch (numOfError) {
        case 1:
            err = 228 / 0;
            cout << "Деление успешно завершено!";
            break;
        case 2:
            c[10] = 'z';
            cout << "Преобразование строки произошло успешно!";
            break;
        default:
            break;
    }

    cout << "all OK";

    return 0;
}
