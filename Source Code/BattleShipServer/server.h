#ifndef SERVER_H
#define SERVER_H
#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>
#include <QMap>
#include <QTimer>

// Класс Session управляет игровым сеансом (сессией) между двумя игроками в игре. Он хранит состояние игры, поля игроков и осуществляет взаимодействие с сетевыми сокетами игроков //
class Session : public QTcpServer{
public:
    Session(QString player1,QString player2, QTcpSocket* player1Sock, QTcpSocket* player2Sock);
    void updateTimer();
    int getGameState(){
        return gameState;
    }
    void placeShip(const int& x,const int& y,QTcpSocket *clientSocket);
    void makeMove(const int& x,const int& y);
    int gameOver();
    QTcpSocket* player1Sock;
    QTcpSocket* player2Sock;
private:
    int gameState = 0;
    bool playersTurn = 0;
    QVector<QVector<int>> player1Field;
    QVector<QVector<int>> player2Field;
    QString player1;
    QString player2;
    QMap<QTcpSocket*, QVector<QVector<int>>> playersFields;
    QMap<QTcpSocket*,int> playerShips;
    QTimer *timer;
    int remainingTime = 20;
};

// Класс SessionManager управляем всеми сессиями на сервере (Создаёт, удаляет).
class SessionManager{
public:
    void addSession(const QString& player1, const QString& player2, QTcpSocket* player1Sock, QTcpSocket* player2Sock);
    Session* getSession(QTcpSocket* clientSocket);
    void removeSession(Session* session);
private:
    QMap<QString,Session*> activeSessions;
    QList<Session*> sessions;
};

// Класс Server управляет сетевыми подключениями и логикой взаимодействия с клиентами.
class Server : public QTcpServer
{
public:
    Server();
protected:
    void incomingConnection(qintptr socketDescriptor) override;
    void sockReady();
    void sockDisc();
    void sendToClient(int request,QString string);
    void initializePlayers(const QString& stringData,QString& player1, QString& player2);
    bool hasSession(QTcpSocket* disconnectedSocket);
    SessionManager* sessionManager;
protected:
    QByteArray Data;
    QMap<QString,QTcpSocket*> usersOnline;
    QTcpSocket* clientSock;
};



#endif // SERVER_H
