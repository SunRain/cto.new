#pragma once

#include "PerformanceProfile.hpp"

#include <QObject>
#include <QProcess>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <memory>

namespace Runtime {
class RunningManager;
}

namespace Launch {

enum class LaunchPlatform {
    Native,
    Steam,
    Epic,
    Wine,
    Proton
};

struct LaunchConfig {
    QString titleId;
    QString displayName;
    QString executable;
    QStringList arguments;
    QString workingDirectory;
    LaunchPlatform platform = LaunchPlatform::Native;
    QString protonPath;
    QString protonVersion;
    QString wineBinary = QStringLiteral("wine");
    QString winePrefix;
    QString steamBinary = QStringLiteral("steam");
    QString epicBinary = QStringLiteral("legendary");
    QVariantMap environment;
    PerformanceProfile profile = PerformanceProfile::defaultProfile();
    bool useGamescope = true;
    bool dryRun = false;
};

class GamescopeLauncher : public QObject {
    Q_OBJECT

public:
    explicit GamescopeLauncher(QObject* parent = nullptr);
    explicit GamescopeLauncher(Runtime::RunningManager* runningManager, QObject* parent = nullptr);
    ~GamescopeLauncher() override;

    void setRunningManager(Runtime::RunningManager* manager);
    Runtime::RunningManager* runningManager() const;

    bool launch(const LaunchConfig& config);
    QStringList buildGamescopeArguments(const PerformanceProfile& profile) const;
    QVariantMap buildEnvironment(const LaunchConfig& config) const;
    QString buildGamescopeCommand(const LaunchConfig& config) const;

    Q_INVOKABLE bool isGameRunning(const QString& titleId) const;
    Q_INVOKABLE void exitToShell(const QString& titleId = QString());
    Q_INVOKABLE void toggleOverlay(const QString& titleId);

signals:
    void launchStarted(const QString& titleId, qint64 pid);
    void launchFailed(const QString& titleId, const QString& error);
    void gameExited(const QString& titleId, int exitCode, const QString& exitStatus);
    void exitToShellRequested(const QString& titleId);
    void overlayToggleRequested(const QString& titleId);

private slots:
    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    struct RunningProcess {
        QString titleId;
        QString displayName;
        std::unique_ptr<QProcess> process;
        LaunchConfig config;
    };

    QStringList constructFullCommand(const LaunchConfig& config) const;
    QString platformToString(LaunchPlatform platform) const;
    bool supportsSuspend(LaunchPlatform platform) const;
    QString suspendUnsupportedReason(LaunchPlatform platform) const;

    Runtime::RunningManager* m_runningManager = nullptr;
    QHash<QString, std::unique_ptr<RunningProcess>> m_runningProcesses;
};

LaunchPlatform parsePlatform(const QString& platformStr);
QString platformToString(LaunchPlatform platform);

} // namespace Launch
