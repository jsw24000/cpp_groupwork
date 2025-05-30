#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QMap>
#include <QStringList>
#include <QSizePolicy>
#include <QSize>
#include <QLineEdit>
#include <QLayoutItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置滚动区域的布局
    QVBoxLayout *layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(5);
    ui->scrollAreaWidgetContents->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addPostButton_clicked() //如果点击添加按钮
{
    AddPostDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) { //如果确认了
        dialog.saveTextToFile(dialog.getInputText()); //保存
        QString content = "空空如也";
        QString fileName = "post_" + QString::number(AddPostDialog::totalPostNumber - 1) + ".txt";
        QString filePath = "C:/Users/69156/Desktop/cpp_groupwork/main/qtgroupwork/src/posts/" + fileName;
        content=AddPostDialog::getText(filePath);
        addPost(content);
    }
}

void MainWindow::addPost(const QString &content) //把帖子显示在滚动区域
{
    PostWidget *postWidget = new PostWidget(content, ui->scrollAreaWidgetContents);
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    layout->addWidget(postWidget);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    AddPostDialog::saveTotalPostNumber(); // 关闭时保存计数器
    event->accept();
}

QMap<QString, QWidget*> originalWidgets;
bool isEditable = false;

void MainWindow::on_selfcenterButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_menuButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}



void MainWindow::on_infoEditButton_clicked()
{
    isEditable = !isEditable;
    const QStringList fields = {"username", "password", "number", "school", "phone"};
    QPushButton* infoEdit = findChild<QPushButton*>("infoEditButton");
    if (infoEdit) infoEdit->setText(isEditable ? "提交修改" : "更改信息"); // 让按钮根据状态改变text信息


    for (const QString& field : fields) {
        const QString labelName = field + "Label";
        const QString lineEditName = field + "LineEdit";

        if (isEditable) { // 进入编辑模式：替换Label为LineEdit
            QLabel* originalLabel = findChild<QLabel*>(labelName); // 找到对应的QLabel
            if (!originalLabel || originalWidgets.contains(field)) continue;

            // 保存当前QLabel的原始状态（到相应QMap中）
            originalWidgets[field] = originalLabel;
            originalSizePolicies[field] = originalLabel->sizePolicy();
            originalSizeHints[field] = originalLabel->sizeHint();

            // 创建LineEdit并继承Label的尺寸属性
            QLineEdit* edit = new QLineEdit(originalLabel->text());
            edit->setObjectName(lineEditName);
            edit->setSizePolicy(originalSizePolicies[field]); // 统一尺寸策略
            edit->setMinimumSize(originalSizeHints[field]);   // 统一最小尺寸

            // 特殊处理密码字段（变为不可见）
            if (field == "password") {
                edit->setEchoMode(QLineEdit::Password);
            }

            // 替换控件并隐藏原始Label
            ui->gridLayout->replaceWidget(originalLabel, edit);
            originalLabel->hide();
        } else { // 提交修改：替换LineEdit为Label
            QLineEdit* edit = findChild<QLineEdit*>(lineEditName);
            QLabel* originalLabel = originalWidgets.value(field); // 从成员变量获取
            if (!edit || !originalLabel) continue;

            // 更新Label文本并恢复原始尺寸属性
            originalLabel->setText(edit->text());
            originalLabel->setSizePolicy(originalSizePolicies[field]);
            originalLabel->setMinimumSize(originalSizeHints[field]);

            if(field == "password") originalLabel->setText("********");
            // 替换控件并销毁LineEdit
            ui->gridLayout->replaceWidget(edit, originalLabel);
            originalLabel->show();
            edit->deleteLater();  // 彻底移除LineEdit

            // 清理临时存储
            originalWidgets.remove(field);
            originalSizePolicies.remove(field);
            originalSizeHints.remove(field);
        }
    }
}
