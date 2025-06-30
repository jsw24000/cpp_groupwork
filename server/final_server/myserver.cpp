#include "myserver.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

int MyServer::totalPostNumber = 0;

MyServer::MyServer(QObject *parent) : QTcpServer(parent) {
    searchManager = new SearchManager();
}

void MyServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    m_clients.append(socket);

    QString clientInfo = getClientInfo(socket);
    emit clientConnected(clientInfo);
    emit logMessage(QString("新客户端连接：%1").arg(clientInfo));

    connect(socket, &QTcpSocket::readyRead, this, &MyServer::handleClientData);
    connect(socket, &QTcpSocket::disconnected, this, &MyServer::onClientDisconnected);
}

MyServer::~MyServer() {
    saveTotalPostNumber();
    delete searchManager;
}

QString MyServer::getClientInfo(QTcpSocket* socket)
{
    return QString("%1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());
}

void MyServer::onClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        QString clientInfo = getClientInfo(socket);
        emit clientDisconnected(clientInfo);
        emit logMessage(QString("客户端断开连接：%1").arg(clientInfo));

        m_clients.removeOne(socket);
        socket->deleteLater();
    }
}

//处理监听到的请求
void MyServer::handleClientData() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QString clientInfo = getClientInfo(socket);
    emit logMessage(QString("收到来自 %1 的数据：%2").arg(clientInfo, QString(data).left(100)));

    //如果是想获取所有帖子的请求
    if (data == "GetAllPosts") {
        emit requestReceived(QString("获取所有帖子请求 - 来自：%1").arg(clientInfo));
        handleGetAllPosts(socket);
        return;
    }

    // 尝试解析 JSON 数据
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();
        emit logMessage("解析到JSON数据");

        if (obj.contains("type") && obj["type"].toString() == "POST" && obj.contains("content")) {
            //如果是存贴请求
            emit requestReceived(QString("发帖请求 - 来自：%1").arg(clientInfo));
            QString content = obj["content"].toString();
            saveTextToFile(content);
            QByteArray response = "Success";
            socket->write(response);
            socket->waitForBytesWritten();
            emit logMessage(QString("帖子保存成功，编号：%1").arg(totalPostNumber - 1));
            return;
        }
        //如果是收藏请求
        else if (obj.contains("type") && obj["type"].toString() == "FAVORITE" && obj.contains("filename") && obj.contains("username")) {
            QString filename = obj["filename"].toString();
            QString username = obj["username"].toString();
            emit requestReceived(QString("收藏请求 - 用户：%1，文件：%2").arg(username, filename));
            handleFavoriteRequest(socket, filename, username);
            return;
        }
        //如果是显示所有收藏的帖子的请求
        else if (obj.contains("type") && obj["type"].toString() == "GET_FAVORITES" && obj.contains("username")) {
            QString username = obj["username"].toString();
            emit requestReceived(QString("获取收藏请求 - 用户：%1").arg(username));
            handleGetFavorites(socket, username);
            return;
        }
        // 如果是搜索请求
        else if (obj.contains("type") && obj["type"].toString() == "SEARCH" && obj.contains("keyword")) {
            QString keyword = obj["keyword"].toString();
            emit requestReceived(QString("搜索请求 - 关键词：%1").arg(keyword));
            handleSearchRequest(socket, keyword);
            return;
        }
        //如果是请求获取或者修改用户信息
        else if (obj.contains("type") && obj["type"].toString() == "GET_USERINFO" && obj.contains("username")) {
            QString username = obj["username"].toString();
            emit requestReceived(QString("获取用户信息请求 - 用户：%1").arg(username));
            handleGetUserInfo(socket, username);
            return;
        }
        else if (obj.contains("type") && obj["type"].toString() == "UPDATE_USERINFO" && obj.contains("username")) {
            QJsonObject userInfo = obj["userInfo"].toObject();
            QString username = obj["username"].toString();
            emit requestReceived(QString("更新用户信息请求 - 用户：%1").arg(username));
            handleUpdateUserInfo(socket, username, userInfo);
            return;
        }
        //如果是有关评论的请求
        else if (obj.contains("type") && obj["type"].toString() == "GET_COMMENTS" && obj.contains("filename")) {
            QString filename = obj["filename"].toString();
            emit requestReceived(QString("获取评论请求 - 文件：%1").arg(filename));
            handleGetComments(socket, filename);
            return;
        }
        else if (obj.contains("type") && obj["type"].toString() == "POST_COMMENT" && obj.contains("filename") && obj.contains("username") && obj.contains("content")) {
            QString filename = obj["filename"].toString();
            QString username = obj["username"].toString();
            QString content = obj["content"].toString();
            emit requestReceived(QString("发表评论请求 - 用户：%1，文件：%2").arg(username, filename));
            handlePostComment(socket, filename, username, content);
            return;
        }
        //如果是登录或者注册请求
        else if (obj.contains("type")) {
            QString type = obj["type"].toString();
            if (type == "SIGNUP") {
                QString username = obj["username"].toString();
                QString password = obj["password"].toString();
                emit requestReceived(QString("注册请求 - 用户：%1").arg(username));
                QJsonObject userInfo;
                userInfo["password"] = password;
                userInfo["number"] = "";
                userInfo["school"] = "";
                userInfo["phone"] = "";
                QJsonDocument doc(userInfo);
                handleSignUp(socket, username, doc.toJson());
                return;
            } else if (type == "LOGIN") {
                QString username = obj["username"].toString();
                QString password = obj["password"].toString();
                emit requestReceived(QString("登录请求 - 用户：%1").arg(username));
                handleLogin(socket, username, password);
                return;
            }
        }
    } else {
        emit logMessage(QString("JSON解析错误：%1").arg(parseError.errorString()));
    }
}

