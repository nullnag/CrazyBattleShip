#include "gamefield.h"
#include "./ui_gamefield.h"
#include "SocketManager.h"

GameField::GameField(QWidget *parent, QString username, QString enemy)
    : QMainWindow(parent)
    , ui(new Ui::GameField)
    , username(username)
    , enemy(enemy)
{
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    connect(&SocketManager::instance(), &SocketManager::dataReceived, this, &GameField::handleData);
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QVBoxLayout* mainVerticalLayout = new QVBoxLayout(centralWidget); // Основной макет

    mainLayout = new QHBoxLayout();  // Основной горизонтальный макет
    downLayout = new QVBoxLayout();  // Создаем вертикальный макет для размещения под игровыми полями

    // Виджет для поля игра
    QWidget* playerFieldWidget = new QWidget(centralWidget);
    playerGrid = new QGridLayout(playerFieldWidget);
    playerGrid->setSpacing(5);
    playerFieldWidget->setLayout(playerGrid);

    // Виджет для вражеского поля
    QWidget* enemyFieldWidget = new QWidget(centralWidget);
    enemyGrid = new QGridLayout(enemyFieldWidget);
    enemyGrid->setSpacing(5);
    enemyFieldWidget->setLayout(enemyGrid);

    // Виджет таймера
    Timerlabel = new QLabel("00", centralWidget);
    QFont font = Timerlabel->font();
    font.setPointSize(48);
    Timerlabel->setFont(font);
    Timerlabel->setAlignment(Qt::AlignCenter);
    Timerlabel->setFixedSize(100, 100);

    // Добавляем поля и таймер в основной горизонтальный макет
    mainLayout->addWidget(playerFieldWidget);
    mainLayout->addWidget(Timerlabel, 0, Qt::AlignCenter);
    mainLayout->addWidget(enemyFieldWidget);

    // Инициализация сеток
    initializeGrid(playerGrid, enemyGrid);
    setGridEnabled(enemyGrid, 0,1); // Выключаем сетку противника перед началом игры

    // Добавляем основной макет в главный вертикальный макет
    mainVerticalLayout->addLayout(mainLayout);

    // Лейблы "Ваш ход" и "Ход противника"
    yourTurnLabel = new QLabel("", centralWidget);
    enemyTurnLabel = new QLabel("", centralWidget);
    QFont turnLabelFont = yourTurnLabel->font();
    turnLabelFont.setPointSize(20);
    yourTurnLabel->setFont(turnLabelFont);
    enemyTurnLabel->setFont(turnLabelFont);
    yourTurnLabel->setAlignment(Qt::AlignCenter);
    enemyTurnLabel->setAlignment(Qt::AlignCenter);

    // Лейбл оставшихся кораблей
    RemainingShipsLabel = new QLabel("Осталось кораблей: 10",centralWidget);
    RemainingShipsLabel->setFont(turnLabelFont);
    RemainingShipsLabel->setFont(turnLabelFont);

    // Добавляем метки "Ваш ход" , "Ход противника" и "Оставшиеся корабли" в нижний макет
    downLayout->addWidget(yourTurnLabel);
    downLayout->addWidget(enemyTurnLabel);
    downLayout->addWidget(RemainingShipsLabel);

    // Добавляем нижний макет под игровыми полями
    mainVerticalLayout->addLayout(downLayout);
}
//Функция вызывается если приходят данные с сервера//
void GameField::handleData(const QString &data){
    // D - Игрок покинул игру, T - Обновление таймера, R - Оставшиеся корабли //
    // Если первый символ пришедших данных начинается с этих флагов, то выполнять следующие действия
    if (data[0] == 'D'){
        QMessageBox::information(nullptr, "Информация", "Игрок покинул игру", QMessageBox::Ok); // Оповестить игрока, что противник покинул игру
    }
    if (data[0] == 'T'){
        QString timestampStr = data.mid(1);
        Timerlabel->setText(timestampStr); // Изменить секунды в таймеры приходящие с сервера
        if (timestampStr == "0"){ // Если время вышло, то открыть поле противника и закрыть поле игрока
            QTimer::singleShot(2000, this, [=]() { // Задержка в 2 секунды
                setGridEnabled(playerGrid, 0, 1);
            });
            // Затем через 4 секунды включаем сетку врага
            QTimer::singleShot(4000, this, [=]() { // Задержка в 4 секунды
                setGridEnabled(enemyGrid, 1, 1);
            });
        }
    }
    if (data[0] == 'R'){ // Изменить значение лейбла оставшихся кораблей
        QString RemainingShips = data.mid(1);
        if (RemainingShips.toInt() > 0){
            setGridEnabled(playerGrid, 1, 0); // Если корабли есть, то снимаем блокировку с поля игрока
        }
        else{
            setGridEnabled(playerGrid, 0, 0); // Если кораблей не осталось блокируем поле игрока
            QMessageBox::information(nullptr, "Информация", "У вас не осталось кораблей", QMessageBox::Ok); // Оповестить игрока если не осталось кораблей
        }
        RemainingShipsLabel->setText("Осталось кораблей: " + RemainingShips); // Изменить лейбл оставшихся кораблей
        downLayout->addWidget(RemainingShipsLabel, 0, Qt::AlignCenter);
    }
    QStringList messages = data.split("\n", Qt::SkipEmptyParts); // Используется для разделения приходящих сообщений от сервера
    for (const QString& message : messages) {
        QString trimmedMessage = message.trimmed();
        qDebug() << "Message: " << trimmedMessage;
        if (trimmedMessage  == "turn " + username || trimmedMessage  == "turn " + enemy) { // Если в сообщениие от сервера был описан ход игрока или противника
            QFont font = yourTurnLabel->font(); // Изменяем лейб хода, чтобы вывести чей сейчас ход //
            yourTurnLabel->setFont(font);
            yourTurnLabel->setAlignment(Qt::AlignCenter);
            enemyTurnLabel->setFont(font);
            enemyTurnLabel->setAlignment(Qt::AlignCenter);
            downLayout->addWidget(enemyTurnLabel, 0, Qt::AlignCenter);
            downLayout->addWidget(yourTurnLabel, 0, Qt::AlignCenter);
            QString currentTurnPlayer = trimmedMessage.mid(5); // Убираем не нужные символы и записываем в переменную ход игрока
            if (currentTurnPlayer == username) { // Если текущий ход принадлежит игроку
                setGridEnabled(enemyGrid, true,1); // Разблокировать поле противника
                yourTurnLabel->setText("Ваш ход"); // Изменить лейбл текущего хода
                enemyTurnLabel->setText("");
                isPlayerTurn = true;               // Изменяем переменную хода игрока
                qDebug() << "It's your turn!";
            } else if (currentTurnPlayer == enemy) { // Если текущий ход пренадлежит противнику
                setGridEnabled(enemyGrid, false,1); // Отключаем поле противника
                enemyTurnLabel->setText("Ход игрока " + enemy); // Изменить лейбл текущего хода
                yourTurnLabel->setText("");
                isPlayerTurn = false;               // Изменяем переменную хода игрока
                qDebug() << "Waiting for enemy turn.";
            }
            return;
        }
        QStringList parts = data.split(" ");  // Предположим, что данные приходят в формате "Hit x y" или "Miss x y"
        if (parts[0] == "Hit" || parts[0] == "Miss") {
            int x = parts[1].toInt();  // Координата x
            int y = parts[2].toInt();  // Координата y
            if (isPlayerTurn) { // Если ход игрока
                onShotResult(x, y, parts[0]);  // Обновляем поле противника
            } else { // Если ход противника
                onShotResultOpponent(x, y, parts[0]);  // Обновляем свое поле
            }
        }
    }
}

