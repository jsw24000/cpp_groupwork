#include "addpostdialog.h"
#include "ui_addpostdialog.h"
#include "client.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>


int AddPostDialog::totalPostNumber = 0;

// 从QSettings加载计数器值
void AddPostDialog::loadTotalPostNumber() {
    QSettings settings("QTQ-group", "ShuKeng");
    totalPostNumber = settings.value("totalPostNumber", 0).toInt();
    qDebug() << "Loaded totalPostNumber:" << totalPostNumber;
}

// 将计数器值保存到QSettings
void AddPostDialog::saveTotalPostNumber() {
    QSettings settings("QTQ-group", "ShuKeng");
    settings.setValue("totalPostNumber", totalPostNumber);
    qDebug() << "Saved totalPostNumber:" << totalPostNumber;
}

//手动设置
void AddPostDialog::setTotalPostNumber(int value) {
    totalPostNumber = value;
    saveTotalPostNumber(); // 立即保存新值
}

AddPostDialog::AddPostDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddPostDialog)
{
    ui->setupUi(this);
}

AddPostDialog::~AddPostDialog()
{
    delete ui;
}



void AddPostDialog::on_addKey_clicked()
{
    // 获取当前 QTextEdit 中的文本
    QString currentText = ui->inputKey->toPlainText();
    currentText.append("  #");
    // 更新 QTextEdit 中的文本
    ui->inputKey->setPlainText(currentText);
    // 将光标移动到文本末尾
    QTextCursor cursor = ui->inputKey->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->inputKey->setTextCursor(cursor);
}

QString AddPostDialog::getInputText() const //获取刚输入的文本
{
    return ui->inputText->toPlainText();
}

// void AddPostDialog::saveTextToFile(const QString& text)
// {
//     Client client;
//     client.connectToServer("192.168.43.242", 1234); // 连接到服务器

//     // 构造一个包含帖子内容的请求
//     QByteArray request = text.toUtf8();
//     client.sendRequest(request);

//     connect(&client, &Client::dataReceived, [this](const QByteArray& data) {
//         QString response = QString(data);
//         if (response == "Success") {
//             QMessageBox::information(this, "发布成功", "文本已成功发布了！");
//         } else {
//             QMessageBox::critical(this, "发布失败", "糟糕！请再试试看");
//         }
//     });
// }

QString AddPostDialog::getText(const QString& filePath){ //返回任意指定路径的文本
    QFile file(filePath);
    QString fileContent;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        fileContent = in.readAll();
        file.close();
    }

    return fileContent;
}


