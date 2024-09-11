#include "MainMenu.h"
#include "SocketManager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SocketManager::instance().connectToServer("127.0.0.1",5555); // Служит для подключения к серверу
    MainMenu main;
    main.show();
    return a.exec();
}
