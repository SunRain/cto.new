#include "DataModel.h"

#include <QVariantMap>

DataModel::DataModel(QObject *parent)
    : QObject(parent)
{
    // Seed with placeholder entries to keep UI responsive during development.
    addPlaceholderItem(QStringLiteral("Welcome"), QStringLiteral("Explore curated media."));
    addPlaceholderItem(QStringLiteral("Getting Started"), QStringLiteral("Import your library to continue."));
}

QVariantList DataModel::items() const
{
    return m_items;
}

int DataModel::count() const
{
    return m_items.count();
}

void DataModel::addPlaceholderItem(const QString &title, const QString &subtitle)
{
    QVariantMap entry;
    entry.insert(QStringLiteral("title"), title);
    entry.insert(QStringLiteral("subtitle"), subtitle);
    m_items.append(entry);

    emit itemsChanged();
    emit countChanged();
}

void DataModel::clearItems()
{
    if (m_items.isEmpty()) {
        return;
    }

    m_items.clear();
    emit itemsChanged();
    emit countChanged();
}
