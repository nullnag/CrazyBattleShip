#include "MainMenu.h"
#include "SocketManager.h"

MainMenu::MainMenu(QWidget *parent)
    : QWidget{parent}
{
    //Установка свойств окна, кнопок и полей ввода
    this->setFixedSize(200, 100);
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    QPushButton *OkButton = new QPushButton("OK",this);
    layout = new QVBoxLayout(this);
    usernameLineEdit = new QLineEdit(this);
    usernameLineEdit->setPlaceholderText("Введите имя пользователя");
    layout->addWidget(OkButton);
    layout->addWidget(usernameLineEdit);
    setLayout(layout);
    connect(OkButton,&QPushButton::clicked,this,&MainMenu::pressedOkButton); // Сигнал о нажатии кнопки
    connect(usernameLineEdit, &QLineEdit::returnPressed,this,&MainMenu::pressedOkButton); // Сигнал о нажитии Enter
    connect(&SocketManager::instance(), &SocketManager::dataReceived, this, &MainMenu::handleData); // Сигнал о том, что данные пришли
}

void MainMenu::pressedOkButton(){
    SocketManager::instance().sendData("1"+usernameLineEdit->text()); // Отправляет на сервер запрос 1 с именем пользователя
}

void MainMenu::handleData(const QString &data){
    qDebug() << "Recieved data:" << data;
    if (data[0] == 'E'){ // Е - Такой логин существует либо неверные данные.
        QMessageBox::information(nullptr,"Информация","Такой логин уже существуте",QMessageBox::Ok);
    }
    else if (data[0] == 'N') { // N - Пользователь не в сети либо игрок пытается вызвать самого себя на бой
        QMessageBox::information(nullptr, "Информация", "Пользователь не в сети", QMessageBox::Ok);
    }
    else if (data == "Reject"){ // Reject - Противник отказался от игры
        QMessageBox::information(nullptr, "Информация", "Игрок отказался от игры", QMessageBox::Ok);
    }
    else if (data[0] == 'B'){ // B - Противник согласился на игру
        enemy = ""; // Очищаем имя противника, если начнёться другая игра
        for (int i = 2;i < data.size();i++){
            enemy += data[i]; // Добавляем имя противника
        }
        QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Информация", "Вас вызывает на бой игрок: " + enemy, QMessageBox::Yes | QMessageBox::No); // Вызывается окно принять бой или отклонить
        if (reply == QMessageBox::Yes){
            SocketManager::instance().sendData("3"+username+"|"+enemy); // Если принемаем, отправляем на сервер запрос 3 с именем игрока и противника
        }
        else{
            SocketManager::instance().sendData("0"+username+"|"+enemy); // Если отказываемся, отправляем на сервер запрос 0 с именем игрока и противника
        }
    }
    else if (data == "Play"){ // Play - Запускает игру
        field = new GameField(nullptr,username,enemy); // Создание объекта игрового поля
        this->hide(); // Скрываем главное меню
        field->show(); // Показываем игровое поле
    }
    else if (data[0] == 'D'){ // Игрок вышел из игры
        QMessageBox::information(nullptr, "Информация", "Игрок покинул игру", QMessageBox::Ok);
        this->show(); // Показываем главное меню
        delete field; // Удаляем объект игрового поля
    }
    else if (data[0] == 'S'){ // S - Успешный вход
        username = usernameLineEdit->text(); // Записываем имя игрока из поля ввода
        QLayoutItem *item; // Переменная для скрытия старых кнопок
        while((item = layout->takeAt(0))){
            delete item->widget();
            delete item; // Удаляем не нужный кнопки
        }
        QPushButton *PlayButton = new QPushButton("Играть",this); // Создаем новые кнопки для игры и выхода
        QPushButton *ExitButton = new QPushButton("Выход",this);
        layout->addWidget(PlayButton); //Добавляем в главное окно
        layout->addWidget(ExitButton);
        connect(PlayButton, &QPushButton::clicked,this,&MainMenu::pressedPlayButton);
        connect(ExitButton, &QPushButton::clicked,this,&QCoreApplication::quit);
    }
    QStringList messages = data.split("\n", Qt::SkipEmptyParts); // Разделяем приходящие данные с сервера
    // Нужно для того чтобы определить кто победил в игре и после этого вывести главное меню и удалить игровое поле //
    for (const QString& message : messages) {
        if (data == "Win player: " + username){
            QMessageBox::information(nullptr, "Информация", "Вы выйграли", QMessageBox::Ok);
            this->show();
            delete field;
        }
        if (data == "Win player: " + enemy){
            QMessageBox::information(nullptr, "Информация", "Вы проиграли", QMessageBox::Ok);
            this->show();
            delete field;
        }
    }
}

//Функция при нажатии кнопки играть
void MainMenu::pressedPlayButton(){
    bool ok;
    enemy = QInputDialog::getText(this,"Введите имя противника", "Имя противника",QLineEdit::Normal, "", &ok); // Вводим имя противника
    if (ok && !enemy.isEmpty()) {
        SocketManager::instance().sendData("2"+enemy+"|"+username); // Отправляем на сервер запрос 2 о создании игры
        QMessageBox::information(this, "Ждём ответа от", "Игрока: " + enemy);
    } else if (ok) {
        // Если пользователь нажал "OK", но строка пустая
        QMessageBox::warning(this, "Ошибка", "Вы не ввели строку.");
    }
}


