#include "sourcemanager.h"

#include <QDir>
#include <QFileInfo>

SourceManager::SourceManager(QObject *parent)
    : QObject(parent)
{
}

SourceManager::~SourceManager() = default;

bool SourceManager::validateSteamPath(const QString &path)
{
    QFileInfo info(path);
    bool valid = info.exists() && info.isDir();
    emit sourceValidated("steam", valid);
    return valid;
}

bool SourceManager::validateLegendaryInstallation(const QString &path)
{
    QFileInfo info(path);
    bool valid = info.exists() && info.isExecutable();
    emit sourceValidated("legendary", valid);
    return valid;
}

bool SourceManager::validateWinePrefix(const QString &path)
{
    QFileInfo info(path);
    bool valid = info.exists() && info.isDir()
            && QFileInfo(path + "/drive_c").exists();
    emit sourceValidated("wine", valid);
    return valid;
}

QStringList SourceManager::autoDetectSteamLibraries()
{
    QStringList paths;
#if defined(Q_OS_LINUX)
    QString defaultPath = QDir::homePath() + "/.steam/steam/steamapps";
    if (QFileInfo::exists(defaultPath)) {
        paths << defaultPath;
    }
#endif
    return paths;
}

QString SourceManager::autoDetectLegendary()
{
    QString path;
#if defined(Q_OS_LINUX)
    QString defaultPath = QDir::homePath() + "/.local/bin/legendary";
    if (QFileInfo::exists(defaultPath)) {
        path = defaultPath;
    }
#endif
    return path;
}
