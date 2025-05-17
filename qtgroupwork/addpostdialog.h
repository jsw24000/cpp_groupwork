#ifndef ADDPOSTDIALOG_H
#define ADDPOSTDIALOG_H

#include <QDialog>

namespace Ui {
class AddPostDialog;
}

class AddPostDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPostDialog(QWidget *parent = nullptr);
    ~AddPostDialog();

private slots:

private:
    Ui::AddPostDialog *ui;
};

#endif // ADDPOSTDIALOG_H
