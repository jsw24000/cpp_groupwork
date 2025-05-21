#ifndef POSTWIDGET_H
#define POSTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class PostWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PostWidget(const QString &content, QWidget *parent = nullptr);
    ~PostWidget();

private:
    QLabel *contentLabel;
};

#endif // POSTWIDGET_H