//Функция вызывается если происходит ход противника//
void GameField::onShotResultOpponent(int x, int y, const QString &result) {
    // Обновляем клетку на поле игрока по координатам x, y
    ClickableLabel* cell = playerCells[x][y];  // Получаем ссылку на нужную клетку
    if (cell) {  // Проверяем, что клетка существует
        if (result == "Hit") {
            cell->setStyleSheet("background-color: green;");  // Попадание
        } else if (result == "Miss") {
            cell->setStyleSheet("background-color: red;");  // Промах
        }
        cell->setEnabled(false);  // Отключаем клетку, чтобы нельзя было по ней снова нажать
        qDebug() << "Updated player's cell at (" << x << "," << y << ") with result:" << result;
    } else {
        qDebug() << "Player cell at (" << x << "," << y << ") not found!";
    }
}
//Функция вызывается если происходит наш ход //
void GameField::onShotResult(int x, int y, const QString &result) {
    // Обновляем клетку по координатам x, y в сетке
    ClickableLabel* cell = enemyCells[x][y];  // Получаем ссылку на нужную клетку
    if (result == "Hit") {
        cell->setStyleSheet("background-color: green;");  // Попадание
    } else if (result == "Miss") {
        cell->setStyleSheet("background-color: red;");  // Промах
    }
    cell->setEnabled(false);  // Отключаем клетку, чтобы нельзя было по ней снова нажать
    qDebug() << "Updated cell at (" << x << "," << y << ") with result:" << result;
}

