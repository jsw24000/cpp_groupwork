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
        if (obj.contains("type") && obj["type"].toString() == "POST" && obj.contains("content")) {
            QString content = obj["content"].toString();
            // 保存帖子内容到文件
            saveTextToFile(content);
            // 发送响应给客户端
            QByteArray response = "Success";
            socket->write(response);
            socket->waitForBytesWritten();
            return;
        }
    }
}

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

// 从QSettings加载计数器值
void MyServer::loadTotalPostNumber() {
    QSettings settings("QTQ-group", "ShuKeng");
    totalPostNumber = settings.value("totalPostNumber", 0).toInt();
    qDebug() << "Loaded totalPostNumber:" << totalPostNumber;
}

// 将计数器值保存到QSettings
void MyServer::saveTotalPostNumber() {
    QSettings settings("QTQ-group", "ShuKeng");
    settings.setValue("totalPostNumber", totalPostNumber);
    qDebug() << "Saved totalPostNumber:" << totalPostNumber;
}

//手动设置
void MyServer::setTotalPostNumber(int value) {
    totalPostNumber = value;
    saveTotalPostNumber(); // 立即保存新值
}
