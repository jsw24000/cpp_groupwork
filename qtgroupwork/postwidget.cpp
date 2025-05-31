#include "postwidget.h"
#include "ui_postwidget.h"
#include "comment.h"
#include "reply.h"

const int FIXED_HEIGHT = 100;
PostWidget::PostWidget(const QString &content, const QString fileName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PostWidget)
    , my_filename(fileName)
{
    ui->setupUi(this);
    // contentLabel = new QLabel(content, this);
    // QVBoxLayout *layout = new QVBoxLayout(this);
    // 设置QTextEdit的内容
    ui->textEdit->setPlainText(content);
    // 设置为只读模式（可选）
    ui->textEdit->setReadOnly(true);

    // layout->addWidget(contentLabel);
    // setLayout(layout);
    setStyleSheet("border: 1px solid black; margin: 5px;");

    setFixedHeight(FIXED_HEIGHT);
}

PostWidget::~PostWidget()
{
    delete ui;
}

void PostWidget::on_commentButton_clicked()
{
    Comment *comment = new Comment(my_filename);
    comment->show();
    comment->loadReplies();
}

