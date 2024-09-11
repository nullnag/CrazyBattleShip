#include "MainMenu.h"
#include "SocketManager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SocketManager::instance().connectToServer("26.103.198.212",5555); // Служит для подключения к серверу
    MainMenu main;
    main.show();
    return a.exec();
}
