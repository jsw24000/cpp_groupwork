#ifndef POSTWIDGET_H
#define POSTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QJsonObject>
#include "client.h"

namespace Ui {
class PostWidget;
}

class PostWidget : public QWidget
{
    Q_OBJECT
public:
    QString myusername;
    Client* client_ptr;
    const QString my_filename;
    explicit PostWidget(const QString &content, const QString fileName, QWidget *parent = nullptr);
    ~PostWidget();

private slots:
    void on_commentButton_clicked();

    void on_saveButton_clicked();

private:
    // QLabel *contentLabel;
    Ui::PostWidget *ui;
};

#endif // POSTWIDGET_H
