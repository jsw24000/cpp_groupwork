#include "mainwindow.h"
#include "myserver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MyServer::setTotalPostNumber(0);
    MyServer server;
    if (server.listen(QHostAddress::Any, 1234)) {
        qDebug() << "Server started";
    } else {
        qDebug() << "Server error:" << server.errorString();
    }
    return a.exec();
}
