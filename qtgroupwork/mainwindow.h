// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "addpostdialog.h"
#include "postwidget.h"
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
    ~MainWindow();

private slots:
    void on_addPostButton_clicked();
    void addPost(const QString &content, const QString &fileName);
    void handlePostsReceived(const QList<Post>& posts);
    void handleDataReceived(const QByteArray& data);
    void handleSocketError(QAbstractSocket::SocketError error);

private:
    Ui::MainWindow *ui;
    Client client;

    void loadAllPosts();
    void handleConnectionError();
    void closeEvent(QCloseEvent *event) override;
};

#endif
