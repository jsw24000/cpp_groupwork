#ifndef POST_H
#define POST_H

#include <QString>

class Post
{
public:
    Post(const QString& fileName = "", const QString& content = "");

    QString fileName() const;
    QString content() const;
    QString title() const; // 从内容中提取标题

private:
    QString m_fileName;
    QString m_content;
};

#endif

