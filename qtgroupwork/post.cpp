#include "post.h"

Post::Post(const QString& fileName, const QString& content)
    : m_fileName(fileName), m_content(content)
{
}

QString Post::fileName() const
{
    return m_fileName;
}

QString Post::content() const
{
    return m_content;
}

QString Post::title() const
{
    // 假设第一行为标题
    return m_content.section('\n', 0, 0);
}
