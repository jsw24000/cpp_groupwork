#include "postwidget.h"
#include "ui_postwidget.h"
#include "comment.h"
#include "reply.h"
#include <QDebug>
#include "mainwindow.h"

const int FIXED_HEIGHT = 100;
PostWidget::PostWidget(const QString &content, const QString fileName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PostWidget)
    , my_filename(fileName)
{
    ui->setupUi(this);
    // contentLabel = new QLabel(content, this);
    // QVBoxLayout *layout = new QVBoxLayout(this);
    // 设置QTextEdit的内容
    ui->textEdit->setPlainText(content);
    // 设置为只读模式（可选）
    ui->textEdit->setReadOnly(true);

    // layout->addWidget(contentLabel);
    // setLayout(layout);
    setStyleSheet("border: 1px solid black; margin: 5px;");

    setFixedHeight(FIXED_HEIGHT);
}

PostWidget::~PostWidget()
{
    delete ui;
}

void PostWidget::on_commentButton_clicked()
{
    Comment *comment = new Comment(my_filename);
    comment->show();
    comment->loadReplies();
}


void PostWidget::on_saveButton_clicked()
{
    qDebug()<<"准备发送收藏信息";
    QJsonObject requestObj;
    requestObj["type"] = "FAVORITE";
    requestObj["filename"] = my_filename;
    requestObj["username"] = myusername;
    QJsonDocument doc(requestObj);
    QByteArray request = doc.toJson(QJsonDocument::Compact);


    if (client_ptr!=nullptr) {
        qDebug()<<"开始发送收藏信息";
        if (client_ptr->m_socket->state() == QTcpSocket::ConnectedState) {
            client_ptr->sendRequest(request);
            qDebug() << "测试收藏";
            connect(client_ptr, &Client::dataReceived, [this](const QByteArray& data) {
                QString response = QString(data);
                if (response == "FavoriteSuccess") {
                    QMessageBox::information(this, "收藏成功", "帖子已成功收藏！");
                } else {
                    QMessageBox::critical(this, "收藏失败", "收藏帖子时出现错误，请重试。");
                }
            });
        }
    }

}

