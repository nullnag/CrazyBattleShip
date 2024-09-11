#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QMainWindow>
#include <QGridLayout>
#include "ClickableLabel.h"
#include <QTimer>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class GameField;
}
QT_END_NAMESPACE

// Класс GameField управляет главным игровым окном и полями игрока и противника
class GameField : public QMainWindow
{
    Q_OBJECT

public:
    // Конструктор: инициализирует игровое поле и принимает имя игрока и противника
    GameField(QWidget *parent = nullptr, QString username = "", QString enemy = "");

    ~GameField();
     // Инициализирует сетку игрового поля для игрока и противника
    void initializeGrid(QGridLayout* playerGrid, QGridLayout* enemyGrid);

     // Создаёт отдельную ячейку сетки по координатам x, y
    ClickableLabel *createCell(int x, int y);
private slots:
    // Обрабатывает данные, полученные от сервера
    void handleData(const QString &data);

    // Включает или отключает взаимодействие с сеткой в зависимости от состояния игры
    void setGridEnabled(QGridLayout* Grid, bool enabled, int gameState);

    // Обрабатывает результат выстрела противника по координатам x, y
    void onShotResultOpponent(int x, int y, const QString &result);

    // Обрабатывает результат выстрела игрока по координатам x, y
    void onShotResult(int x, int y, const QString &result);
private:
    bool isPlayerTurn = false; // Флаг, указывающий, ход ли игрока
    QString username;          // Имя игрока
    QString enemy;             // Имя противника
    QGridLayout* enemyGrid;    // Сетка игрового поля противника
    QGridLayout* playerGrid;   // Сетка игрового поля игрока
    QVector<QVector<ClickableLabel*>> enemyCells; // Ячейки поля противника
    QVector<QVector<ClickableLabel*>> playerCells; // Ячейки поля игрока
    QLabel *Timerlabel;        // Лейбл для таймера
    QWidget* centralWidget;    // Центральный виджет
    QHBoxLayout *mainLayout;   // Основная компоновка
    QVBoxLayout *downLayout;   // Нижняя компоновка
    QLabel* enemyTurnLabel;    // Лейбл, указывающий, что ход противника
    QLabel* yourTurnLabel;     // Лейбл, указывающий, что ход игрока
    QLabel* RemainingShipsLabel; // Лейбл с оставшимися кораблями
    Ui::GameField *ui;         // Интерфейс пользователя
};
#endif // GAMEFIELD_H
