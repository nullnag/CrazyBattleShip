#include "SocketManager.h"

// Функция для подключения к серверу по указанному адресу и порту //
void SocketManager::connectToServer(const QString &host, quint16 port){
    socket.connectToHost(host,port);
    connect(&socket, &QTcpSocket::readyRead, this, &SocketManager::readData);
}

// Функция, возвращающая указатель на сокет //
QTcpSocket* SocketManager::getSocket(){
    return &socket;
}

//Слот для чтения данных из сокета//
void SocketManager::readData(){
    while (socket.bytesAvailable()) {
        QByteArray data = socket.readAll();
        QString receivedData = QString::fromUtf8(data);
        emit dataReceived(QString(receivedData));
    }
}

// Функция для отправки данных на сервер //
void SocketManager::sendData(const QString &Data){
    if (socket.state() == QAbstractSocket::ConnectedState) {
        socket.write(Data.toUtf8());
        socket.waitForBytesWritten();
        socket.flush();
    } else {
        qDebug() << "Не подключен к серверу!";
    }
}
