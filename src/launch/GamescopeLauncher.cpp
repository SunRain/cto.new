#include "GamescopeLauncher.hpp"

#include "runtime/RunningManager.hpp"

#include <QDebug>
#include <QProcessEnvironment>
#include <QtGlobal>

namespace Launch {

namespace {
QString normalize(const QString& value)
{
    return value.trimmed().toLower();
}

QString joinCommandForLog(const QStringList& command)
{
    QStringList quoted;
    quoted.reserve(command.size());
    for (const QString& part : command) {
        if (part.contains(' ')) {
            quoted << QStringLiteral("\"%1\"").arg(part);
        } else {
            quoted << part;
        }
    }
    return quoted.join(' ');
}

} // namespace

GamescopeLauncher::GamescopeLauncher(QObject* parent)
    : QObject(parent)
{
}

GamescopeLauncher::GamescopeLauncher(Runtime::RunningManager* runningManager, QObject* parent)
    : QObject(parent)
    , m_runningManager(runningManager)
{
}

GamescopeLauncher::~GamescopeLauncher()
{
    for (auto it = m_runningProcesses.begin(); it != m_runningProcesses.end(); ++it) {
        if (it.value() && it.value()->process) {
            QProcess* proc = it.value()->process.get();
            if (proc->state() != QProcess::NotRunning) {
                proc->terminate();
                if (!proc->waitForFinished(2000)) {
                    proc->kill();
                }
            }
        }
    }
}

void GamescopeLauncher::setRunningManager(Runtime::RunningManager* manager)
{
    m_runningManager = manager;
}

Runtime::RunningManager* GamescopeLauncher::runningManager() const
{
    return m_runningManager;
}

bool GamescopeLauncher::launch(const LaunchConfig& config)
{
    if (config.titleId.isEmpty()) {
        emit launchFailed(config.titleId, QStringLiteral("Missing title identifier"));
        return false;
    }

    if (config.executable.isEmpty()) {
        emit launchFailed(config.titleId, QStringLiteral("Missing executable"));
        return false;
    }

    if (m_runningProcesses.contains(config.titleId)) {
        emit launchFailed(config.titleId, QStringLiteral("Title is already running"));
        return false;
    }

    LaunchConfig effectiveConfig = config;
    if (!effectiveConfig.profile.isValid()) {
        effectiveConfig.profile = PerformanceProfile::defaultProfile();
    }

    QStringList command = constructFullCommand(effectiveConfig);
    if (command.isEmpty()) {
        emit launchFailed(config.titleId, QStringLiteral("Unable to construct launch command"));
        return false;
    }

    QVariantMap envMap = buildEnvironment(effectiveConfig);

    if (effectiveConfig.dryRun) {
        QString commandLog = joinCommandForLog(command);
        qInfo() << "[GamescopeLauncher] Dry-run launch for" << effectiveConfig.titleId << ':' << commandLog;
        qInfo() << "[GamescopeLauncher] Environment:" << envMap;
        return true;
    }

    auto process = std::make_unique<QProcess>();
    QProcess* processPtr = process.get();

    QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
    for (auto it = envMap.constBegin(); it != envMap.constEnd(); ++it) {
        procEnv.insert(it.key(), it.value().toString());
    }
    processPtr->setProcessEnvironment(procEnv);

    if (!effectiveConfig.workingDirectory.isEmpty()) {
        processPtr->setWorkingDirectory(effectiveConfig.workingDirectory);
    }

    connect(processPtr, &QProcess::started, this, &GamescopeLauncher::onProcessStarted);
    connect(processPtr, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &GamescopeLauncher::onProcessFinished);
    connect(processPtr, &QProcess::errorOccurred, this, &GamescopeLauncher::onProcessError);

    auto runningProcess = std::make_unique<RunningProcess>();
    runningProcess->titleId = effectiveConfig.titleId;
    runningProcess->displayName = effectiveConfig.displayName;
    runningProcess->process = std::move(process);
    runningProcess->config = effectiveConfig;

    QString program = command.value(0);
    QStringList arguments = command.mid(1);
    processPtr->setProperty("titleId", effectiveConfig.titleId);
    m_runningProcesses.insert(effectiveConfig.titleId, std::move(runningProcess));

    processPtr->start(program, arguments);

    if (!processPtr->waitForStarted(1000)) {
        QString error = processPtr->errorString();
        emit launchFailed(effectiveConfig.titleId, error.isEmpty() ? QStringLiteral("Failed to start process") : error);
        m_runningProcesses.remove(effectiveConfig.titleId);
        return false;
    }

    return true;
}

QStringList GamescopeLauncher::constructFullCommand(const LaunchConfig& config) const
{
    QStringList command;

    if (config.useGamescope) {
        command << QStringLiteral("gamescope");
        command << buildGamescopeArguments(config.profile);
    }

    QStringList appCommand;
    switch (config.platform) {
    case LaunchPlatform::Steam: {
        QString steamBinary = config.steamBinary.isEmpty() ? QStringLiteral("steam") : config.steamBinary;
        appCommand << steamBinary << QStringLiteral("-applaunch") << config.executable;
        appCommand << config.arguments;
        break;
    }
    case LaunchPlatform::Epic: {
        QString epicBinary = config.epicBinary.isEmpty() ? QStringLiteral("legendary") : config.epicBinary;
        appCommand << epicBinary << QStringLiteral("launch") << config.executable;
        appCommand << config.arguments;
        break;
    }
    case LaunchPlatform::Proton: {
        QString protonBinary = config.protonPath.isEmpty() ? QStringLiteral("proton") : config.protonPath;
        appCommand << protonBinary << QStringLiteral("run") << config.executable;
        appCommand << config.arguments;
        break;
    }
    case LaunchPlatform::Wine: {
        QString wineBinary = config.wineBinary.isEmpty() ? QStringLiteral("wine") : config.wineBinary;
        appCommand << wineBinary << config.executable;
        appCommand << config.arguments;
        break;
    }
    case LaunchPlatform::Native:
    default: {
        appCommand << config.executable;
        appCommand << config.arguments;
        break;
    }
    }

    command << appCommand;
    return command;
}

QStringList GamescopeLauncher::buildGamescopeArguments(const PerformanceProfile& profile) const
{
    PerformanceProfile effective = profile.isValid() ? profile : PerformanceProfile::defaultProfile();

    QStringList args;
    args << QStringLiteral("--exit-with-child");

    if (effective.fpsCap > 0) {
        const QString fpsValue = QString::number(effective.fpsCap);
        args << QStringLiteral("--fps") << fpsValue;
        args << QStringLiteral("--game-fps") << fpsValue;
    }

    if (effective.renderWidth > 0 && effective.renderHeight > 0) {
        args << QStringLiteral("-w") << QString::number(effective.renderWidth);
        args << QStringLiteral("-h") << QString::number(effective.renderHeight);
    }

    if (effective.outputWidth > 0 && effective.outputHeight > 0) {
        args << QStringLiteral("-W") << QString::number(effective.outputWidth);
        args << QStringLiteral("-H") << QString::number(effective.outputHeight);
    }

    switch (effective.windowMode) {
    case WindowMode::Fullscreen:
        args << QStringLiteral("--fullscreen");
        break;
    case WindowMode::Borderless:
        args << QStringLiteral("--borderless");
        break;
    case WindowMode::Windowed:
        args << QStringLiteral("--windowed");
        break;
    }

    switch (effective.scaling) {
    case ScalingMode::FSR:
        args << QStringLiteral("--fsr");
        if (effective.fsrSharpness >= 0 && effective.fsrSharpness <= 20) {
            args << QStringLiteral("--fsr-sharpness") << QString::number(effective.fsrSharpness);
        }
        break;
    case ScalingMode::Integer:
        args << QStringLiteral("--integer-scale");
        break;
    case ScalingMode::Native:
    default:
        break;
    }

    if (effective.vsync) {
        args << QStringLiteral("--vsync");
    } else {
        args << QStringLiteral("--no-vsync");
    }

    if (effective.allowTearing) {
        args << QStringLiteral("--allow-tearing");
    }

    if (effective.refreshRate > 0) {
        args << QStringLiteral("--preferred-refresh") << QString::number(effective.refreshRate);
    }

    args << QStringLiteral("--mangoapp");
    args << QStringLiteral("--");
    return args;
}

QVariantMap GamescopeLauncher::buildEnvironment(const LaunchConfig& config) const
{
    QVariantMap env = config.environment;

    if (config.platform == LaunchPlatform::Proton) {
        if (!config.protonVersion.isEmpty()) {
            env.insert(QStringLiteral("PROTON_VERSION"), config.protonVersion);
        }
        if (!config.winePrefix.isEmpty()) {
            env.insert(QStringLiteral("WINEPREFIX"), config.winePrefix);
        }
        const QString home = qEnvironmentVariable("HOME");
        env.insert(QStringLiteral("STEAM_COMPAT_CLIENT_INSTALL_PATH"), home + QStringLiteral("/.local/share/Steam"));
        env.insert(QStringLiteral("STEAM_COMPAT_DATA_PATH"), env.value(QStringLiteral("WINEPREFIX"), home + QStringLiteral("/.local/share/Steam/steamapps/compatdata")).toString());
        env.insert(QStringLiteral("PROTON_LOG"), QStringLiteral("0"));
        env.insert(QStringLiteral("PROTON_USE_WINED3D"), QStringLiteral("0"));
        env.insert(QStringLiteral("DXVK_HUD"), QStringLiteral("compiler"));
        env.insert(QStringLiteral("DXVK_STATE_CACHE"), QStringLiteral("1"));
    } else if (config.platform == LaunchPlatform::Wine) {
        if (!config.winePrefix.isEmpty()) {
            env.insert(QStringLiteral("WINEPREFIX"), config.winePrefix);
        }
        env.insert(QStringLiteral("WINEDEBUG"), QStringLiteral("-all"));
        env.insert(QStringLiteral("DXVK_HUD"), QStringLiteral("compiler"));
        env.insert(QStringLiteral("DXVK_STATE_CACHE"), QStringLiteral("1"));
    } else if (config.platform == LaunchPlatform::Steam) {
        env.insert(QStringLiteral("STEAM_RUNTIME"), QStringLiteral("1"));
    }

    env.insert(QStringLiteral("ENABLE_VKBASALT"), QStringLiteral("0"));
    env.insert(QStringLiteral("__GL_THREADED_OPTIMIZATIONS"), QStringLiteral("1"));
    env.insert(QStringLiteral("__GL_SHADER_DISK_CACHE"), QStringLiteral("1"));
    env.insert(QStringLiteral("__GL_SHADER_DISK_CACHE_SKIP_CLEANUP"), QStringLiteral("1"));
    env.insert(QStringLiteral("MESA_GLTHREAD"), QStringLiteral("true"));

    return env;
}

bool GamescopeLauncher::isGameRunning(const QString& titleId) const
{
    return m_runningProcesses.contains(titleId);
}

void GamescopeLauncher::exitToShell(const QString& titleId)
{
    QString targetId = titleId;
    if (targetId.isEmpty() && !m_runningProcesses.isEmpty()) {
        targetId = m_runningProcesses.firstKey();
    }

    if (!m_runningProcesses.contains(targetId)) {
        return;
    }

    auto* process = m_runningProcesses.value(targetId)->process.get();
    if (!process) {
        return;
    }

    emit exitToShellRequested(targetId);

    process->terminate();
    if (!process->waitForFinished(2000)) {
        process->kill();
    }
}

void GamescopeLauncher::toggleOverlay(const QString& titleId)
{
    emit overlayToggleRequested(titleId);
}

void GamescopeLauncher::onProcessStarted()
{
    auto* process = qobject_cast<QProcess*>(sender());
    if (!process) {
        return;
    }
    const QString titleId = process->property("titleId").toString();
    auto it = m_runningProcesses.find(titleId);
    if (it == m_runningProcesses.end()) {
        return;
    }

    qint64 pid = process->processId();
    const auto& config = it.value()->config;

    if (m_runningManager) {
        const bool suspendSupported = supportsSuspend(config.platform);
        const QString suspendReason = suspendUnsupportedReason(config.platform);
        m_runningManager->registerGame(titleId, config.displayName, pid, suspendSupported, suspendReason);
    }

    emit launchStarted(titleId, pid);
}

void GamescopeLauncher::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    auto* process = qobject_cast<QProcess*>(sender());
    if (!process) {
        return;
    }

