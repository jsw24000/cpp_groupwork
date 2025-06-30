#include "signup.h"
#include "ui_signup.h"
#include "login.h"

SignUp::SignUp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SignUp)
{
    ui->setupUi(this);
    setWindowTitle("注册");
    setIcon(20, ui->availableButton, ":/icon/off.png");
    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, this, &SignUp::on_signUpButton_clicked);
}

SignUp::~SignUp()
{
    delete ui;
}

QString SignUp::usernameJudge(const QString& username){
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
    // 检查长度是否在4~20之间
    if (username.length() < 4 || username.length() > 20) {
        return "用户名长度需在4~20个字符！";
    }

    // 检查是否只包含合法字符（字母、数字、下划线）
    for (const QChar &c : username) {
        if (!(c.isLetterOrNumber() || c == '_')) {
            return "用户名不能包含除大小写字母、下划线、数字之外的任何字符！";
        }
    }

    // 如果通过所有检查，返回合法
    return "legal";
}

QString SignUp::passwordJudge(const QString& password){
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
    // 1. 检查长度
    if (password.length() < 8 || password.length() > 12) {
        return "密码长度需在8~12位！";
    }

    // 2. 检查非法字符
    for (const QChar &c : password) {
        if (c == '\'' || c == '\"' || c == '<' || c == '>' || c == ';') {
            return "密码不能包含单引号、双引号、尖括号和分号！";
        }
    }

    // 3. 检查字符类型（至少包含3种）
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (const QChar &c : password) {
        if (c >= 'A' && c <= 'Z') {
            hasUpper = true;
        } else if (c >= 'a' && c <= 'z') {
            hasLower = true;
        } else if (c >= '0' && c <= '9') {
            hasDigit = true;
        } else {
            hasSpecial = true;
        }
    }

    int typeCount = 0;
    if (hasUpper) typeCount++;
    if (hasLower) typeCount++;
    if (hasDigit) typeCount++;
    if (hasSpecial) typeCount++;

    if (typeCount < 3) {
        return "密码必须包含大写字母、小写字母、数字、特殊字符中的至少3种字符类型！";
    }

    // 所有检查通过
    return "legal";
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
    QString userInformation = SignUp::usernameJudge(username);
    if(userInformation != "legal"){
        QMessageBox::critical(this, "用户名不合法", userInformation);
        ui->usernameLineEdit->setText("");
        ui->passwordLineEdit->setText("");
        return ;
    }
    QString password = ui->passwordLineEdit->text();
    QString passInfo = passwordJudge(password);
    if(passInfo != "legal"){
        QMessageBox::critical(this, "密码不合法", passInfo);
        ui->passwordLineEdit->setText("");
        return ;
    }
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

void SignUp::setIcon(int a, QPushButton* button, const QString& filename){
    button->setText("");
    button->setFixedSize(a, a);
    button->setIcon(QIcon(filename));
    button->setIconSize(QSize(button->width(), button->height()));
    button->setStyleSheet(R"(
        QPushButton {
            background-color: transparent; /* 透明背景 */
            border: none; /* 去除边框 */
            padding: 0px; /* 去除内边距，让图标贴合按钮边缘 */
        })"
                          );
}

bool is_on = false;

void SignUp::on_availableButton_clicked()
{
    // 如果当前状态为不可见
    if(!is_on){
        setIcon(20, ui->availableButton, ":/icon/on.png"); // 改为可见图标
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal); // 显示真实密码
    }
    else {
        setIcon(20, ui->availableButton, ":/icon/off.png"); // 改为不可见图标
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password); // 隐藏密码为圆点
    }

    is_on = !is_on; // 切换状态
}
