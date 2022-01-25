#include <iostream>
#include <fstream>
#include <sys/msg.h>

#define PID 3

using namespace std;

struct askMessage
{
    long mtype;
    int sender;
    int answer_queue;
    int ask_time;
};

struct answerMessage
{
    long mtype;
    int sender;
};

int main()
{
    bool isMainProcess = false;
    int doneProcesses=0;
    int localQueue;
    int commonQueue;
    askMessage ask;                     //отправка запроса
    askMessage askBuffer[2];            //полученные запросы
    int indexBuffer=0;                  //количество запросов в ожидании
    answerMessage answer;               //отправка ответов
    answerMessage access;               //получение разрешений
    int numOfGetAccess = 0;             //количество полученных
    int numOfSendAccess = 0;            //количество отправленных


    //создаем общую очередь
    commonQueue = msgget(777,
                         0606 | IPC_CREAT | IPC_EXCL);
    if(commonQueue != -1)
    {
        isMainProcess=true;
        cout << "Создана общая очередь" << endl;
    }
    else
    {//подключаемся если уже создана
        commonQueue = msgget(777,
                             0606 | IPC_CREAT);
        cout << "Подключен к общей очереди" << endl;
    }

    //создаем локальную очередь
    localQueue = msgget(IPC_PRIVATE,
                        0606 | IPC_CREAT);
    cout << "Создана локальная очередь" << endl;


    //инициализация запросов и занесение их в очередь
    ask.ask_time        = time(NULL);
    ask.mtype           = (PID) % 3 + 1;
    ask.sender          = PID;
    ask.answer_queue    = localQueue;

    msgsnd(commonQueue,
           &ask,
           sizeof(askMessage),
           0);

    ask.mtype = (PID + 1) % 3 + 1;

    msgsnd(commonQueue,
           &ask,
           sizeof(askMessage),
           0);

    //инициализация ответа
    answer.mtype    = 1;
    answer.sender   = PID;


    //ожидание двух разрешений на файл (проверка общей очереди)
    while(numOfGetAccess < 2)
    {
        if(msgrcv(commonQueue,
                  &askBuffer[indexBuffer],
                  sizeof(askMessage),
                  PID,
                  IPC_NOWAIT) != -1)
        {//проверкa запросов в общей очереди для этой програмы
            cout << "Получен запрос доступа от "    << askBuffer[indexBuffer].sender    << endl;
            cout << "Время запроса: "               << askBuffer[indexBuffer].ask_time  << endl;
            if(askBuffer[indexBuffer].ask_time  < ask.ask_time ||
               (askBuffer[indexBuffer].ask_time == ask.ask_time &&
                askBuffer[indexBuffer].sender < PID))
            {//приоритет младшего и меньший индификатор
                msgsnd(askBuffer[indexBuffer].answer_queue,
                       &answer,
                       sizeof(answerMessage),
                       0);
                ++numOfSendAccess;
                cout << "Выдача разрешения процессу " << askBuffer[indexBuffer].sender << endl;
            }
            else//запомнить в буфере если старше
                ++indexBuffer;
        }

        if(msgrcv(localQueue,
                  &access,
                  sizeof(answerMessage),
                  1,
                  IPC_NOWAIT) != -1)
        {//проверка разрешений в локальной очереди
            ++numOfGetAccess;
            cout << "Получено разрешение на доступ от "     <<access.sender     << endl;
            cout << "Время получения: "                     <<time(NULL)  << endl;
        }
    }


    //вывод файла
    cout << "Начать вывод файла" << endl;
    ifstream fileStream("TEXT.txt");
    string str;

    while(getline(fileStream,str))
        cout << str << endl;
    fileStream.close();
    cout << "Конец файла. Файл закрыт." << endl;


    //выдача разрешений всем ожидающим
    while(indexBuffer > 0)
    {
        --indexBuffer;
        msgsnd(askBuffer[indexBuffer].answer_queue,
               &answer,
               sizeof(answerMessage),
               0);
        ++numOfSendAccess;
        cout << "Выдача разрешения процессу " << askBuffer[indexBuffer].sender << endl;
    }
    //если еще не все запросили доступ
    while(numOfSendAccess < 2)
    {//проверка запросов из общей очереди для этой программы
        if(msgrcv(commonQueue,
                  &askBuffer[indexBuffer],
                  sizeof(askMessage),
                  PID,
                  IPC_NOWAIT) != -1)
        {
            msgsnd(askBuffer[indexBuffer].answer_queue,
                   &answer,
                   sizeof(answerMessage),
                   0);
            ++numOfSendAccess;
            cout << "Выдача разрешения процессу " << askBuffer[indexBuffer].sender << endl;
        }
    }


    //отпрвака готовности завершения общей очереди
    ask.mtype = 0;
    msgsnd(commonQueue,
           &ask,
           sizeof(askMessage),
           0);
    //ожиание готовности остальных процессов
    if(isMainProcess)
    {
        while(doneProcesses < 3)
        {
            if(msgrcv(commonQueue,
                      &ask,
                      sizeof(askMessage),
                      0,
                      0) != -1)
                ++doneProcesses;
        }
        msgctl(commonQueue,
               IPC_RMID,
               0);
    }
    //удалене локальной очереди
    msgctl(localQueue,
           IPC_RMID,
           0);

    return 0;
}