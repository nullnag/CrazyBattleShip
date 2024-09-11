#include "server.h"

Server::Server() {
    sessionManager = new SessionManager();
    if (this->listen(QHostAddress::Any, 5555)){
        qDebug() << "listen 5555";
    }
    else{
        qDebug() << "error 5555";
    }
}

void Server::incomingConnection(qintptr socketDescriptor){
    clientSock = new QTcpSocket;
    clientSock->setSocketDescriptor(socketDescriptor);
    connect(clientSock,&QTcpSocket::readyRead,this, &Server::sockReady);
    connect(clientSock,&QTcpSocket::disconnected,this,&Server::sockDisc);
    qDebug() << socketDescriptor << " connected";
}

void Server::initializePlayers(const QString& stringData,QString& player1, QString& player2){
    player1.clear();
    player2.clear();
    for (int i = 0; i < stringData.size(); i++){
        if (stringData[i] == '|'){
            i++;
            for (; i < stringData.size();i++){
                player2 += stringData[i];
            }
            break;
        }
        player1 += stringData[i];
    }
}

void Server::sockReady(){
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    QString player1 = "";
    QString player2 = "";
    if (clientSocket){
        QByteArray data = clientSocket->readAll();
        if (data.isEmpty()){
            clientSocket->write("E:Такой логин уже есть");
            return;
        }
        if (data[0] == '0'){
            QString stringData = QString::fromUtf8(data);
            initializePlayers(stringData, player1,player2);
            if (usersOnline[player2] == nullptr){
                qDebug() << "Error";
                return;
            }
            usersOnline[player2]->write("Reject");
            return;
        }
        if (data[0] == '1'){
            data.remove(0,1);
            QString stringData = QString::fromUtf8(data);
            qDebug() << "Message: " << data;
            if (stringData.size() == 0){
                clientSocket->write("E:Такой логин уже есть");
                clientSocket->flush();
                return;
            }
            if (usersOnline.contains(stringData)){
                clientSocket->write("E:Такой логин уже есть");
                clientSocket->flush();
                return;
            }
            clientSocket->write("S:");
            clientSocket->flush();
            usersOnline[stringData] = clientSocket;
        }
        if (data[0] == '2'){
            data.remove(0,1);
            QString stringData = QString::fromUtf8(data);
            initializePlayers(stringData, player1,player2);
            qDebug() << "Вызов " << data;
            if (player1 == player2){
                clientSocket->write("N:Пользователя нет в сети");
            }
            if (usersOnline.contains(player1)){
                QString response = "B:" + player2;
                usersOnline[player1]->write(response.toUtf8());
            }
            else{
                clientSocket->write("N:Пользователя нет в сети");
            }
            return;
        }
        if (data[0] == '3'){
            data.remove(0,1);
            QString stringData = QString::fromUtf8(data);
            initializePlayers(stringData, player1,player2);
            if (sessionManager->getSession(clientSocket)){
                clientSocket->write("E:Игроки уже в игре");
                return;
            }
            sessionManager->addSession(player1,player2,usersOnline[player1],usersOnline[player2]);
            usersOnline[player1]->write("Play");
            usersOnline[player2]->write("Play");
            return;
        }
        if (data[0] == '4'){
            data.remove(0,1);
            QString stringData = QString::fromUtf8(data);
            Session* session = sessionManager->getSession(clientSocket);
            int x = data[0] - '0';
            int y = data[1] - '0';
            if (session->getGameState() == 0){
                session->placeShip(x,y, clientSocket);
            }
            else if (session->getGameState() == 1){
                session->makeMove(x,y);
                if (session->getGameState() == 3){
                    sessionManager->removeSession(session);
                }
            }
            return;
        }
    }
}

bool Server::hasSession(QTcpSocket* disconnectedSocket){
    Session* session = sessionManager->getSession(disconnectedSocket);
    if (session == nullptr) {
        return false;
    }

    if (disconnectedSocket == session->player1Sock) {
        if (session->player2Sock) {
            session->player2Sock->write("D:Player Disconnected from session");
        }
    }
    if (disconnectedSocket == session->player2Sock) {
        if (session->player1Sock) {
            session->player1Sock->write("D:Player Disconnected from session");
        }

    }
    sessionManager->removeSession(session);
    return true;
}

void Server::sockDisc(){
    qDebug() << "disc";
    QTcpSocket* disconnectedSocket = qobject_cast<QTcpSocket*>(sender());
    if (disconnectedSocket) {
        qDebug() << "Disconnected socket:" << disconnectedSocket;
        hasSession(disconnectedSocket);
        for (auto it = usersOnline.begin(); it != usersOnline.end();) {
            if (it.value() == disconnectedSocket){
                it = usersOnline.erase(it);
            }
            else{
                ++it;
            }
        }
        disconnectedSocket->deleteLater();

    } else {
        qDebug() << "Error: Sender is not a QTcpSocket.";
    }
}

void Server::sendToClient(int request, QString string){
    Data.clear();
    qDebug() << "Sended" << "\n" << string;
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version());
    out << string;
}

