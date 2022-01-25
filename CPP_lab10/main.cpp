#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <fstream>
#include <string>

using namespace std;

int main(int argv, char** argc) {
    if (argv < 2)
        return -1;

    if (string(argc[1]) == "0") {
        // создаем писательский семафор
        sem_t* writerSemafor = sem_open("writerSemafor", O_CREAT, 0666, 1);
        int iterNum = 7;
        ofstream file;

        if (!writerSemafor)
            return -1;

        while(--iterNum) {
            sem_wait(writerSemafor);

            file.open("output.txt", ios::app);
            file << "Writing to the file... " << getpid() << " " << iterNum << endl;
            cout << "Writing to the file... " << getpid() << " " << iterNum << endl;
            file.close();

            sem_post(writerSemafor);
            sleep((rand()+2)%2);
        }

    }
    else {
        sem_t* readerSemafor = sem_open("readerSemafor", O_CREAT, 0666, 1);
        sem_t* writerSemafor;
        ifstream fileStream("output.txt");
        string bufferString;
        int counter;

        if (!readerSemafor)
            return -1;

        sem_t* countSemafor = sem_open("countSemafor", O_CREAT, 0666, 0);
        if (!countSemafor)
            return -1;

        do {
            writerSemafor = sem_open("writerSemafor", 0);
        }while(!writerSemafor);

        while(!fileStream.eof()) {
            sem_wait(readerSemafor); // блокируем других читателей
            sem_post(countSemafor);
            sem_getvalue(countSemafor, &counter);

            if (counter == 1)
                sem_wait(writerSemafor); // блокируем файл

            sem_post(readerSemafor);

            getline(fileStream, bufferString); //читаем строку
            cout << bufferString << endl;

            sem_wait(readerSemafor); // блокируем других читателей
            sem_wait(countSemafor);

            sem_getvalue(countSemafor, &counter);
            if(counter == 0)
                sem_post(writerSemafor); // снимаем блокировку файла

            sem_post(readerSemafor);
            sleep(1);
        }
        fileStream.close();

    }
    return 0;
}
