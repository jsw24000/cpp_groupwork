#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addpostdialog.h"
#include "postwidget.h"
#include "login.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_addPostButton_clicked();
    void addPost(const QString &content); //把帖子显示在滚动区域

    void on_selfcenterButton_clicked();

    void on_menuButton_clicked();

    void on_infoEditButton_clicked();

private:
    Ui::MainWindow *ui;
    QMap<QString, QLabel*> originalWidgets;       // 存储原始Label指针（field名→Label）
    QMap<QString, QSizePolicy> originalSizePolicies; // 存储原始尺寸策略
    QMap<QString, QSize> originalSizeHints;        // 存储原始大小提示
protected:
    void closeEvent(QCloseEvent *event) override;

};
#endif // MAINWINDOW_H
