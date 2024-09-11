#ifndef GAME_H
#define GAME_H
#include <QVector>
#include <QMessageBox>
#include <QPushButton>
#include <QObject>
#include "SocketManager.h"

// Класс служит для изменения стадии игры
class Game : public QObject{
    Q_OBJECT
public:
    explicit Game(QObject *parent = nullptr);
    int GameState = 0; // Стадия игры
private:
    // Функция для чтения приходящих данных с сервера
    void handleData(const QString &data);
};
#endif // GAME_H
