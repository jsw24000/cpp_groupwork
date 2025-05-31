#include "login.h"
#include "ui_login.h"
#include "signup.h"
#include "mainwindow.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void sqlite_Init()
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("user.db");
    if(!db.open())
    {
        qDebug()<<"open error";
    }
    //create excle
    QString createsql=QString("create table if not exists user(id integer primary key autoincrement,"
                                "username ntext unique not NULL,"
                                "password ntext not NULL)");
    QSqlQuery query;
    if(!query.exec(createsql)){
        qDebug()<<"table create error";
    }
    else{
        qDebug()<<"table create success";
    }
}

QString usernameJudge(QString username){
    /*
    判断username是否合法，返回一个QString：

    合法用户名的要求：
    1. 4~20个字符
    2. 不能包含除大小写字母、下划线、数字之外的任何字符

    依次检查用户名的两项基本要求：
    若1不满足，return "用户名长度需在4~20个字符！"
    若1满足，2不满足，return "用户名不能包含除大小写字母、下划线、数字之外的任何字符！"
    若合法，return "legal"
    */

    return " ";
}

QString passwordJudge(QString password){
    /*
    判断password是否合法，返回一个QString

    合法密码的要求：
    1. 长度为8~12位；
    2. 不允许使用单引号、双引号、尖括号、分号；
    3. 必须包含下述字符类型中至少3种：
        1) 大写字母（A-Z）
        2) 小写字母（a-z）
        3) 数字（0-9）
        4) 特殊符号（不允许的类型在2中筛查）

    依次检查密码的3项基本要求：
    若1不满足，return "密码长度需在8~12位！"
    若2不满足，return "密码不能包含单引号、双引号、尖括号和分号！"
    若3不满足，return "密码必须包含大写字母、小写字母、数字、特殊字符中的至少3种字符类型！"
    若合法，return "legal"

    */
    return " ";
}

void Login::on_commitButton_clicked()
{
    sqlite_Init();
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString sql=QString("select * from user where username='%1' and password='%2'")
                      .arg(username).arg(password);
    //创建执行语句对象
    QSqlQuery query(sql);
    //判断执行结果
    if(!query.next())
    {
        qDebug()<<"Login error";
        QMessageBox::information(this,"登录认证","登录失败,账户或者密码错误");
    }
    else
    {
        qDebug()<<"Login success";
        QMessageBox::information(this,"登录认证","登录成功");
        //登录成功后可以跳转到其他页面
        MainWindow *w = new MainWindow;
        w->show();
        this->close();
    }
}


void Login::on_signUpButton_clicked()
{
    this->close();
    SignUp *s = new SignUp;
    s->show();
}

