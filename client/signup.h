#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include "mainwindow.h"
#include "client.h"

namespace Ui {
class SignUp;
}

class SignUp : public QWidget
{
    Q_OBJECT

public:
    explicit SignUp(QWidget *parent = nullptr);
    ~SignUp();

private slots:
    void on_loginButton_clicked();

    void on_signUpButton_clicked();

    void on_availableButton_clicked();

private:
    Ui::SignUp *ui;
    Client client;
    void setIcon(int a, QPushButton* button, const QString& filename);
    QString usernameJudge(const QString& username);
    QString passwordJudge(const QString& password);
};

#endif // SIGNUP_H
