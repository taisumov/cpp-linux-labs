#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

void printSortedArray(int *array) {
    for(int i = 0; i < 10; ++i)
        cout << array[i] << ' ';
    cout << endl;
}

void processSorter(int connect_fd) {
    if(fork() == 0){
        int array[10];
        //sleep(5);
        recv(connect_fd, array, sizeof(array), 0);
        sort(array, array + 10);
        cout << "sorted: ";
        printSortedArray(array);

        send(connect_fd, array, sizeof(array) * 10, 0);

        close(connect_fd);
        exit(EXIT_SUCCESS);
    }
}

int main() {
    int socket_fd;
    // Открытие сокета
    // Эта операция выполняет построение сокета и возвращает его дескриптор.
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cout << "SOCKET CREATION ERROR!" << endl;
        return EXIT_FAILURE;
    }

    //struct sockaddr_in {
    //  short int sin_family;           // Тип домена – значение AF_INET
    //  unsigned short int sin_port;    // Номер порта в сетевом порядке байт,
    //                                     первые 1024 порта зарезервированы;
    //                                     если 0, то система самостоятельно выберет номер порта,
    //  struct in_addr sin_addr;        // IP-адрес в сетевом порядке байт
    //  unsigned char sin_zero[8];      // Дополнение до размера структуры sockaddr
    //};
    sockaddr_in socketAddress;
    socketAddress.sin_family= AF_INET;
    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(8080);

    //int bind(int sockfd, struct sockaddr *addr, int addrlen);
    //sockfd – дескриптор сокета
    //addr – указатель на структуру с адресом
    //addrlen = sizeof(addr) – длина структуры
    if(bind(socket_fd, (sockaddr*)&socketAddress, sizeof(sockaddr))) {
        return EXIT_FAILURE;
    }

    //перевод сокета в пассивное (слушающее) состояние и создание очередей для
    //порождаемых при установлении соединения присоединенных сокетов, находящихся в
    //состоянии не полностью установленного соединения и полностью установленного
    //соединения.
    if(listen(socket_fd, 1) < 0) {
        return EXIT_FAILURE;
    }

    // Задаём время ожидания
    timeval timeOut = {5};

    for(;;) {
        fd_set socket_set;

        //обнуляем наборы дескрипторов
        FD_ZERO(&socket_set);
        FD_SET(socket_fd, &socket_set);

        // n – кол-во опрашиваемых дескрипторов сокетов(на единицу больше самого
        //большого номера описателей из всех наборов), если все FD_SETSIZE
        //
        // readfds, writefds, exceptfds наборы дескрипторов, которые следует проверять,
        //соответственно, на готовность к чтению, записи и на наличие
        //исключительных ситуаций.
        //
        // timeout- время ожидания (верхняя граница времени, которое пройдет перед
        //возвратом из select), если 0, то процесс будет приостановлен до тех пор, пока
        //один из сокетов не изменит свое состояние
        if(select(socket_fd+1, &socket_set, nullptr, nullptr, &timeOut) > 0) {
            int connect_fd,
                sockSize = sizeof(socketAddress);

            //accept используется сервером, ориентированным на
            //установление связи путем виртуального соединения, для приема полностью
            //установленного соединения .
            if((connect_fd = accept(socket_fd, (sockaddr*)&socketAddress, (socklen_t *)&sockSize)) < 0) {
                return EXIT_FAILURE;
            }
            processSorter(connect_fd);
        }
        else {
            cout << "TIME OUT, CLOSING PROGRAM..." << endl;
            break;
        }
        timeOut.tv_sec = 10;

    }
    close(socket_fd);
    return 0;
}