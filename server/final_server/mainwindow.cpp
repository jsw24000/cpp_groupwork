#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , connectedClients(0)
{
    ui->setupUi(this);
    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 设置窗口标题和大小
    setWindowTitle("服务器监控面板 - TCP服务器");
    resize(900, 700);

    // 创建中央窗口部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 创建顶部状态区域
    QWidget *statusWidget = new QWidget();
    QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);

    // 服务器状态标签
    statusLabel = new QLabel("服务器状态：正在启动...");
    statusLabel->setStyleSheet(
        "QLabel { "
        "font-weight: bold; "
        "color: #2196F3; "
        "padding: 8px; "
        "border: 1px solid #ddd; "
        "border-radius: 4px; "
        "background-color: #f5f5f5; "
        "}"
        );

    // 客户端连接数标签
    clientCountLabel = new QLabel("连接客户端：0");
    clientCountLabel->setStyleSheet(
        "QLabel { "
        "font-weight: bold; "
        "color: #4CAF50; "
        "padding: 8px; "
        "border: 1px solid #ddd; "
        "border-radius: 4px; "
        "background-color: #f5f5f5; "
        "}"
        );

    // 清空日志按钮
    clearLogButton = new QPushButton("清空日志");
    clearLogButton->setStyleSheet(
        "QPushButton { "
        "background-color: #FF9800; "
        "color: white; "
        "border: none; "
        "padding: 8px 16px; "
        "border-radius: 4px; "
        "font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "background-color: #F57C00; "
        "} "
        "QPushButton:pressed { "
        "background-color: #E65100; "
        "}"
        );

    connect(clearLogButton, &QPushButton::clicked, this, &MainWindow::clearLog);

    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(clientCountLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(clearLogButton);

    mainLayout->addWidget(statusWidget);

    // 创建日志显示区域标签
    QLabel *logLabel = new QLabel("服务器日志：");
    logLabel->setStyleSheet("QLabel { font-weight: bold; color: #333; margin-top: 10px; }");
    mainLayout->addWidget(logLabel);

    // 创建日志显示区域
    logDisplay = new QTextEdit();
    logDisplay->setReadOnly(true);
    logDisplay->setStyleSheet(
        "QTextEdit { "
        "background-color: #1e1e1e; "
        "color: #d4d4d4; "
        "border: 1px solid #333; "
        "border-radius: 4px; "
        "font-family: 'Consolas', 'Monaco', 'Courier New', monospace; "
        "font-size: 9pt; "
        "padding: 8px; "
        "}"
        );

    mainLayout->addWidget(logDisplay);

    // 显示欢迎信息
    appendLog("=== 服务器监控面板启动 ===");
    appendLog("系统初始化中...");
}

void MainWindow::appendLog(const QString &message)
{
    // 获取当前时间戳
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    // 格式化消息
    QString formattedMessage = QString("<span style='color: #569cd6;'>[%1]</span> %2")
                                   .arg(timestamp, message);

    // 添加到日志显示区域
    logDisplay->append(formattedMessage);

    // 自动滚动到底部
    QScrollBar *scrollBar = logDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void MainWindow::updateServerStatus(const QString &status, bool isRunning)
{
    QString color = isRunning ? "#4CAF50" : "#f44336";
    statusLabel->setText(QString("服务器状态：%1").arg(status));
    statusLabel->setStyleSheet(
        QString("QLabel { "
                "font-weight: bold; "
                "color: %1; "
                "padding: 8px; "
                "border: 1px solid #ddd; "
                "border-radius: 4px; "
                "background-color: #f5f5f5; "
                "}").arg(color)
        );
}

void MainWindow::clearLog()
{
    logDisplay->clear();
    appendLog("=== 日志已清空 ===");
}

void MainWindow::onServerStarted()
{
    updateServerStatus("运行中 (端口: 1234)", true);
    appendLog("<span style='color: #4CAF50;'>✓ 服务器启动成功，监听端口：1234</span>");
}

void MainWindow::onServerError(const QString &error)
{
    updateServerStatus("启动失败", false);
    appendLog(QString("<span style='color: #f44336;'>✗ 服务器错误：%1</span>").arg(error));
}

void MainWindow::onClientConnected(const QString &clientInfo)
{
    connectedClients++;
    clientCountLabel->setText(QString("连接客户端：%1").arg(connectedClients));
    appendLog(QString("<span style='color: #2196F3;'>+ 客户端连接：%1</span>").arg(clientInfo));
}

void MainWindow::onClientDisconnected(const QString &clientInfo)
{
    connectedClients = qMax(0, connectedClients - 1);
    clientCountLabel->setText(QString("连接客户端：%1").arg(connectedClients));
    appendLog(QString("<span style='color: #ff9800;'>- 客户端断开：%1</span>").arg(clientInfo));
}

void MainWindow::onRequestReceived(const QString &requestInfo)
{
    appendLog(QString("<span style='color: #e91e63;'>📨 请求处理：%1</span>").arg(requestInfo));
}
