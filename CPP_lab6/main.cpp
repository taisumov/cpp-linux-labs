/**
 * Ислам Тайсумов, группа 8308
 *
 * Компиляция программы:
 * 1. g++ -o main main.cpp
 * 2. ./main x y (x процессов, задержка - y секунд)
 *
**/

#include <iostream>
#include <sys/time.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>
#include <vector>

using namespace std;

//сюда записываем время работы каждого процесса
std::vector<string> timeOfChilds;

//функция, вызываемая порождающим процессом
void printTimeChild(int signal_num){

    //ограничиваем от комбинации CTRL+Z
    signal(SIGTSTP,SIG_IGN);

    //получаем время работы дочернего процесса
    time_t secondsChild = time(NULL);
    //переводим его в привычный формат
    tm* timeChild = localtime(&secondsChild);
    //добавляем в глобальный массив
    timeOfChilds.push_back(asctime(timeChild));

    pid_t pid = fork();
    if (!pid){          //если процесс создан, то:
        cout << "Child's PID: "  << getpid() << endl;
        cout << "Child's time: " << asctime(timeChild) << endl;
        exit(0);
    }
}

int main(int argc, char *argv[]){
    int numOfChilds = atoi(argv[1]);
    int timeToDelay = atoi(argv[2]);

    //ограничиваем от комбинации CTRL+Z
    signal(SIGTSTP,SIG_IGN);
    //при окончании таймера запускаем функцию
    signal(SIGALRM,printTimeChild);


    //    struct itimerval {
    //        struct timeval it_interval; /* следующее значение */
    //        struct timeval it_value;    /* текущее значение */
    //    };
    //    struct timeval {
    //        long tv_sec;                /* секунды */
    //        long tv_usec;               /* микросекунды */
    //    };

    const struct itimerval timer{timeToDelay, 0, timeToDelay, 0};
    struct       itimerval current_timer;

    for (int i = 0; i < numOfChilds; i++){
        if (setitimer(ITIMER_REAL, &timer, &current_timer) == -1){
            //ITIMER_REAL - уменьшается постоянно (в режиме реального времени)
            //              и подает сигнал SIGALRM, когда значение таймера становится равным 0.
            cout << "Error!" << endl;
        }
        pause();
    }

    time_t secondsParent = time(NULL);
    tm* timeParent = localtime(&secondsParent);
    usleep(500);
    cout << "--------------------------------------" << endl;
    cout << "Parent's time: " << asctime(timeParent) << endl;

    for(int i = 0; i < numOfChilds; i++) {
        cout << "#" << i+1 << " child's time: " << timeOfChilds[i] << endl;
    }

    return 0;
}