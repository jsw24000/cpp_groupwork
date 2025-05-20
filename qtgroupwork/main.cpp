#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 启动时加载上次保存的值
    AddPostDialog::loadTotalPostNumber();
    // AddPostDialog::setTotalPostNumber(0);
    MainWindow w;
    w.show();
    return a.exec();
}
