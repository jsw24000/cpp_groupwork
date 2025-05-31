#ifndef COMMENT_H
#define COMMENT_H

#include <QWidget>
#include <QString>
#include <QFile>

namespace Ui {
class Comment;
}

class Comment : public QWidget
{
    Q_OBJECT

public:
    QString my_filename;
    explicit Comment(const QString &filename, QWidget *parent = nullptr);
    ~Comment();
    void loadReplies();

private slots:
    void on_pushButton_clicked();

private:
    void loadText(const QString &filename);
    Ui::Comment *ui;
};

#endif // COMMENT_H
