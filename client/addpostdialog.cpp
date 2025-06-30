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
    setWindowTitle("发帖");
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


