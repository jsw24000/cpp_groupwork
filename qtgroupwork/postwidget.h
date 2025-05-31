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
    const QString my_filename;
    explicit PostWidget(const QString &content, const QString fileName, QWidget *parent = nullptr);
    ~PostWidget();

private slots:
    void on_commentButton_clicked();

private:
    // QLabel *contentLabel;
    Ui::PostWidget *ui;
};

#endif // POSTWIDGET_H
