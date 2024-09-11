#include "ClickableLabel.h"
ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {
    setAlignment(Qt::AlignCenter);

    // Инициализируем объект класса Game для взаимодействия с состоянием игры
    game = new Game();
}

// Деструктор для ClickableLabel
ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    // Получаем координаты ячейки из свойств метки
    x = this->property("cellX").toInt();
    y = this->property("cellY").toInt();
    // Формируем строку с позицией ячейки, которая отправляется на сервер. 4 - Запрос поставить корабль или выстрелить в зависимостри от стадии игры
    QString cellPos = "4" + QString::number(x) + QString::number(y);
    if (game->GameState == 0){ // Если стадия игры 0, устанавливаем корабли
        if(!property("occupied").toBool()){ // Если клетка не занята
            this->setStyleSheet("background-color: black;"); // Изменяем цвет на черный
            setProperty("occupied", true); // Присваиваем свойство клетки "Занято"
            SocketManager::instance().sendData(cellPos); // Отправляем координаты на сервер
        }
        else{ // Если клетка занята
            this->setStyleSheet("background-color: white; border: 1px solid black;"); // Изменяем цвет на белый
            setProperty("occupied", false); // Присваиваем свойство клетки "Не занято"
            SocketManager::instance().sendData(cellPos); // Отправляем координаты на сервер
        }
        return;
    }
    else if (game->GameState == 1){ // Если стадия игры уничтожение кораблей
        SocketManager::instance().sendData(cellPos);  // Отправляем координаты на сервер
    }
    qDebug() << "Clicked" << x << " "<< y;
    emit clicked(x, y);
}
