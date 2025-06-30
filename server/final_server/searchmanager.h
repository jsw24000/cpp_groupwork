#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QString>
#include <QList>
#include <QFileInfo>

class SearchManager
{
public:
    SearchManager();
    QList<QFileInfo> search(const QString& query);

private:
    QList<QFileInfo> searchByNumber(const QString& numberQuery);
    QList<QFileInfo> searchByKeywords(const QString& keywordsQuery);
    bool isNumberQuery(const QString& query);
    int extractPostNumber(const QString& fileName);
    bool containsAllKeywords(const QString& text, const QStringList& keywords);
};

#endif // SEARCHMANAGER_H
