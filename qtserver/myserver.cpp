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

MyServer::MyServer(QObject *parent) : QTcpServer(parent) {}

void MyServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    m_clients.append(socket);

    connect(socket, &QTcpSocket::readyRead, this, &MyServer::handleClientData);
    connect(socket, &QTcpSocket::disconnected, [this, socket]() {
        m_clients.removeOne(socket);
        socket->deleteLater();
    });
}

MyServer::~MyServer() {
    saveTotalPostNumber();
}

//处理监听到的请求
void MyServer::handleClientData() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    qDebug() << "Received:" << QString(data);

    //如果是想获取所有帖子的请求
    if (data == "GetAllPosts") {
        handleGetAllPosts(socket);
        return;
    }

    // 尝试解析 JSON 数据
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();
        qDebug()<<"读取到了JSON信息";
        qDebug()<<obj["type"];
        qDebug()<<obj["filename"];
        qDebug()<<obj["username"];
        if (obj.contains("type") && obj["type"].toString() == "POST" && obj.contains("content")) {
            //如果是存贴请求
            qDebug()<<"准备存放帖子";
            QString content = obj["content"].toString();
            // 保存帖子内容到文件
            saveTextToFile(content);
            // 发送响应给客户端
            QByteArray response = "Success";
            socket->write(response);
            socket->waitForBytesWritten();
            return;
        }
        //如果是收藏请求
        else if (obj.contains("type") && obj["type"].toString() == "FAVORITE" && obj.contains("filename") && obj.contains("username")) {
            QString filename = obj["filename"].toString();
            QString username = obj["username"].toString();
            qDebug()<<"准备处理";
            handleFavoriteRequest(socket, filename, username);
            return;
        }
        //如果是显示所有收藏的帖子的请求
        else if (obj.contains("type") && obj["type"].toString() == "GET_FAVORITES" && obj.contains("username")) {
            qDebug()<<"准备读取所有收藏的帖子";
            QString username = obj["username"].toString();
            handleGetFavorites(socket, username);
            return;
        }
        // 如果是搜索请求
        else if (obj.contains("type") && obj["type"].toString() == "SEARCH" && obj.contains("keyword")) {
            QString keyword = obj["keyword"].toString();
            handleSearchRequest(socket, keyword);
            return;
        }
        //如果是请求获取或者修改用户信息
        else if (obj.contains("type") && obj["type"].toString() == "GET_USERINFO" && obj.contains("username")) {
            QString username = obj["username"].toString();
            handleGetUserInfo(socket, username);
            return;
        }
        else if (obj.contains("type") && obj["type"].toString() == "UPDATE_USERINFO" && obj.contains("username")) {
            QJsonObject userInfo = obj["userInfo"].toObject();
            QString username = obj["username"].toString();
            handleUpdateUserInfo(socket, username, userInfo);
            return;
        }
        //如果是有关评论的请求
        else if (obj.contains("type") && obj["type"].toString() == "GET_COMMENTS" && obj.contains("filename")) {
            QString filename = obj["filename"].toString();
            handleGetComments(socket, filename);
            return;
        }
        else if (obj.contains("type") && obj["type"].toString() == "POST_COMMENT" && obj.contains("filename") && obj.contains("username") && obj.contains("content")) {
            QString filename = obj["filename"].toString();
            QString username = obj["username"].toString();
            QString content = obj["content"].toString();
            handlePostComment(socket, filename, username, content);
            return;
        }
        //如果是登录或者注册请求
        else if (obj.contains("type")) {
            qDebug()<<"准备登录/注册";
            QString type = obj["type"].toString();
            if (type == "SIGNUP") {
                QString username = obj["username"].toString();
                QString password = obj["password"].toString();
                // 创建包含用户信息的JSON对象
                QJsonObject userInfo;
                userInfo["password"] = password;
                userInfo["number"] = "";  // 默认值
                userInfo["school"] = "";  // 默认值
                userInfo["phone"] = "";   // 默认值
                // 转换为JSON文档并发送
                QJsonDocument doc(userInfo);
                handleSignUp(socket, username, doc.toJson());
                return;
            } else if (type == "LOGIN") {
                QString username = obj["username"].toString();
                QString password = obj["password"].toString();
                handleLogin(socket, username, password);
                return;
            }
        }
    }else{
        qDebug() << "JSON 解析错误:" << parseError.errorString();
    }
}

