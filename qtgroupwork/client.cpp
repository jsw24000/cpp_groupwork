#include "client.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

Client::Client(QObject *parent) : QObject(parent), m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &Client::onError);
}

Client::~Client()
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected();
    }
}

bool Client::connectToServer(const QString &host, quint16 port)
{
    m_socket->connectToHost(host, port);
    return m_socket->waitForConnected();
}

void Client::sendRequest(const QByteArray &request)
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->write(request);
        m_socket->waitForBytesWritten();
    }
}

void Client::requestAllPosts()
{
    QByteArray request = "GetAllPosts";
    sendRequest(request);
}

void Client::sendPost(const QString& content)
{
    QJsonObject postObj;
    postObj["type"] = "POST";
    postObj["content"] = content;
    QJsonDocument doc(postObj);
    QByteArray request = doc.toJson(QJsonDocument::Compact);
    sendRequest(request);
}

void Client::onConnected()
{
    qDebug() << "Connected to server";
}

void Client::onDisconnected()
{
    qDebug() << "Disconnected from server";
}

void Client::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    qDebug() << "Received from server:" << QString(data);

    QList<Post> posts;
    QString responseStr = QString::fromUtf8(data);
    QStringList postBlocks = responseStr.split("\n===\n", Qt::SkipEmptyParts);
    for (const QString& postBlock : postBlocks) {
        QStringList lines = postBlock.split("\n", Qt::KeepEmptyParts);
        QString fileName;
        QString content;
        for (const QString& line : lines) {
            if (line.startsWith("FileName: ")) {
                fileName = line.mid(10);
            } else if (line.startsWith("Content-Length: ")) {
                // 这里如果需要根据内容长度做更精确处理可进一步编写逻辑，目前先略过
                continue;
            } else if (line == "---") {
                continue;
            } else {
                content += line + "\n";
            }
        }
        if (!fileName.isEmpty() &&!content.isEmpty()) {
            posts.append(Post(fileName, content));
        }
    }
    emit postsReceived(posts);
}

void Client::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << m_socket->errorString();
    emit errorOccurred(socketError); // 发射 errorOccurred 信号
}
