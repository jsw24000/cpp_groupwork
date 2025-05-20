#ifndef POSTWIDGET_H
#define POSTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

namespace Ui {
class PostWidget;
}

class PostWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PostWidget(const QString &content, QWidget *parent = nullptr);
    ~PostWidget();

private:
    // QLabel *contentLabel;
    Ui::PostWidget *ui;
};

#endif // POSTWIDGET_H
