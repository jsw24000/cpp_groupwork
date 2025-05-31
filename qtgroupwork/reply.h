#ifndef REPLY_H
#define REPLY_H

#include <QDialog>

namespace Ui {
class Reply;
}

class Reply : public QDialog
{
    Q_OBJECT

public:
    explicit Reply(const QString fileName, QWidget *parent = nullptr);
    ~Reply();
    QString my_fileName;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Reply *ui;
};

#endif // REPLY_H