    const QString titleId = process->property("titleId").toString();
    auto it = m_runningProcesses.find(titleId);
    if (it == m_runningProcesses.end()) {
        return;
    }

    if (m_runningManager) {
        m_runningManager->markGameExited(titleId);
    }

    QString status = (exitStatus == QProcess::NormalExit) ? QStringLiteral("normal") : QStringLiteral("crashed");
    emit gameExited(titleId, exitCode, status);

    m_runningProcesses.erase(it);
}

void GamescopeLauncher::onProcessError(QProcess::ProcessError error)
{
    auto* process = qobject_cast<QProcess*>(sender());
    if (!process) {
        return;
    }

    const QString titleId = process->property("titleId").toString();

    QString errorMessage;
    switch (error) {
    case QProcess::FailedToStart:
        errorMessage = QStringLiteral("Failed to start process");
        break;
    case QProcess::Crashed:
        errorMessage = QStringLiteral("Process crashed");
        break;
    case QProcess::Timedout:
        errorMessage = QStringLiteral("Process timed out");
        break;
    case QProcess::WriteError:
        errorMessage = QStringLiteral("Process write error");
        break;
    case QProcess::ReadError:
        errorMessage = QStringLiteral("Process read error");
        break;
    case QProcess::UnknownError:
    default:
        errorMessage = QStringLiteral("Unknown process error");
        break;
    }

    if (m_runningManager) {
        m_runningManager->markGameExited(titleId);
    }

    m_runningProcesses.remove(titleId);
    emit launchFailed(titleId, errorMessage);
}

