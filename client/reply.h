#ifndef REPLY_H
#define REPLY_H

#include <QDialog>
#include "client.h"

namespace Ui {
class Reply;
}

class Reply : public QDialog
{
    Q_OBJECT

public:
    explicit Reply(const QString filename, Client* client, QWidget *parent = nullptr);
    ~Reply();
    QString myusername;
    QString getCommentText() const;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Reply *ui;
    QString my_fileName;
    Client* m_client;
};

#endif // REPLY_H
