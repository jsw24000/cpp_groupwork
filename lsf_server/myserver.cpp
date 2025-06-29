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

//澶勭悊鐩戝惉鍒扮殑璇锋眰
void MyServer::handleClientData() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    qDebug() << "Received:" << QString(data);

    //濡傛灉鏄兂鑾峰彇鎵€鏈夊笘瀛愮殑璇锋眰
    if (data == "GetAllPosts") {
        handleGetAllPosts(socket);
        return;
    }

    // 灏濊瘯瑙ｆ瀽 JSON 鏁版嵁
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();
        //濡傛灉鏄瓨璐磋姹?
        if (obj.contains("type") && obj["type"].toString() == "POST" && obj.contains("content")) {
            QString content = obj["content"].toString();
            // 淇濆瓨甯栧瓙鍐呭鍒版枃浠?
            saveTextToFile(content);
            // 鍙戦€佸搷搴旂粰瀹㈡埛绔?
            QByteArray response = "Success";
            socket->write(response);
            socket->waitForBytesWritten();
            return;
        }
        //濡傛灉鏄櫥褰曟垨鑰呮敞鍐岃姹?
        else if (obj.contains("type")) {
            QString type = obj["type"].toString();
            if (type == "SIGNUP") {
                QString username = obj["username"].toString();
                QString password = obj["password"].toString();
                handleSignUp(socket, username, password);
                return;
            } else if (type == "LOGIN") {
                QString username = obj["username"].toString();
                QString password = obj["password"].toString();
                handleLogin(socket, username, password);
                return;
            }
        }
        //濡傛灉鏄敹钘忚姹?
        else if (obj.contains("type") && obj["type"].toString() == "FAVORITE" && obj.contains("filename") && obj.contains("username")) {
            QString filename = obj["filename"].toString();
            QString username = obj["username"].toString();
            handleFavoriteRequest(socket, filename, username);
            return;
        }
        //濡傛灉鏄樉绀烘墍鏈夋敹钘忕殑甯栧瓙鐨勮姹?
        else if (obj.contains("type") && obj["type"].toString() == "GET_FAVORITES" && obj.contains("username")) {
            QString username = obj["username"].toString();
            handleGetFavorites(socket, username);
            return;
        }
    }
}

//澶勭悊璇诲彇鎵€鏈夊笘瀛愮殑璇锋眰
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

            // 娣诲姞鏂囦欢鍚嶈
            response.append(("FileName: " + fileInfo.fileName() + "\n").toUtf8());
            // 娣诲姞鍐呭闀垮害琛岋紙鍙€変絾鎺ㄨ崘锛岀敤浜庡鐞嗗唴瀹逛腑鍙兘鍖呭惈鐨勫垎闅旂锛?
            response.append(("Content-Length: " + QString::number(content.length()) + "\n").toUtf8());

            // 娣诲姞鍒嗛殧琛?
            response.append("---\n");

            // 娣诲姞鍐呭
            response.append(content.toUtf8());

            // 娣诲姞缁撴潫鏍囪
            response.append("\n===\n");

            file.close();
        }
    }

    socket->write(response);
    socket->waitForBytesWritten();
}

//澶勭悊鑾峰彇鎵€鏈夋敹钘忕殑甯栧瓙鐨勮姹?
void MyServer::handleGetFavorites(QTcpSocket* socket, const QString& username)
{
    QString favoritesFilePath = "src/favorites/favorites_" + username + ".txt";
    QFile favoritesFile(favoritesFilePath);
    if (favoritesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&favoritesFile);
        QStringList favoriteFileNames;
        while (!in.atEnd()) {
            QString fileName = in.readLine();
            favoriteFileNames.append(fileName);
        }
        favoritesFile.close();

        QDir dir("src/posts");
        QByteArray response;
        for (const QString& fileName : favoriteFileNames) {
            QFile file(dir.filePath(fileName));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QString content = in.readAll();

                // 娣诲姞鏂囦欢鍚嶈
                response.append(("FileName: " + fileName + "\n").toUtf8());
                // 娣诲姞鍐呭闀垮害琛岋紙鍙€変絾鎺ㄨ崘锛岀敤浜庡鐞嗗唴瀹逛腑鍙兘鍖呭惈鐨勫垎闅旂锛?
                response.append(("Content-Length: " + QString::number(content.length()) + "\n").toUtf8());

                // 娣诲姞鍒嗛殧琛?
                response.append("---\n");

                // 娣诲姞鍐呭
                response.append(content.toUtf8());

                // 娣诲姞缁撴潫鏍囪
                response.append("\n===\n");

                file.close();
            }
        }

        socket->write(response);
        socket->waitForBytesWritten();
    }
}

//澶勭悊娉ㄥ唽璇锋眰
void MyServer::handleSignUp(QTcpSocket* socket, const QString &username, const QString &password)
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
        QTextStream out(&file);
        out << password;
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

//澶勭悊鐧诲綍璇锋眰
void MyServer::handleLogin(QTcpSocket* socket, const QString &username, const QString &password)
{
    QString filePath = "src/users/" + username + ".txt";
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString storedPassword = in.readAll();
        file.close();
        if (storedPassword == password) {
            QByteArray response = "LoginSuccess";
            socket->write(response);
            socket->waitForBytesWritten();
        } else {
            QByteArray response = "LoginError";
            socket->write(response);
            socket->waitForBytesWritten();
        }
    } else {
        QByteArray response = "LoginError";
        socket->write(response);
        socket->waitForBytesWritten();
    }
}

//澶勭悊瀛樿创璇锋眰
void MyServer::saveTextToFile(const QString& text)
{
    qDebug() << "Current working directory: " << QDir::currentPath();
    // 妫€鏌ュ苟鍒涘缓鐩綍
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
        MyServer::saveTotalPostNumber(); // 绔嬪嵆淇濆瓨鏇存柊鍚庣殑鍊?
    }
}

//澶勭悊鏀惰棌璇锋眰
void MyServer::handleFavoriteRequest(QTcpSocket* socket, const QString& filename, const QString& username) {
    QDir dir("src/user_loves");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QString filePath = "src//favorites/favorites_" + username + ".txt";
    QFile userFile(filePath);
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
        }
        userFile.close();
    }

    QByteArray response = "FavoriteSuccess";
    socket->write(response);
    socket->waitForBytesWritten();
}

// 浠嶲Settings鍔犺浇璁℃暟鍣ㄥ€?
void MyServer::loadTotalPostNumber() {
    QSettings settings("QTQ-group", "ShuKeng");
    totalPostNumber = settings.value("totalPostNumber", 0).toInt();
    qDebug() << "Loaded totalPostNumber:" << totalPostNumber;
}

// 灏嗚鏁板櫒鍊间繚瀛樺埌QSettings
void MyServer::saveTotalPostNumber() {
    QSettings settings("QTQ-group", "ShuKeng");
    settings.setValue("totalPostNumber", totalPostNumber);
    qDebug() << "Saved totalPostNumber:" << totalPostNumber;
}

//鎵嬪姩璁剧疆
void MyServer::setTotalPostNumber(int value) {
    totalPostNumber = value;
    saveTotalPostNumber(); // 绔嬪嵆淇濆瓨鏂板€?
}
