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

    setIcon(25, ui->commentButton, ":/icon/comment.png");
    ui->commentButton->setCursor(Qt::PointingHandCursor);
    setIcon(25, ui->saveButton, ":/icon/unbookmark.png");
    ui->saveButton->setCursor(Qt::PointingHandCursor);
    // layout->addWidget(contentLabel);
    // setLayout(layout);
    setStyleSheet("border: 1px solid black; margin: 5px;");

    setFixedHeight(FIXED_HEIGHT);
}

PostWidget::~PostWidget()
{
    delete ui;
}

void PostWidget::setIcon(int a, QPushButton* button, const QString& filename){
    button->setText("");
    button->setFixedSize(a, a);
    button->setIcon(QIcon(filename));
    button->setIconSize(QSize(button->width(), button->height()));
    button->setStyleSheet(R"(
        QPushButton {
            background-color: transparent; /* 透明背景 */
            border: none; /* 去除边框 */
            padding: 0px; /* 去除内边距，让图标贴合按钮边缘 */
        })"
                          );
}

void PostWidget::on_commentButton_clicked()
{
    if (client_ptr != nullptr) {
        QString temp = ui->textEdit->toPlainText();
        Comment *comment = new Comment(temp, my_filename, client_ptr);
        comment->myusername = myusername;
        comment->show();
    } else {
        QMessageBox::warning(this, "错误", "无法连接到服务器");
    }
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
                    setIcon(25, ui->saveButton, ":/icon/bookmark.png");
                } else {
                    QMessageBox::critical(this, "收藏失败", "收藏帖子时出现错误，请重试。");
                }
            });
        }
    }

}

void PostWidget::setTextEditSize(int width, int height)
{
    ui->textEdit->setFixedSize(width, height);
}

