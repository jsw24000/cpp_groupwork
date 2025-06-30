#include "mainwindow.h"
#include "myserver.h"
#include <QApplication>
#include <QHostAddress>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 创建主窗口
    MainWindow window;
    window.show();

    // 显示初始化信息
    window.appendLog("应用程序启动");
    window.appendLog("正在加载帖子总数...");

    // 加载帖子总数
    MyServer::loadTotalPostNumber();
    window.appendLog(QString("帖子总数加载完成，当前总数：%1").arg(MyServer::totalPostNumber));

    // 创建服务器
    window.appendLog("正在创建服务器实例...");
    MyServer server;

    // 连接服务器信号到主窗口槽
    QObject::connect(&server, &MyServer::logMessage, &window, &MainWindow::appendLog);
    QObject::connect(&server, &MyServer::clientConnected, &window, &MainWindow::onClientConnected);
    QObject::connect(&server, &MyServer::clientDisconnected, &window, &MainWindow::onClientDisconnected);
    QObject::connect(&server, &MyServer::requestReceived, &window, &MainWindow::onRequestReceived);

    // 启动服务器
    window.appendLog("正在启动TCP服务器...");
    if (server.listen(QHostAddress::Any, 1234)) {
        window.onServerStarted();
        qDebug() << "Server started";
    } else {
        window.onServerError(server.errorString());
        qDebug() << "Server error:" << server.errorString();
    }

    return a.exec();
}
