#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QObject>
#include <QVariantList>

class DataModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit DataModel(QObject *parent = nullptr);
    ~DataModel() override = default;

    QVariantList items() const;
    int count() const;

    Q_INVOKABLE void addPlaceholderItem(const QString &title, const QString &subtitle);
    Q_INVOKABLE void clearItems();

signals:
    void itemsChanged();
    void countChanged();

private:
    QVariantList m_items;
};

#endif
