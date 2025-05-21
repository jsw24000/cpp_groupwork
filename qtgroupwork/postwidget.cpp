#include "postwidget.h"

const int FIXED_HEIGHT = 100;
PostWidget::PostWidget(const QString &content, QWidget *parent)
    : QWidget(parent)
{
    contentLabel = new QLabel(content, this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(contentLabel);

    setLayout(layout);
    setStyleSheet("border: 1px solid black; margin: 5px;");

    setFixedHeight(FIXED_HEIGHT);
}

PostWidget::~PostWidget()
{

}
