#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

void printSortedArray(int *array) {
    for(int i = 0; i < 10; ++i)
        cout << array[i] << ' ';
    cout << endl;
}

int main() {
    int socket_fd = 1;

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

    time_t  endTimeToConnect,
            startTimeToConnect = time(nullptr);
    int isConnected;
    // за 5 секунд пытаемся подключиться
    do {
        isConnected = connect(socket_fd, (sockaddr*)&socketAddress, sizeof(socketAddress));
        endTimeToConnect = time(nullptr);
        if((endTimeToConnect - startTimeToConnect) > 4)
            break;
    }while(isConnected < 0);

    // если не получилось подключиться, выходим из программы
    if(isConnected == -1) {
        cout << "CONNECT ERROR!" << endl;
        return EXIT_FAILURE;
    }

    // создаем массив
    int* array = new(int[10]);
    srand(time(0));
    for(int i = 0; i < 10; ++i)
        array[i] = rand() % 15;

    // отправляем массив на сервер
    send(socket_fd, array, 40, 0);
    cout << "sent: ";
    printSortedArray(array);

    fd_set socket_set;

    // обнуляем наборы дескрипторов
    FD_ZERO(&socket_set);
    FD_SET(socket_fd, &socket_set);

    // задаем таймер в 1 секунду
    timeval timeOut = {1};

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

        recv(socket_fd, array, 40, 0);
        cout << "received: ";
        printSortedArray(array);

    }
    else {
        cout << "TIME OUT, CLOSING PROGRAM..." << endl;
    }

    close(socket_fd);
    return 0;
}
