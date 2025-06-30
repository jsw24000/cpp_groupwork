#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
// #include <QSqlDatabase> //数据驱动
// #include <QSqlQuery> //数据库执行语句
#include <QMessageBox>//消息盒子
#include <QDebug>
#include "client.h"

void sqlite_Init();

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_commitButton_clicked();

    void on_signUpButton_clicked();

private:
    Ui::Login *ui;
    Client client;
};

#endif // LOGIN_H
