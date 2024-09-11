#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H
#include <QTcpSocket>
#include <QObject>
#include <QDebug>

// Класс для подключения к серверу и обработки приходящих данных
class SocketManager : public QObject{
    Q_OBJECT

public:
    // Функция, возвращающая единственный экземпляр класса
    static SocketManager& instance() {
        // Статический экземпляр класса, который создается один раз и используется всегда
        static SocketManager instance;
        return instance;
    }
    // Функция для подключения к серверу по указанному адресу и порту
    void connectToServer(const QString &host, quint16 port);

    // Функция, возвращающая указатель на сокет
    QTcpSocket* getSocket();

    // Функция для отправки данных на сервер
    void sendData(const QString &Data);

signals:
    // Сигнал, который будет испускаться при получении данных с сервера
    void dataReceived(const QString &data);
private slots:
    // Слот для чтения данных из сокета
    void readData();
private:
    // Приватный конструктор, чтобы предотвратить создание объекта извне
    SocketManager(QObject *parent = nullptr) : QObject(parent) {}

    QTcpSocket socket;     // Сокет для работы с сетью
};

#endif // SOCKETMANAGER_H
