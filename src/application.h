#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QString>
#include <QStringList>

class SettingsManager;

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool firstRun READ isFirstRun NOTIFY firstRunChanged)
    Q_PROPERTY(bool daemonAvailable READ isDaemonAvailable NOTIFY daemonAvailableChanged)

public:
    explicit Application(QObject *parent = nullptr);
    ~Application();

    bool isFirstRun() const;
    bool isDaemonAvailable() const;

    Q_INVOKABLE void completeFirstRun();
    Q_INVOKABLE bool checkDaemonStatus();

    Q_INVOKABLE QStringList steamLibraryPaths() const;
    Q_INVOKABLE QString legendaryPath() const;
    Q_INVOKABLE QStringList winePrefixes() const;

    Q_INVOKABLE void saveSteamLibraries(const QStringList &paths);
    Q_INVOKABLE void saveLegendaryPath(const QString &path);
    Q_INVOKABLE void saveWinePrefixes(const QStringList &paths);

signals:
    void firstRunChanged();
    void daemonAvailableChanged();

private:
    SettingsManager *m_settings;
    bool m_firstRun;
    bool m_daemonAvailable;

    void checkFirstRun();
    void initializeDaemon();
};

#endif // APPLICATION_H
