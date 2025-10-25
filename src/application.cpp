#include "application.h"
#include "settingsmanager.h"

#include <QProcess>
#include <QDebug>

Application::Application(QObject *parent)
    : QObject(parent),
      m_settings(new SettingsManager(this)),
      m_firstRun(true),
      m_daemonAvailable(false)
{
    checkFirstRun();
    initializeDaemon();
}

Application::~Application() = default;

bool Application::isFirstRun() const
{
    return m_firstRun;
}

bool Application::isDaemonAvailable() const
{
    return m_daemonAvailable;
}

void Application::checkFirstRun()
{
    m_firstRun = m_settings->isFirstRun();
    emit firstRunChanged();
}

void Application::initializeDaemon()
{
    m_daemonAvailable = checkDaemonStatus();
    emit daemonAvailableChanged();
}

void Application::completeFirstRun()
{
    m_settings->setFirstRun(false);
    m_firstRun = false;
    emit firstRunChanged();
}

bool Application::checkDaemonStatus()
{
    QProcess process;
    process.start("systemctl", {"is-active", "--quiet", "gameperf-daemon.service"});
    bool daemonRunning = process.waitForFinished() && process.exitCode() == 0;
    m_daemonAvailable = daemonRunning;
    emit daemonAvailableChanged();
    return daemonRunning;
}

QStringList Application::steamLibraryPaths() const
{
    return m_settings->getSteamLibraryPaths();
}

QString Application::legendaryPath() const
{
    return m_settings->getLegendaryPath();
}

QStringList Application::winePrefixes() const
{
    return m_settings->getWinePrefixes();
}

void Application::saveSteamLibraries(const QStringList &paths)
{
    m_settings->setSteamLibraryPaths(paths);
}

void Application::saveLegendaryPath(const QString &path)
{
    m_settings->setLegendaryPath(path);
}

void Application::saveWinePrefixes(const QStringList &paths)
{
    m_settings->setWinePrefixes(paths);
}
