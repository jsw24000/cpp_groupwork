#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addpostdialog.h"
#include "postwidget.h"

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

private:
    Ui::MainWindow *ui;

protected:
    void closeEvent(QCloseEvent *event) override;

};
#endif // MAINWINDOW_H