//获取已有评论
void MyServer::handleGetComments(QTcpSocket* socket, const QString& filename)
{
    // QString commentFilePath = "src/comments/" + filename + "_comments.txt";
    // QFile commentFile(commentFilePath);

    // if (commentFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //     QTextStream in(&commentFile);
    //     QByteArray response;

    //     while (!in.atEnd()) {
    //         QString line = in.readLine();
    //         response.append(line.toUtf8() + "\n");
    //     }

    //     socket->write(response);
    //     commentFile.close();
    // } else {
    //     QByteArray response = "NoComments";
    //     socket->write(response);
    // }
    // socket->waitForBytesWritten();
}

//储存评论
void MyServer::handlePostComment(QTcpSocket* socket, const QString& filename, const QString& username, const QString& content)
{
    // // 确保评论目录存在
    // QDir dir("src/comments");
    // if (!dir.exists()) {
    //     dir.mkpath(".");
    // }

    // QString commentFilePath = "src/comments/" + filename + "_comments.txt";
    // QFile commentFile(commentFilePath);

    // if (commentFile.open(QIODevice::Append | QIODevice::Text)) {
    //     QTextStream out(&commentFile);
    //     out << username << ": " << content << "\n";
    //     commentFile.close();

    //     QByteArray response = "CommentPosted";
    //     socket->write(response);
    // } else {
    //     QByteArray response = "CommentFailed";
    //     socket->write(response);
    // }
    // socket->waitForBytesWritten();
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
    } else {
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
    } else {
        QByteArray response = "UpdateUserInfoFailed";
        socket->write(response);
        socket->waitForBytesWritten();
    }
}

// 处理搜索请求
void MyServer::handleSearchRequest(QTcpSocket* socket, const QString& keyword)
{
    QList<QFileInfo> searchResults = searchManager->search(keyword); // 调用搜索算法

    QByteArray response;
    for (const QFileInfo& fileInfo : searchResults) {
        qDebug()<<"找到了文件"<<fileInfo;
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            QString fileName = fileInfo.fileName();

            // 添加文件名行
            response.append(("FileName: " + fileName + "\n").toUtf8());
            // 添加内容长度行（可选但推荐，用于处理内容中可能包含的分隔符）
            response.append(("Content-Length: " + QString::number(content.length()) + "\n").toUtf8());

            // 添加分隔行
            response.append("---\n");

            // 添加内容
            response.append(content.toUtf8());

            // 添加结束标记
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

    QByteArray response;
    for (const QFileInfo& fileInfo : fileList) {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            // 添加文件名行
            response.append(("FileName: " + fileInfo.fileName() + "\n").toUtf8());
            // 添加内容长度行（可选但推荐，用于处理内容中可能包含的分隔符）
            response.append(("Content-Length: " + QString::number(content.length()) + "\n").toUtf8());

            // 添加分隔行
            response.append("---\n");

            // 添加内容
            response.append(content.toUtf8());

            // 添加结束标记
            response.append("\n===\n");

            file.close();
        }
    }

    socket->write(response);
    socket->waitForBytesWritten();
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
        while (!in.atEnd()) {
            // 读取文件名行
            line = in.readLine();
            if (!line.startsWith("FileName: ")) {
                continue;
            }
            QString fileName = line.mid(10); // 提取文件名

            // 跳过分隔行
            in.readLine();

            // 读取内容
            QString content;
            while (!in.atEnd()) {
                line = in.readLine();
                if (line == "===") {
                    break;
                }
                content.append(line + "\n");
            }

            // 添加文件名行
            response.append(("FileName: " + fileName + "\n").toUtf8());
            // 添加内容长度行（可选但推荐，用于处理内容中可能包含的分隔符）
            response.append(("Content-Length: " + QString::number(content.length()) + "\n").toUtf8());

            // 添加分隔行
            response.append("---\n");

            // 添加内容
            response.append(content.toUtf8());

            // 添加结束标记
            response.append("\n===\n");
        }

        favoritesFile.close();

        QJsonDocument doc(responseObj);
        response.prepend(doc.toJson(QJsonDocument::Compact) + "\n");

        socket->write(response);
        socket->waitForBytesWritten();
    } else {
        // 处理收藏文件打开失败的情况
        qDebug() << "Failed to open favorites file:" << favoritesFile.fileName();
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
    } else {
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
        // 读取整个文件内容
        QByteArray fileData = file.readAll();
        file.close();

        // 解析JSON格式的用户信息
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(fileData, &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            qDebug() << "JSON解析错误:" << parseError.errorString();
            socket->write("LoginError");
            socket->waitForBytesWritten();
            return;
        }

        QJsonObject userInfo = doc.object();
        QString storedPassword = userInfo["password"].toString();

        if (storedPassword == password) {
            socket->write("LoginSuccess");
        } else {
            socket->write("LoginError");
        }
    } else {
        socket->write("LoginError");
    }
    socket->waitForBytesWritten();
}

