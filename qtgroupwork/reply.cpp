#include "reply.h"
#include "ui_reply.h"

#include <QDir>

Reply::Reply(const QString fileName, QWidget *parent)
    : QDialog(parent)
    , my_fileName(fileName)
    , ui(new Ui::Reply)
{
    ui->setupUi(this);
}

Reply::~Reply()
{
    delete ui;
}

void Reply::on_buttonBox_accepted()
{
    // 检查并创建目录
    QDir dir("src/reply");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }

    QString filePath = "src/reply/" + my_fileName + "_reply";
    QFile file(filePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        QString replyText = ui->textEdit->toPlainText();
        out << replyText << "\n---END_OF_REPLY---\n";
        file.close();
        accept();
    }
}

