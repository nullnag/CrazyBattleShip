#ifndef MAINMENU
#define MAINMENU

#include <QWidget>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QInputDialog>
#include "gamefield.h"

// Класс для создания главного меню из которого можно ввести имя пользователя и начать игру
class MainMenu : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = nullptr);

    // Метод, вызываемый при нажатии кнопки "ОК"
    void pressedOkButton();

    // Метод, вызываемый при нажатии кнопки "Играть"
    void pressedPlayButton();

private:
    GameField *field;            // Указатель на объект игрового поля
    QLineEdit *usernameLineEdit; // Указатель на поле ввода имени пользователя
    QString username;            // Имя игрока
    QString enemy;               // Имя противника
    QVBoxLayout *layout;         // Макет для размещения виджетов по вертикали

    // Метод для обработки полученных данных от сервера
    void handleData(const QString& data);

    // Метод для проверки имени пользователя
    void CheckUserName();
signals:
};

#endif // MAINMENU