//获取已有评论
void MyServer::handleGetComments(QTcpSocket* socket, const QString& filename)
{
    emit logMessage(QString("处理获取评论请求：%1").arg(filename));
    QByteArray response = "NoComments";
    socket->write(response);
    socket->waitForBytesWritten();
}

//储存评论
void MyServer::handlePostComment(QTcpSocket* socket, const QString& filename, const QString& username, const QString& content)
{
    emit logMessage(QString("处理发表评论请求：用户 %1 对文件 %2").arg(username, filename));
    QByteArray response = "CommentPosted";
    socket->write(response);
    socket->waitForBytesWritten();
}

//获取用户信息
void MyServer::handleGetUserInfo(QTcpSocket* socket, const QString& username)
{
    QString filePath = "src/users/" + username + ".txt";
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            emit logMessage(QString("用户信息格式错误：%1").arg(username));
            QByteArray response = "GetUserInfoFailed: Invalid JSON format";
            socket->write(response);
            socket->waitForBytesWritten();
            return;
        }

        QJsonObject responseObj;
        responseObj["type"] = "USERINFO";
        responseObj["username"] = username;
        responseObj["password"] = doc.object()["password"].toString();
        responseObj["number"] = doc.object()["number"].toString();
        responseObj["school"] = doc.object()["school"].toString();
        responseObj["phone"] = doc.object()["phone"].toString();

        QJsonDocument responseDoc(responseObj);
        socket->write(responseDoc.toJson());
        socket->waitForBytesWritten();
        emit logMessage(QString("用户信息发送成功：%1").arg(username));
    } else {
        emit logMessage(QString("无法读取用户信息：%1").arg(username));
        QByteArray response = "GetUserInfoFailed";
        socket->write(response);
        socket->waitForBytesWritten();
    }
}

//修改用户信息
void MyServer::handleUpdateUserInfo(QTcpSocket* socket, const QString& username, const QJsonObject& userInfo)
{
    QString filePath = "src/users/" + username + ".txt";
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument doc(userInfo);
        file.write(doc.toJson());
        file.close();

        QByteArray response = "UpdateUserInfoSuccess";
        socket->write(response);
        socket->waitForBytesWritten();
        emit logMessage(QString("用户信息更新成功：%1").arg(username));
    } else {
        emit logMessage(QString("用户信息更新失败：%1").arg(username));
        QByteArray response = "UpdateUserInfoFailed";
        socket->write(response);
        socket->waitForBytesWritten();
    }
}

