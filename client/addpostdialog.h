#ifndef ADDPOSTDIALOG_H
#define ADDPOSTDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class AddPostDialog;
}

class AddPostDialog : public QDialog
{
    Q_OBJECT

public:
    static int totalPostNumber;
    static void loadTotalPostNumber(); // 从设置加载计数器
    static void saveTotalPostNumber(); // 保存计数器到设置
    static void setTotalPostNumber(int value); //设置总帖子数，手动

    explicit AddPostDialog(QWidget *parent = nullptr);
    ~AddPostDialog();

    void saveTextToFile(const QString& text); //保存文本

    static QString getText(const QString& filePath); //返回任意指定路径的文本
    QString getInputText() const; //返回刚输入的文本，因为.ui是私有的

private slots:
    void on_addKey_clicked();

private:
    Ui::AddPostDialog *ui;
};

#endif // ADDPOSTDIALOG_H
