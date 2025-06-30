#include "reply.h"
#include "ui_reply.h"
#include <QMessageBox>

Reply::Reply(const QString fileName, Client* client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Reply)
    , my_fileName(fileName)
    , m_client(client)
{
    ui->setupUi(this);
    setWindowTitle("发表回复");
}

Reply::~Reply()
{
    delete ui;
}

QString Reply::getCommentText() const
{
    return ui->textEdit->toPlainText();
}

void Reply::on_buttonBox_accepted(){}