// 处理搜索请求
void MyServer::handleSearchRequest(QTcpSocket* socket, const QString& keyword)
{
    QList<QFileInfo> searchResults = searchManager->search(keyword);
    emit logMessage(QString("搜索完成，找到 %1 个结果").arg(searchResults.size()));

    QByteArray response;
    for (const QFileInfo& fileInfo : searchResults) {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            QString fileName = fileInfo.fileName();

            response.append(("FileName: " + fileName + "\n").toUtf8());
            response.append(("Content-Length: " + QString::number(content.length()) + "\n").toUtf8());
            response.append("---\n");
            response.append(content.toUtf8());
            response.append("\n===\n");

            file.close();
        }
    }

    socket->write(response);
    socket->waitForBytesWritten();
}

//处理读取所有帖子的请求
void MyServer::handleGetAllPosts(QTcpSocket* socket) {
    QDir dir("src/posts");
    QStringList filters;
    filters << "post_*.txt";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);

    emit logMessage(QString("准备发送 %1 个帖子").arg(fileList.size()));

    QByteArray response;
    for (const QFileInfo& fileInfo : fileList) {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            response.append(("FileName: " + fileInfo.fileName() + "\n").toUtf8());
            response.append(("Content-Length: " + QString::number(content.length()) + "\n").toUtf8());
            response.append("---\n");
            response.append(content.toUtf8());
            response.append("\n===\n");
            file.close();
        }
    }

    socket->write(response);
    socket->waitForBytesWritten();
    emit logMessage("所有帖子数据发送完成");
}

// 处理获取所有收藏的帖子的请求
void MyServer::handleGetFavorites(QTcpSocket* socket, const QString& username)
{
    QString favoritesFilePath = "src/favorites/favorites_" + username + ".txt";
    QFile favoritesFile(favoritesFilePath);
    if (favoritesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&favoritesFile);
        QByteArray response;

        QJsonObject responseObj;
        responseObj["type"] = "GET_FAVORITES";

        QString line;
        int favoriteCount = 0;
        while (!in.atEnd()) {
            line = in.readLine();
            if (!line.startsWith("FileName: ")) {
                continue;
            }
            QString fileName = line.mid(10);
            favoriteCount++;

            in.readLine();

            QString content;
            while (!in.atEnd()) {
                line = in.readLine();
                if (line == "===") {
                    break;
                }
                content.append(line + "\n");
            }

            response.append(("FileName: " + fileName + "\n").toUtf8());
            response.append(("Content-Length: " + QString::number(content.length()) + "\n").toUtf8());
            response.append("---\n");
            response.append(content.toUtf8());
            response.append("\n===\n");
        }

        favoritesFile.close();

        QJsonDocument doc(responseObj);
        response.prepend(doc.toJson(QJsonDocument::Compact) + "\n");

        socket->write(response);
        socket->waitForBytesWritten();
        emit logMessage(QString("用户 %1 的 %2 个收藏帖子发送完成").arg(username).arg(favoriteCount));
    } else {
        emit logMessage(QString("无法打开收藏文件：%1").arg(favoritesFilePath));
        QByteArray response = "GetFavoritesFailed: Failed to open favorites file";
        socket->write(response);
    }
}

//处理注册请求
void MyServer::handleSignUp(QTcpSocket* socket, const QString &username, const QByteArray &userInfoData)
{
    QDir dir("src/users");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            emit logMessage("创建用户目录失败");
            QByteArray response = "SignUpError";
            socket->write(response);
            socket->waitForBytesWritten();
            return;
        }
    }

    QString filePath = "src/users/" + username + ".txt";
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument doc = QJsonDocument::fromJson(userInfoData);
        QJsonObject userInfo = doc.object();

        QTextStream out(&file);
        out << userInfo["password"].toString() << "\n";
        out << userInfo["number"].toString() << "\n";
        out << userInfo["school"].toString() << "\n";
        out << userInfo["phone"].toString();
        file.close();

        QByteArray response = "SignUpSuccess";
        socket->write(response);
        socket->waitForBytesWritten();
        emit logMessage(QString("用户注册成功：%1").arg(username));
    } else {
        emit logMessage(QString("用户注册失败：%1").arg(username));
        QByteArray response = "SignUpError";
        socket->write(response);
        socket->waitForBytesWritten();
    }
}

