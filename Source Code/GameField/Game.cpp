#include "Game.h"

Game::Game(QObject *parent) : QObject(parent) {
    GameState = 0;
    connect(&SocketManager::instance(), &SocketManager::dataReceived, this, &Game::handleData);
}

void Game::handleData(const QString &data){
    if(data == "GameStateChange"){ // Если сервер отправил "GameStateChange"
        GameState = 1; // Изменяем стадию игры
    }
}

