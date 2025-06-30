#include "comment.h"
#include "ui_comment.h"
#include "reply.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QJsonObject>

Comment::Comment(QString postcontent, const QString &filename, Client* client, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Comment)
    , my_filename(filename)
    , m_client(client)
{
    ui->setupUi(this);
    setWindowTitle("评论区");
    ui->textEdit->setReadOnly(true);
    QString richText = QString("<b><font color='blue'>%1</font></b>").arg(postcontent);
    ui->textEdit->setHtml(richText);

    // 创建评论列表并添加到scrollArea中
    commentList = new QListWidget();
    commentList->setWordWrap(true);
    commentList->setStyleSheet("QListWidget { border: none; }");
    ui->scrollArea->setWidget(commentList);
    ui->scrollArea->setWidgetResizable(true);

    // 确保评论目录存在
    QDir().mkpath("src/comments");

    // 加载已有评论
    loadComments();

    // 连接信号
    // connect(m_client, &Client::dataReceived, this, &Comment::handleDataReceived);

}

void Comment::loadComments()
{
    commentList->clear();

    QString commentFilePath = "comments/" + my_filename + "_comments.txt";
    QFile commentFile(commentFilePath);

    if (commentFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&commentFile);

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                QListWidgetItem *item = new QListWidgetItem(line);
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable); // 不可选中
                commentList->addItem(item);
            }
        }

        commentFile.close();
    }
}

void Comment::saveComment(const QString& username, const QString& content)
{
    // 确保评论目录存在
    QDir().mkpath("comments");

    QString commentFilePath = "comments/" + my_filename + "_comments.txt";
    QFile commentFile(commentFilePath);

    if (commentFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&commentFile);

        // 格式化评论: [时间] 用户名: 内容
        QString formattedComment = QString("%1: %2").arg(username, content);

        out << formattedComment << "\n";
        commentFile.close();

        // 重新加载评论
        loadComments();
    } else {
        QMessageBox::warning(this, "错误", "无法保存评论");
    }
}

Comment::~Comment()
{
    delete ui;
}
//暂时弃置的函数
void Comment::handleCommentsReceived(const QByteArray& data){}
void Comment::handleDataReceived(const QByteArray& data){}
void Comment::requestComments(){}

void Comment::on_pushButton_clicked()
{
    Reply *reply = new Reply(my_filename, m_client);
    reply->myusername = myusername;
    if (reply->exec() == QDialog::Accepted) {
        saveComment(myusername, reply->getCommentText());
    }
    delete reply;
}