//处理登录请求
void MyServer::handleLogin(QTcpSocket* socket, const QString &username, const QString &password)
{
    QString filePath = "src/users/" + username + ".txt";
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray fileData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(fileData, &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            emit logMessage(QString("用户登录失败（格式错误）：%1").arg(username));
            socket->write("LoginError");
            socket->waitForBytesWritten();
            return;
        }

        QJsonObject userInfo = doc.object();
        QString storedPassword = userInfo["password"].toString();

        if (storedPassword == password) {
            socket->write("LoginSuccess");
            emit logMessage(QString("用户登录成功：%1").arg(username));
        } else {
            socket->write("LoginError");
            emit logMessage(QString("用户登录失败（密码错误）：%1").arg(username));
        }
    } else {
        socket->write("LoginError");
        emit logMessage(QString("用户登录失败（用户不存在）：%1").arg(username));
    }
    socket->waitForBytesWritten();
}

//处理存贴请求
void MyServer::saveTextToFile(const QString& text)
{
    QDir dir("src/posts");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QString fileName = "post_" + QString::number(totalPostNumber) + ".txt";
    QString filePath = "src/posts/" + fileName;
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << text;
        file.close();
        totalPostNumber++;
        MyServer::saveTotalPostNumber();
    }
}

//处理收藏请求
void MyServer::handleFavoriteRequest(QTcpSocket* socket, const QString& filename, const QString& username) {
    QDir dir("src/favorites");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            emit logMessage("创建收藏目录失败");
            QByteArray response = "FavoriteFailed: Failed to create directory";
            socket->write(response);
            return;
        }
    }

    QString filePath = "src/favorites/favorites_" + username + ".txt";
    QFile userFile(filePath);

    if (userFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&userFile);
        QString fileContent = in.readAll();
        if (fileContent.contains("FileName: " + filename)) {
            userFile.close();
            emit logMessage(QString("收藏失败：%1 已经收藏了 %2").arg(username, filename));
            QByteArray response = "FavoriteFailed: File already exists";
            socket->write(response);
            return;
        }
        userFile.close();
    }

    if (userFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&userFile);
        QFile postFile("src/posts/" + filename);

        if (postFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&postFile);
            QString content = in.readAll();
            out << "FileName: " << filename << "\n";
            out << "---\n";
            out << content << "\n===\n";
            postFile.close();
            emit logMessage(QString("收藏成功：%1 收藏了 %2").arg(username, filename));
        } else {
            emit logMessage(QString("收藏失败：无法打开帖子文件 %1").arg(filename));
            QByteArray response = "FavoriteFailed: Failed to open post file";
            socket->write(response);
            userFile.close();
            return;
        }
        userFile.close();
    } else {
        emit logMessage(QString("收藏失败：无法打开用户收藏文件 %1").arg(filePath));
        QByteArray response = "FavoriteFailed: Failed to open user file";
        socket->write(response);
        return;
    }

    QByteArray response = "FavoriteSuccess";
    socket->write(response);
}

// 从文件加载计数器值
void MyServer::loadTotalPostNumber() {
    QFile file("src/totalnumber.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString numberStr = in.readLine();
        totalPostNumber = numberStr.toInt();
        file.close();
    } else {
        totalPostNumber = 0;
    }
    qDebug() << "Loaded totalPostNumber:" << totalPostNumber;
}

// 将计数器值保存到文件
void MyServer::saveTotalPostNumber() {
    QFile file("src/totalnumber.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << totalPostNumber;
        file.close();
    }
    qDebug() << "Saved totalPostNumber:" << totalPostNumber;
}

//手动设置
void MyServer::setTotalPostNumber(int value) {
    totalPostNumber = value;
    saveTotalPostNumber();
}
