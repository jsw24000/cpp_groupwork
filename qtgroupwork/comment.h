#ifndef COMMENT_H
#define COMMENT_H

#include <QWidget>
#include <QString>
#include <QFile>
#include "client.h"
#include <QListWidget>

namespace Ui {
class Comment;
}

class Comment : public QWidget
{
    Q_OBJECT

public:
    explicit Comment(QString postcontent, const QString &filename, Client* client, QWidget *parent = nullptr);
    ~Comment();
    void loadReplies();
    QString myusername;

private slots:
    void on_pushButton_clicked();
    void handleCommentsReceived(const QByteArray& data);
    void handleDataReceived(const QByteArray& data);

private:
    void saveComment(const QString& username, const QString& content);
    void loadComments();
    void requestComments();
    void requestPostContent();
    void loadText(const QString &filename);
    Ui::Comment *ui;
    QString my_filename;
    Client* m_client;
    QListWidget* commentList;
};

#endif // COMMENT_H