//Функция вызывается при создании объекта//
void GameField::initializeGrid(QGridLayout* playerGrid, QGridLayout* enemyGrid){
    enemyCells.resize(10); // Инициализируем клетки противника, чтобы к ним можно было обращаться по координатам
    for (int i = 0; i < 10; ++i) {
        enemyCells[i].resize(10);
    }
    playerCells.resize(10); // Инициализируем клетки игрока, чтобы к ним можно было обращаться по координатам
    for (int i = 0; i < 10; ++i) {
        playerCells[i].resize(10);
    }
    for (int i = 0; i < 10; ++i) { // Отображаем поле игрока
        for (int j = 0; j < 10; ++j) {
            ClickableLabel *cell = createCell(i, j);
            playerGrid->addWidget(cell, i, j);
            playerCells[i][j] = cell; // Заполняем вложенный список игрока
        }
    }
    for (int i = 0; i < 10; ++i) { // Отображаем поле противника
        for (int j = 0; j < 10; ++j) {
            ClickableLabel *enemyCell = createCell(i, j);
            enemyGrid->addWidget(enemyCell, i, j);
            enemyCells[i][j] = enemyCell; // Заполняем вложенный список врага
        }
    }
}

//Функция вызывается при передаче хода либо при стадии установки кораблей//
void GameField::setGridEnabled(QGridLayout* Grid, bool enabled, int gameState) {
    for (int i = 0; i < Grid->rowCount(); ++i) {
        for (int j = 0; j < Grid->columnCount(); ++j) {
            QWidget *widget = Grid->itemAtPosition(i, j)->widget(); // Ссылка на клетку
            if (gameState == 0){ // Если стадия игры установка кораблей
                if (widget->property("occupied").toBool()){ // Если клетка занята, то её не блокируем.
                    continue;
                }
            }
            if (widget) {
                widget->setEnabled(enabled); // Блокируем клетки
            }
        }
    }
}

//Функция вызывается при запуске функции initializeGrid(QGridLayout* playerGrid, QGridLayout* enemyGrid)//
// Создаёт новый объект клетки и устанавливает свойста затем возвращает клетку
ClickableLabel *GameField::createCell(int x, int y){
    ClickableLabel* cell = new ClickableLabel(this);
    cell->setFixedSize(50,50);
    cell->setScaledContents(true);
    cell->setStyleSheet("border: 1px solid black; padding: 0px; margin: 0px;");
    cell->setContentsMargins(0, 0, 0, 0);
    cell->setProperty("cellX", x);
    cell->setProperty("cellY", y);
    return cell;
}

GameField::~GameField()
{
    delete ui;
}
