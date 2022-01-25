#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>

using namespace std;

bool quit = false;

void quitSigAction(int quitSignal) {
    cout << "Second child: parent finished\n";
    quit = true;
}
int main(int argc,char* argv[]) {
    int sig;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);

    // sigaction используется для изменения действий процесса при получении соответствующего сигнала
    // struct sigaction {
    //     void (*sa_handler)(int);
    //     void (*sa_sigaction)(int, siginfo_t *, void *);
    //     sigset_t sa_mask;
    //     int sa_flags;
    //     void (*sa_restorer)(void);
    // }
    struct sigaction act;
    act.sa_handler = &quitSigAction;
    sigaction(SIGQUIT,
              &act,
              nullptr);

    sigset_t set_quit;
    sigaddset(&set_quit, SIGQUIT);

    // sigprocmask используется для того, чтобы изменить
    // список блокированных в данный момент сигналов
    //
    // SIG_UNBLOCK
    // Сигналы, устанавливаемое значение битов которых равно set, удаляются из списка блокируемых сигналов.
    // Допускается разблокировать незаблокированные сигналы.
    //
    // Если значение поля oldset не равно нулю, то предыдущее значение маски сигналов
    // записывается в oldset.
    sigprocmask(SIG_UNBLOCK,&set_quit,nullptr);

    ofstream file("secondChild.txt");

    char symb;
    sigwait(&set, &sig);

    if (file.is_open()) {
        int size;
        while(size != -1 || !quit) {
            size = read(*argv[1], &symb, 1);
            if(size != -1) {
                cout << "Second: " << symb << endl;
                file << symb;
                kill(0, SIGUSR1);
                sigwait(&set, &sig);
            }
        };

        //int kill(pid_t pid, int sig);
        //  pid > 0, сигнал sig посылается процессу с идентификатором pid.
        //  pid = 0, то sig посылается каждому процессу, который входит в группу текущего процесса.
        //  pid = -1, то sig посылается каждому процессу, за исключением процесса с номером 1 (init).
        //  pid < -1, то sig посылается каждому процессу, который входит в группу процесса -pid.
        //  sig = 0, то никакой сигнал не посылается, а только выполняется проверка на существования
        //           процесса или группы.
        kill(0, SIGUSR1);
        file.close();
        close(*argv[1]);
    }
    exit(EXIT_SUCCESS);
}