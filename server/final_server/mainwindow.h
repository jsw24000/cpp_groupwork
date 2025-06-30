#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

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

    // 添加日志显示方法
    void appendLog(const QString &message);
    void updateServerStatus(const QString &status, bool isRunning);

public slots:
    void clearLog();
    void onServerStarted();
    void onServerError(const QString &error);
    void onClientConnected(const QString &clientInfo);
    void onClientDisconnected(const QString &clientInfo);
    void onRequestReceived(const QString &requestInfo);

private:
    Ui::MainWindow *ui;

    // GUI组件
    QTextEdit *logDisplay;
    QLabel *statusLabel;
    QLabel *clientCountLabel;
    QPushButton *clearLogButton;

    // 统计信息
    int connectedClients;

    void setupUI();
};

#endif // MAINWINDOW_H
