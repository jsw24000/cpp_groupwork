#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <post.h>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();
    bool isConnected() const {
        return m_socket->state() == QTcpSocket::ConnectedState;
    }
    bool connectToServer(const QString &host, quint16 port);
    void sendRequest(const QByteArray &request);
    void requestComments(const QString& filename);
    void postComment(const QString& filename, const QString& username, const QString& content);
    void requestUserInfo(const QString& username);
    void updateUserInfo(const QString& username, const QJsonObject& userInfo);
    void requestAllPosts();
    void requestFavorites(const QString& username);
    void sendPost(const QString& content);
    void sendSignUpRequest(const QString &username, const QString &password);
    void sendLoginRequest(const QString &username, const QString &password);

    QTcpSocket *m_socket;
signals:
    void dataReceived(const QByteArray &data);
    void userInfoReceived(const QJsonObject &userInfo);
    void postsReceived(const QList<Post>& posts);
    void favoritesReceived(const QList<Post>& posts);
    void errorOccurred(QAbstractSocket::SocketError socketError);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:

};

#endif // CLIENT_H

