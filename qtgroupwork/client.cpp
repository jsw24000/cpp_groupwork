#include "client.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

Client::Client(QObject *parent) : QObject(parent), m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &Client::onError);
}

// client.cpp
Client::~Client()
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QTcpSocket::UnconnectedState) {
            m_socket->waitForDisconnected();
        }
    }
}

bool Client::connectToServer(const QString &host, quint16 port)
{
    m_socket->connectToHost(host, port);
    return m_socket->waitForConnected();
}

//通用的发送给请求方式
void Client::sendRequest(const QByteArray &request)
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->write(request);
        m_socket->waitForBytesWritten();
    }
}

// 请求获取评论
void Client::requestComments(const QString& filename)
{
    QJsonObject requestObj;
    requestObj["type"] = "GET_COMMENTS";
    requestObj["filename"] = filename;
    QJsonDocument doc(requestObj);
    sendRequest(doc.toJson());
}

// 发送新评论
void Client::postComment(const QString& filename, const QString& username, const QString& content)
{
    QJsonObject requestObj;
    requestObj["type"] = "POST_COMMENT";
    requestObj["filename"] = filename;
    requestObj["username"] = username;
    requestObj["content"] = content;
    QJsonDocument doc(requestObj);
    sendRequest(doc.toJson());
}

//请求用户信息
void Client::requestUserInfo(const QString& username)
{
    QJsonObject requestObj;
    requestObj["type"] = "GET_USERINFO";
    requestObj["username"] = username;
    QJsonDocument doc(requestObj);
    sendRequest(doc.toJson());
}

//请求修改用户信息
void Client::updateUserInfo(const QString& username, const QJsonObject& userInfo)
{
    QJsonObject requestObj;
    requestObj["type"] = "UPDATE_USERINFO";
    requestObj["username"] = username;
    requestObj["userInfo"] = userInfo;
    QJsonDocument doc(requestObj);
    sendRequest(doc.toJson());
}

//请求收藏
void Client::requestFavorites(const QString& username)
{
    QJsonObject requestObj;
    requestObj["type"] = "GET_FAVORITES";
    requestObj["username"] = username;
    QJsonDocument doc(requestObj);
    QByteArray request = doc.toJson(QJsonDocument::Compact);
    sendRequest(request);
}

//请求所有帖子
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

void Client::sendSignUpRequest(const QString &username, const QString &password)
{
    QJsonObject requestObj;
    requestObj["type"] = "SIGNUP";
    requestObj["username"] = username;
    requestObj["password"] = password;
    QJsonDocument doc(requestObj);
    QByteArray request = doc.toJson(QJsonDocument::Compact);
    sendRequest(request);
}

void Client::sendLoginRequest(const QString &username, const QString &password)
{
    QJsonObject requestObj;
    requestObj["type"] = "LOGIN";
    requestObj["username"] = username;
    requestObj["password"] = password;
    QJsonDocument doc(requestObj);
    QByteArray request = doc.toJson(QJsonDocument::Compact);
    sendRequest(request);
}

//槽函数
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

    // 尝试解析可能的JSON信息头
    QJsonDocument jsonDoc;
    bool hasJsonHeader = false;
    int newlineIndex = data.indexOf('\n');

    if (newlineIndex != -1) {
        QByteArray jsonPart = data.left(newlineIndex);
        QJsonParseError parseError;
        jsonDoc = QJsonDocument::fromJson(jsonPart, &parseError);
        if (parseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
            hasJsonHeader = true;
            // 移除JSON信息头
            data = data.mid(newlineIndex + 1);
        }
    }

    QString responseStr = QString::fromUtf8(data);

    // 处理简单响应
    if (responseStr == "Success") {
        emit dataReceived(data);
        return;
    } else if (responseStr == "SignUpSuccess") {
        emit dataReceived(data);
        return;
    } else if (responseStr == "LoginSuccess") {
        emit dataReceived(data);
        return;
    } else if (responseStr.startsWith("FavoriteSuccess")) {
        emit dataReceived(data);
        return;
    } else if (responseStr.startsWith("GetFavoritesFailed") || responseStr.startsWith("FavoriteFailed")) {
        emit dataReceived(data);
        return;
    }

    // 处理帖子数据
    QList<Post> posts;
    QStringList postBlocks = responseStr.split("\n===\n", Qt::SkipEmptyParts);

    for (const QString& postBlock : postBlocks) {
        QStringList lines = postBlock.split("\n", Qt::KeepEmptyParts);
        QString fileName;
        QString content;
        bool inContent = false;

        for (const QString& line : lines) {
            if (line.startsWith("FileName: ")) {
                fileName = line.mid(10);
            } else if (line.startsWith("Content-Length: ")) {
                // 可以忽略或用于验证
                continue;
            } else if (line == "---") {
                inContent = true;
                continue;
            } else if (inContent) {
                content += line + "\n";
            }
        }

        if (!fileName.isEmpty() && !content.isEmpty()) {
            posts.append(Post(fileName, content.trimmed()));
        }
    }

    // 判断是收藏帖子还是普通帖子
    if (hasJsonHeader && jsonDoc.object().contains("type") && jsonDoc.object()["type"].toString() == "GET_FAVORITES") {
        emit favoritesReceived(posts);
    } else {
        emit postsReceived(posts);
    }
}

void Client::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << m_socket->errorString();
    emit errorOccurred(socketError); // 发射 errorOccurred 信号
}
