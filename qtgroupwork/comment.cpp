#include "comment.h"
#include "ui_comment.h"
#include "reply.h"

#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <QVBoxLayout>

Comment::Comment(const QString &filename, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Comment)
    , my_filename(filename)
{
    ui->setupUi(this);
    loadText(my_filename);
}

Comment::~Comment()
{
    delete ui;
}

void Comment::loadText(const QString &filename)
{
    QString filePath = "src/posts/" + filename;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString text = in.readAll();
        ui->textEdit->setPlainText(text);
        file.close();
    } else {
        qDebug() << "Failed to open file:" << filePath;
    }
}

void Comment::on_pushButton_clicked()
{
    Reply *reply = new Reply(my_filename);
    if (reply->exec() == QDialog::Accepted) {
        loadReplies();
    }
}

#include <QChar>
#include <QStringList>

void Comment::loadReplies()
{
    QString replyFileName = "src/reply/" + my_filename + "_reply";
    QFile file(replyFileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString allText = in.readAll();
        QStringList replies = allText.split("---END_OF_REPLY---\n", Qt::SkipEmptyParts);

        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
        if (!layout) {
            layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
            ui->scrollAreaWidgetContents->setLayout(layout);
        }

        // 清空现有回复
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }

        const int FIXED_REPLY_HEIGHT = 30; // 定义评论的固定高度

        QString names[26] = {"Alice", "Bob", "Carol", "Dave", "Eve", "Francis", "Grace", "Hans", "Isabella", "Jason", "Kate",
                            "Louis", "Margaret", "Nathan", "Olivia", "Paul", "Queen", "Richard", "Susan", "Thomas", "Uma", "Vivian", "Winnie", "Xander", "Yasmine", "Zach"};
        int current_num = 0;
        for (const QString &reply : replies) {
            QString name = names[current_num];
            QString fullReply = name + ": " + reply.trimmed();
            QLabel *replyLabel = new QLabel(fullReply, ui->scrollAreaWidgetContents);
            replyLabel->setFixedHeight(FIXED_REPLY_HEIGHT); // 设置评论标签的固定高度
            layout->addWidget(replyLabel);

            // 更新首字母
            if (current_num == 25) {
                current_num = 0;
            } else {
                current_num++;
            }
        }

        file.close();
    }
}
