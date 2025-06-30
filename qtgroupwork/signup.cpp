#include "signup.h"
#include "ui_signup.h"
#include "login.h"

SignUp::SignUp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SignUp)
{
    ui->setupUi(this);
    setWindowTitle("注册");
}

SignUp::~SignUp()
{
    delete ui;
}

void SignUp::on_loginButton_clicked()
{
    this->close();
    Login *l = new Login;
    l->show();
}


void SignUp::on_signUpButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    if (!client.isConnected()) {
        if (!client.connectToServer(qApp->property("IP").toString(), 1234)){
            QMessageBox::critical(this, "网络错误", "无法连接到服务器，请检查网络设置");
        }
    }

    // 创建包含默认值的用户信息
    QJsonObject userInfo;
    userInfo["password"] = password;
    userInfo["number"] = "";
    userInfo["school"] = "";
    userInfo["phone"] = "";

    // 发送注册请求
    client.sendSignUpRequest(username, QJsonDocument(userInfo).toJson());

    connect(&client, &Client::dataReceived, [this,username](const QByteArray& data) {
        QString response = QString(data);
        if (response == "SignUpSuccess") {
            MainWindow *w = new MainWindow;
            w->setUserName(username);
            w->show();
            this->close();
        } else {
            QMessageBox::information(this, "注册认证", "注册失败！");
        }
    });
}

