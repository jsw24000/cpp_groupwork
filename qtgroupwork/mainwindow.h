#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "addpostdialog.h"
#include "postwidget.h"
#include "login.h"
#include "client.h"
#include <QList>
#include "post.h"
#include <QAbstractSocket>


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
    QString myusername = "default user";
    void setUserName(const QString& username);
    ~MainWindow();
    Client client;
    bool isFavoritesView = false; // 记录当前视图状态

private slots:

    void on_addPostButton_clicked();
    void addPost(const QString &content, const QString &fileName);
    void handlePostsReceived(const QList<Post>& posts);
    void handleDataReceived(const QByteArray& data);
    void handleSocketError(QAbstractSocket::SocketError error);

    void on_selfcenterButton_clicked();

    void on_menuButton_clicked();

    void on_infoEditButton_clicked();

    void on_pushButton_clicked();

    void on_checkloveButton_clicked();

private:
    Ui::MainWindow *ui;
    void setIcon(int a, QPushButton* button, const QString& filename); // 为按钮设置图标
    QMap<QString, QLabel*> originalWidgets;       // 存储原始Label指针（field名→Label）
    QMap<QString, QSizePolicy> originalSizePolicies; // 存储原始尺寸策略
    QMap<QString, QSize> originalSizeHints;        // 存储原始大小提示

    void loadAllPosts();
    void handleConnectionError();
    void closeEvent(QCloseEvent *event) override;

};
#endif // MAINWINDOW_H
