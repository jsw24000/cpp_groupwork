#include <QCoreApplication>
#include "myserver.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    MyServer server;
    //MyServer::setTotalPostNumber(0);
    if (server.listen(QHostAddress::Any, 1234)) {
        qDebug() << "Server started";
    } else {
        qDebug() << "Server error:" << server.errorString();
    }

    return a.exec();
}
