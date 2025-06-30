// myserver.h
#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "searchmanager.h"

class MyServer : public QTcpServer {
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = nullptr);
    ~MyServer();
    static int totalPostNumber;
    static void loadTotalPostNumber();
    static void saveTotalPostNumber();
    static void setTotalPostNumber(int value); //设置总帖子数，手动

    SearchManager* searchManager;
    void handleGetPost(QTcpSocket* socket, const QString& filename);
    void handleGetComments(QTcpSocket* socket, const QString& filename);
    void handlePostComment(QTcpSocket* socket, const QString& filename, const QString& username, const QString& content);
    void handleGetUserInfo(QTcpSocket* socket, const QString& username);
    void handleUpdateUserInfo(QTcpSocket* socket, const QString& username, const QJsonObject& userInfo);
    void handleSearchRequest(QTcpSocket* socket, const QString& keyword);
    void handleGetAllPosts(QTcpSocket* socket);
    void handleGetFavorites(QTcpSocket* socket, const QString& username);
    void handleSignUp(QTcpSocket* socket, const QString &username, const QByteArray &userInfoData);
    void handleLogin(QTcpSocket* socket, const QString &username, const QString &password);
    void handleFavoriteRequest(QTcpSocket* socket, const QString& filename, const QString& username);
    static void saveTextToFile(const QString& text);

signals:
    void logMessage(const QString &message);
    void clientConnected(const QString &clientInfo);
    void clientDisconnected(const QString &clientInfo);
    void requestReceived(const QString &requestInfo);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void handleClientData();
    void onClientDisconnected();

private:
    QList<QTcpSocket*> m_clients;
    QString getClientInfo(QTcpSocket* socket);
};

#endif // MYSERVER_H
