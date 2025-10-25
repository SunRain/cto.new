#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent),
      m_settings("GamePerf", "Manager")
{
}

SettingsManager::~SettingsManager() = default;

bool SettingsManager::isFirstRun() const
{
    return m_settings.value("firstRun", true).toBool();
}

void SettingsManager::setFirstRun(bool firstRun)
{
    m_settings.setValue("firstRun", firstRun);
}

QStringList SettingsManager::getSteamLibraryPaths() const
{
    return m_settings.value("steam/libraryPaths").toStringList();
}

void SettingsManager::setSteamLibraryPaths(const QStringList &paths)
{
    m_settings.setValue("steam/libraryPaths", paths);
}

QString SettingsManager::getLegendaryPath() const
{
    return m_settings.value("legendary/path").toString();
}

void SettingsManager::setLegendaryPath(const QString &path)
{
    m_settings.setValue("legendary/path", path);
}

QStringList SettingsManager::getWinePrefixes() const
{
    return m_settings.value("wine/prefixes").toStringList();
}

void SettingsManager::setWinePrefixes(const QStringList &paths)
{
    m_settings.setValue("wine/prefixes", paths);
}