//处理存贴请求
void MyServer::saveTextToFile(const QString& text)
{
    qDebug() << "Current working directory: " << QDir::currentPath();
    // 检查并创建目录
    QDir dir("src/posts");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {

            return;
        }
    }

    qDebug()<<"当前存放的帖子的序列号："<<totalPostNumber;
    QString fileName = "post_" + QString::number(totalPostNumber) + ".txt";
    QString filePath = "src/posts/" + fileName;
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << text;
        file.close();
        totalPostNumber++;
        MyServer::saveTotalPostNumber(); // 立即保存更新后的值
    }
}

//处理收藏请求
void MyServer::handleFavoriteRequest(QTcpSocket* socket, const QString& filename, const QString& username) {
    // 创建收藏目录
    qDebug()<<"开始收藏";
    QDir dir("src/favorites");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create directory";
            QByteArray response = "FavoriteFailed: Failed to create directory";
            socket->write(response);
            return;
        }
    }

    // 构建用户收藏文件路径
    QString filePath = "src/favorites/favorites_" + username + ".txt";
    QFile userFile(filePath);

    // 检查文件是否已经收藏
    if (userFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&userFile);
        QString fileContent = in.readAll();
        if (fileContent.contains("FileName: " + filename)) {
            userFile.close();
            QByteArray response = "FavoriteFailed: File already exists";
            socket->write(response);
            return;
        }
        userFile.close();
    }

    // 打开用户收藏文件进行追加写入
    if (userFile.open(QIODevice::Append | QIODevice::Text)) {
        qDebug()<<"打开收藏文件";
        QTextStream out(&userFile);
        QFile postFile("src/posts/" + filename);

        // 打开帖子文件读取内容
        if (postFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&postFile);
            QString content = in.readAll();
            out << "FileName: " << filename << "\n";
            out << "---\n";
            out << content << "\n===\n";
            postFile.close();
        } else {
            qDebug() << "Failed to open post file:" << postFile.fileName();
            QByteArray response = "FavoriteFailed: Failed to open post file";
            socket->write(response);
            userFile.close();
            return;
        }
        userFile.close();
    } else {
        qDebug() << "Failed to open user file:" << userFile.fileName();
        QByteArray response = "FavoriteFailed: Failed to open user file";
        socket->write(response);
        return;
    }

    // 发送成功信息
    QByteArray response = "FavoriteSuccess";
    socket->write(response);
}

// 从QSettings加载计数器值
void MyServer::loadTotalPostNumber() {
    // QSettings settings("QTQ-group", "ShuKeng");
    // qDebug() << "Settings file path:" << settings.fileName();
    // totalPostNumber = settings.value("totalPostNumber", 0).toInt();
    QFile file("src/totalnumber.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString numberStr = in.readLine();
        totalPostNumber = numberStr.toInt();
        file.close();
    } else {
        totalPostNumber = 0; // 如果文件不存在或无法打开，初始化为 0
    }
    qDebug() << "Loaded totalPostNumber:" << totalPostNumber;
}

// 将计数器值保存到QSettings
void MyServer::saveTotalPostNumber() {
    // QSettings settings("QTQ-group", "ShuKeng");
    // qDebug() << "Settings file path:" << settings.fileName();
    // settings.setValue("totalPostNumber", totalPostNumber);
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
    saveTotalPostNumber(); // 立即保存新值
}
