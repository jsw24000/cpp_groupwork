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
    sqlite_Init();
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString sql=QString("insert into user(username,password) values('%1','%2');")
                      .arg(username).arg(password);
    //创建执行语句对象
    QSqlQuery query;
    //判断执行结果
    if(!query.exec(sql))
    {
        qDebug()<<"insert into error";
        QMessageBox::information(this,"注册认证","插入失败！");
    }
    else
    {
        qDebug()<<"insert into success";
        QMessageBox::information(this,"注册认证","插入成功！");
        MainWindow *w = new MainWindow;
        w->show();
        this->close();
    }

}