Session::Session(QString player1,QString player2, QTcpSocket* player1Sock, QTcpSocket* player2Sock){
    this->player1 = player1;
    this->player2 = player2;
    this->player1Sock = player1Sock;
    this->player2Sock = player2Sock;
    player1Field = QVector<QVector<int>>(10,QVector<int>(10,0));
    player2Field = QVector<QVector<int>>(10,QVector<int>(10,0));
    playersFields[player1Sock] = player1Field;
    playersFields[player2Sock] = player2Field;
    playerShips[player1Sock] = 10;
    playerShips[player2Sock] = 10;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Session::updateTimer);
    timer->start(1000);
}

int Session::gameOver(){
    bool player1ShipsRemaining = false;
    bool player2ShipsRemaining = false;

    // Проверяем поле игрока 1
    for (const auto& row : playersFields[player1Sock]) {
        if (row.contains(1)) { // Если есть хотя бы одна часть корабля
            player1ShipsRemaining = true;
            break; // Выходим из цикла, если нашли корабль
        }
    }

    // Проверяем поле игрока 2
    for (const auto& row : playersFields[player2Sock]) {
        if (row.contains(1)) { // Если есть хотя бы одна часть корабля
            player2ShipsRemaining = true;
            break; // Выходим из цикла, если нашли корабль
        }
    }

    // Определяем победителя

    if (!player1ShipsRemaining) {
        // Если у игрока 1 не осталось кораблей, игрок 2 победил
        gameState = 3;
        QString winningPlayer = "Win player: " + player2;
        player1Sock->write(winningPlayer.toUtf8());
        player2Sock->write(winningPlayer.toUtf8());
    } else if (!player2ShipsRemaining) {
        // Если у игрока 2 не осталось кораблей, игрок 1 победил
        gameState = 3;
        QString winningPlayer = "Win player: " + player1;
        player1Sock->write(winningPlayer.toUtf8());
        player2Sock->write(winningPlayer.toUtf8());
    }

    return gameState; // Можно возвращать состояние игры или 0 для окончания
}

void Session::makeMove(const int& x,const int& y){
    QVector<QVector<int>>& field = (playersTurn == 0) ? playersFields[player2Sock] : playersFields[player1Sock];
    QTcpSocket* currentPlayerSock = (playersTurn == 0) ? player1Sock : player2Sock;
    QTcpSocket* otherPlayerSock = (playersTurn == 0) ? player2Sock : player1Sock;

    QString messagePlayerTurn;
    if (field[x][y] == 1) {
        field[x][y] = 2;
        QByteArray HitMessage = "Hit " + QString::number(x).toUtf8() + " " + QString::number(y).toUtf8() + "\n ";
        currentPlayerSock->write(HitMessage);
        otherPlayerSock->write(HitMessage);
        currentPlayerSock->flush();
        otherPlayerSock->flush();
    } else {
        QByteArray MissMessage = "Miss " + QString::number(x).toUtf8() + " " + QString::number(y).toUtf8() + "\n ";
        currentPlayerSock->write(MissMessage);
        otherPlayerSock->write(MissMessage);
        currentPlayerSock->flush();
        otherPlayerSock->flush();

        // Меняем ход
        playersTurn = (playersTurn == 0) ? 1 : 0;
        messagePlayerTurn = "turn " + ((playersTurn == 0) ? player1 : player2);
        player1Sock->write(messagePlayerTurn.toUtf8() + "\n");
        player2Sock->write(messagePlayerTurn.toUtf8() + "\n");
    }
    qDebug() << messagePlayerTurn;
    currentPlayerSock->flush();
    otherPlayerSock->flush();
    gameOver();
}

void Session::placeShip(const int& x,const int& y, QTcpSocket *clientSocket){
    QVector<QVector<int>>& field = playersFields[clientSocket];
    int &RemainingShips = playerShips[clientSocket];
    if (field[x][y] == 1){
        field[x][y] = 0;
        RemainingShips++;
        clientSocket->write("R" + QString::number(RemainingShips).toUtf8());
        return;
    }
    if (RemainingShips == 0){
        clientSocket->write("Warning");
        field[x][y] = 1;
        return;
    }
    field[x][y] = 1;
    RemainingShips--;
    clientSocket->write("R" + QString::number(RemainingShips).toUtf8());
    return;
}

void Session::updateTimer(){
    if (remainingTime == 0){
        timer->stop();
        gameState = 1;
        player1Sock->write("GameStateChange");
        player2Sock->write("GameStateChange");
        player1Sock->flush();
        player2Sock->flush();
        QString messagePlayerTurn =  " turn " + player1;
        player1Sock->write(messagePlayerTurn.toUtf8());
        player2Sock->write(messagePlayerTurn.toUtf8());
        return;
    }
    remainingTime--;
    QByteArray timeData = "T" + QString::number(remainingTime).toUtf8();
    qDebug() << timeData;
    player1Sock->write(timeData);
    player2Sock->write(timeData);
}

void SessionManager::addSession(const QString &player1, const QString &player2, QTcpSocket *player1Sock, QTcpSocket *player2Sock)
{
    Session* newSession = new Session(player1, player2, player1Sock, player2Sock);
    activeSessions[player1 + player2] = newSession;
    sessions.append(newSession);
}

Session* SessionManager::getSession(QTcpSocket* clientSocket)
{
    for (Session* session : sessions) {  // Предположим, что sessions - это список всех сессий
        if (session->player1Sock == clientSocket || session->player2Sock == clientSocket) {
            return session;
        }
    }
    return nullptr;
}

void SessionManager::removeSession(Session* session)
{
    sessions.removeOne(session);
    delete session;
}