QString GamescopeLauncher::platformToString(LaunchPlatform platform) const
{
    return Launch::platformToString(platform);
}

bool GamescopeLauncher::supportsSuspend(LaunchPlatform platform) const
{
    switch (platform) {
    case LaunchPlatform::Wine:
    case LaunchPlatform::Proton:
        return false;
    case LaunchPlatform::Epic:
        return false;
    case LaunchPlatform::Steam:
    case LaunchPlatform::Native:
    default:
        return true;
    }
}

QString GamescopeLauncher::suspendUnsupportedReason(LaunchPlatform platform) const
{
    switch (platform) {
    case LaunchPlatform::Wine:
        return QStringLiteral("Wine compatibility does not support suspend/resume");
    case LaunchPlatform::Proton:
        return QStringLiteral("Proton compatibility does not support suspend/resume");
    case LaunchPlatform::Epic:
        return QStringLiteral("Epic launcher titles do not support suspend/resume");
    case LaunchPlatform::Steam:
    case LaunchPlatform::Native:
    default:
        return QString();
    }
}

LaunchPlatform parsePlatform(const QString& platformStr)
{
    const QString normalized = normalize(platformStr);
    if (normalized == QStringLiteral("steam")) {
        return LaunchPlatform::Steam;
    }
    if (normalized == QStringLiteral("epic")) {
        return LaunchPlatform::Epic;
    }
    if (normalized == QStringLiteral("wine")) {
        return LaunchPlatform::Wine;
    }
    if (normalized == QStringLiteral("proton")) {
        return LaunchPlatform::Proton;
    }
    return LaunchPlatform::Native;
}

QString platformToString(LaunchPlatform platform)
{
    switch (platform) {
    case LaunchPlatform::Steam:
        return QStringLiteral("steam");
    case LaunchPlatform::Epic:
        return QStringLiteral("epic");
    case LaunchPlatform::Wine:
        return QStringLiteral("wine");
    case LaunchPlatform::Proton:
        return QStringLiteral("proton");
    case LaunchPlatform::Native:
    default:
        return QStringLiteral("native");
    }
}

} // namespace Launch
