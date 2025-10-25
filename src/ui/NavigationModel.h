#ifndef NAVIGATIONMODEL_H
#define NAVIGATIONMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>

class NavigationModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString currentRoute READ currentRoute NOTIFY currentRouteChanged)

public:
    explicit NavigationModel(QObject *parent = nullptr);
    ~NavigationModel() override = default;

    int currentIndex() const;
    void setCurrentIndex(int index);

    QString currentRoute() const;

    Q_INVOKABLE void navigateTo(const QString &route);
    Q_INVOKABLE QString getRouteForIndex(int index) const;

signals:
    void currentIndexChanged();
    void currentRouteChanged();

private:
    int m_currentIndex;
    QStringList m_routes;
};

#endif
