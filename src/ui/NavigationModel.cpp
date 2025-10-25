#include "NavigationModel.h"

#include <QStringList>

NavigationModel::NavigationModel(QObject *parent)
    : QObject(parent), m_currentIndex(0)
{
    m_routes << QStringLiteral("home")
             << QStringLiteral("library")
             << QStringLiteral("favorites")
             << QStringLiteral("recent")
             << QStringLiteral("running")
             << QStringLiteral("settings");
}

int NavigationModel::currentIndex() const
{
    return m_currentIndex;
}

void NavigationModel::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_routes.size() || index == m_currentIndex) {
        return;
    }

    m_currentIndex = index;
    emit currentIndexChanged();
    emit currentRouteChanged();
}

QString NavigationModel::currentRoute() const
{
    return m_routes.value(m_currentIndex, QStringLiteral("home"));
}

void NavigationModel::navigateTo(const QString &route)
{
    const int index = m_routes.indexOf(route.toLower());
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

QString NavigationModel::getRouteForIndex(int index) const
{
    return m_routes.value(index);
}
