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

    bool connectToServer(const QString &host, quint16 port);
    void sendRequest(const QByteArray &request);
    void requestAllPosts();
    void sendPost(const QString& content);

    QTcpSocket *m_socket;
signals:
    void dataReceived(const QByteArray &data);
    void postsReceived(const QList<Post>& posts);
    void errorOccurred(QAbstractSocket::SocketError socketError);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:

};

#endif // CLIENT_H

