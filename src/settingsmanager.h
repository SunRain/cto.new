#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);
    ~SettingsManager();

    bool isFirstRun() const;
    void setFirstRun(bool firstRun);

    QStringList getSteamLibraryPaths() const;
    void setSteamLibraryPaths(const QStringList &paths);

    QString getLegendaryPath() const;
    void setLegendaryPath(const QString &path);

    QStringList getWinePrefixes() const;
    void setWinePrefixes(const QStringList &paths);

private:
    QSettings m_settings;
};

#endif // SETTINGSMANAGER_H
