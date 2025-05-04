#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(QObject *parent = nullptr);
    ~Server();
    bool start();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onClientConnected();
    void onClientDisconnected();
    void onReadyRead();

private:
    QList<QTcpSocket*> clients;
    QSqlDatabase db;
};
#endif // SERVER_H