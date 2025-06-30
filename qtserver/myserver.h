// myserver.h
#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class MyServer : public QTcpServer {
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = nullptr);
    ~MyServer();
    static int totalPostNumber;
    static void loadTotalPostNumber();
    static void saveTotalPostNumber();
    static void setTotalPostNumber(int value); //设置总帖子数，手动

    void handleGetAllPosts(QTcpSocket* socket);
    void handleGetFavorites(QTcpSocket* socket, const QString& username);
    void handleSignUp(QTcpSocket* socket, const QString &username, const QString &password);
    void handleLogin(QTcpSocket* socket, const QString &username, const QString &password);
    void handleFavoriteRequest(QTcpSocket* socket, const QString& filename, const QString& username);
    static void saveTextToFile(const QString& text);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void handleClientData();

private:
    QList<QTcpSocket*> m_clients;
};

#endif // MYSERVER_H
