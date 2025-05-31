#include "mainwindow.h"
#include "login.h"
#include <client.h>

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AddPostDialog::loadTotalPostNumber();
    Login l;
    l.show();
    // 启动时加载上次保存的值

    // AddPostDialog::setTotalPostNumber(0);
    // MainWindow w;
    // w.show();
    return a.exec();
}
