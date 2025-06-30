#include "searchmanager.h"
#include <QDebug>

SearchManager::SearchManager()
{
}

QList<Post> SearchManager::search(const QString& query, const QList<Post>& allPosts)
{
    if (query.trimmed().isEmpty()) {
        return allPosts; // 如果查询为空，返回所有帖子
    }

    QString trimmedQuery = query.trimmed();

    // 判断是否为号码搜索
    if (isNumberQuery(trimmedQuery)) {
        return searchByNumber(trimmedQuery, allPosts);
    }

    else {
        return searchByKeywords(trimmedQuery, allPosts);
    }
}

QList<Post> SearchManager::searchByNumber(const QString& numberQuery, co
                                                                          nst QList<Post>& allPosts)
{
    QList<Post> results;

    // 提取号码（去掉开头的#）
    QString numberStr = numberQuery.mid(1); // 去掉#
    bool ok;
    int searchNumber = numberStr.toInt(&ok);

    if (!ok) {
        qDebug() << "Invalid number format:" << numberQuery;
        return results;
    }

    // 在所有帖子中查找匹配的号码
    for (const Post& post : allPosts) {
        int postNumber = extractPostNumber(post.fileName());
        if (postNumber == searchNumber) {
            results.append(post);
            qDebug() << "Found post by number:" << searchNumber << "filename:" << post.fileName();
        }
    }

    return results;
}

QList<Post> SearchManager::searchByKeywords(const QString& keywordsQuery, const QList<Post>& allPosts)
{
    QList<Post> results;

    // 将关键词字符串按空格分割
    QStringList keywords = keywordsQuery.split(' ', Qt::SkipEmptyParts);

    if (keywords.isEmpty()) {
        return results;
    }

    // 在所有帖子中查找包含所有关键词的帖子
    for (const Post& post : allPosts) {
        if (containsAllKeywords(post.content(), keywords)) {
            results.append(post);
            qDebug() << "Found post by keywords:" << keywords << "filename:" << post.fileName();
        }
    }

    return results;
}

bool SearchManager::isNumberQuery(const QString& query)
{
    // 检查是否以#开头，且后面全是数字
    if (!query.startsWith('#') || query.length() <= 1) {
        return false;
    }

    QString numberPart = query.mid(1);
    QRegularExpression regex("^\\d+$");
    return regex.match(numberPart).hasMatch();
}

int SearchManager::extractPostNumber(const QString& fileName)
{
    // 从文件名中提取数字，假设格式为 "post_数字.txt" 或类似格式
    QRegularExpression regex("(\\d+)");
    QRegularExpressionMatch match = regex.match(fileName);

    if (match.hasMatch()) {
        bool ok;
        int number = match.captured(1).toInt(&ok);
        if (ok) {
            return number;
        }
    }

    return -1; // 如果无法提取数字，返回-1
}

bool SearchManager::containsAllKeywords(const QString& text, const QStringList& keywords)
{
    QString lowerText = text.toLower(); // 转换为小写进行不区分大小写的比较

    // 检查文本是否包含所有关键词
    for (const QString& keyword : keywords) {
        QString lowerKeyword = keyword.toLower();
        if (!lowerText.contains(lowerKeyword)) {
            return false; // 如果有任何一个关键词不包含，返回false
        }
    }

    return true; // 所有关键词都包含
}
