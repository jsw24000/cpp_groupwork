#include "mainwindow.h"
#include <client.h>

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    qDebug() << "Current working directory: " << QDir::currentPath();
    QApplication a(argc, argv);
    // 启动时加载上次保存的值/*
    // AddPostDialog::loadTotalPostNumber();
    // // AddPostDialog::setTotalPostNumber(0);

    // Client client;
    // client.connectToServer("192.168.43.242", 1234); // 连接到本地服务器的 1234 端口

    MainWindow w;
    w.show();
    return a.exec();
}
