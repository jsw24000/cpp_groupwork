#include "server.h"
#include <QSqlQuery>
#include <QSqlError>

Server::Server(QObject *parent) : QTcpServer(parent)
{
    // 初始化数据库
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("chat.db");
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
    }
    // 创建用户表
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT)");
}

Server::~Server()
{
    db.close();
}

bool Server::start()
{
    if (!this->listen(QHostAddress::Any, 1234)) {
        qDebug() << "Server could not start!";
        return false;
    } else {
        qDebug() << "Server started!";
        return true;
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    clients.append(socket);
    connect(socket, &QTcpSocket::connected, this, &Server::onClientConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
}

void Server::onClientConnected()
{
    qDebug() << "Client connected!";
}

void Server::onClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        clients.removeOne(socket);
        socket->deleteLater();
    }
}

void Server::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        QByteArray data = socket->readAll();
        // 处理客户端消息
        for (QTcpSocket *client : clients) {
            if (client != socket) {
                client->write(data);
            }
        }
    }
}  