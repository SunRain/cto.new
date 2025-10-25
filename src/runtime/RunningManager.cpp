#include "RunningManager.hpp"

#include <QDateTime>
#include <QVariantMap>

namespace Runtime {

namespace {
constexpr double CPU_ALERT_THRESHOLD = 95.0;
constexpr double GPU_ALERT_THRESHOLD = 95.0;
constexpr double RAM_ALERT_THRESHOLD_PERCENT = 90.0;
constexpr double TEMP_ALERT_THRESHOLD = 85.0;
constexpr double GPU_TEMP_ALERT_THRESHOLD = 85.0;
constexpr double POWER_ALERT_THRESHOLD = 120.0;
constexpr double FPS_FLOOR_ALERT = 15.0;

QString severityToString(RunningManager::AlertSeverity severity)
{
    return severity == RunningManager::AlertSeverity::Critical ? QStringLiteral("critical")
                                                               : QStringLiteral("warning");
}

} // namespace

RunningManager::RunningManager(QObject* parent)
    : RunningManager(createSystemMetricsProvider(), parent)
{
}

RunningManager::RunningManager(std::shared_ptr<ProcessMetricsProvider> provider, QObject* parent)
    : QObject(parent)
    , m_metricsProvider(provider ? provider : createSystemMetricsProvider())
{
    m_updateTimer.setInterval(m_updateIntervalMs);
    connect(&m_updateTimer, &QTimer::timeout, this, &RunningManager::updateMetrics);
    if (m_metricsProvider) {
        m_updateTimer.start();
    }
}

RunningManager::~RunningManager()
{
    m_updateTimer.stop();
}

QVariantList RunningManager::games() const
{
    QVariantList list;
    list.reserve(m_games.size());
    for (const auto& game : m_games) {
        list.append(serializeGame(game));
    }
    return list;
}

QVariantList RunningManager::alerts() const
{
    QVariantList list;
    for (const auto& game : m_games) {
        for (const auto& alert : game.activeAlerts) {
            list.append(serializeAlert(alert));
        }
    }
    return list;
}

int RunningManager::updateIntervalMs() const
{
    return m_updateIntervalMs;
}

void RunningManager::setUpdateIntervalMs(int interval)
{
    if (interval <= 0) {
        interval = 1000;
    }
    if (m_updateIntervalMs == interval) {
        return;
    }

    m_updateIntervalMs = interval;
    m_updateTimer.setInterval(m_updateIntervalMs);
    emit updateIntervalMsChanged();
}

void RunningManager::registerGame(const QString& titleId,
                                  const QString& displayName,
                                  qint64 pid,
                                  bool supportsSuspend,
                                  const QString& suspendUnsupportedReason)
{
    int index = indexForId(titleId);
    if (index >= 0) {
        auto& game = m_games[index];
        game.displayName = displayName;
        game.pid = pid;
        game.supportsSuspend = supportsSuspend;
        game.suspendUnsupportedReason = suspendUnsupportedReason;
        game.state = GameState::Running;
    } else {
        RunningGame game;
        game.titleId = titleId;
        game.displayName = displayName;
        game.pid = pid;
        game.supportsSuspend = supportsSuspend;
        game.suspendUnsupportedReason = suspendUnsupportedReason;
        m_gameIndex.insert(titleId, m_games.size());
        m_games.push_back(game);
    }

    if (!m_updateTimer.isActive() && m_metricsProvider) {
        m_updateTimer.start();
    }

    emit gamesChanged();
}

void RunningManager::markGameExited(const QString& titleId)
{
    int index = indexForId(titleId);
    if (index < 0) {
        return;
    }

    removeGameAt(index);
    emit gameClosed(titleId);
    emit gamesChanged();
}

void RunningManager::refreshNow()
{
    updateMetrics();
}

QVariantMap RunningManager::metricsFor(const QString& titleId) const
{
    int index = indexForId(titleId);
    if (index < 0) {
        return {};
    }

    return serializeGame(m_games[index]).value(QStringLiteral("metrics")).toMap();
}

void RunningManager::focusGame(const QString& titleId)
{
    int index = indexForId(titleId);
    if (index < 0) {
        return;
    }

    const auto& game = m_games[index];
    emit focusRequested(game.titleId, game.pid);
}

void RunningManager::suspendGame(const QString& titleId)
{
    int index = indexForId(titleId);
    if (index < 0) {
        return;
    }

    auto& game = m_games[index];
    if (!game.supportsSuspend) {
        QVariantMap alert;
        alert["type"] = QStringLiteral("suspendUnsupported");
        alert["titleId"] = game.titleId;
        alert["message"] = game.suspendUnsupportedReason.isEmpty()
            ? tr("Suspend is not supported for this title.")
            : game.suspendUnsupportedReason;
        alert["severity"] = QStringLiteral("warning");
        emit alertRaised(game.titleId, alert);
        emit alertsChanged();
        return;
    }

    if (game.state == GameState::Suspended) {
        return;
    }

    game.state = GameState::Suspended;
    emit suspendRequested(game.titleId, game.pid);
    emit gameSuspended(game.titleId);
    emit gamesChanged();
}

void RunningManager::resumeGame(const QString& titleId)
{
    int index = indexForId(titleId);
    if (index < 0) {
        return;
    }

    auto& game = m_games[index];
    if (game.state != GameState::Suspended) {
        return;
    }

    game.state = GameState::Running;
    emit resumeRequested(game.titleId, game.pid);
    emit gameResumed(game.titleId);
    emit gamesChanged();
}

void RunningManager::forceQuit(const QString& titleId)
{
    int index = indexForId(titleId);
    if (index < 0) {
        return;
    }

    const auto& game = m_games.at(index);
    emit forceQuitRequested(game.titleId, game.pid);
    removeGameAt(index);
    emit gameClosed(game.titleId);
    emit gamesChanged();
}

void RunningManager::setMetricsProvider(std::shared_ptr<ProcessMetricsProvider> provider)
{
    m_metricsProvider = provider;
    if (m_metricsProvider && !m_updateTimer.isActive()) {
        m_updateTimer.start();
    } else if (!m_metricsProvider) {
        m_updateTimer.stop();
    }
}

void RunningManager::updateMetrics()
{
    if (!m_metricsProvider) {
        return;
    }

    QVector<QString> toRemove;
    bool anyGameUpdated = false;

    for (auto& game : m_games) {
        if (game.state == GameState::Suspended) {
            continue;
        }

        ProcessMetrics metrics = m_metricsProvider->metricsForPid(game.pid);
        if (!metrics.valid) {
            toRemove.append(game.titleId);
            continue;
        }

        game.metrics = metrics;
        evaluateAlerts(game);
        anyGameUpdated = true;
    }

    for (const auto& id : toRemove) {
        markGameExited(id);
    }

    if (anyGameUpdated || !toRemove.isEmpty()) {
        emit gamesChanged();
    }
}

QVariantMap RunningManager::serializeGame(const RunningGame& game) const
{
    QVariantMap map;
    map["titleId"] = game.titleId;
    map["displayName"] = game.displayName;
    map["pid"] = game.pid;
    map["supportsSuspend"] = game.supportsSuspend;
    map["suspendUnsupportedReason"] = game.supportsSuspend
        ? QString()
        : (game.suspendUnsupportedReason.isEmpty()
            ? tr("Suspend is not supported for this title.")
            : game.suspendUnsupportedReason);
    map["state"] = game.state == GameState::Running ? QStringLiteral("running") : QStringLiteral("suspended");

    QVariantMap metricsMap;
    metricsMap["valid"] = game.metrics.valid;
    metricsMap["cpuPercent"] = game.metrics.cpuPercent;
    metricsMap["gpuPercent"] = game.metrics.gpuPercent;
    metricsMap["ramMb"] = game.metrics.ramMb;
    metricsMap["ramPercent"] = game.metrics.ramPercent;
    metricsMap["temperatureC"] = game.metrics.temperatureC;
    metricsMap["gpuTemperatureC"] = game.metrics.gpuTemperatureC;
    metricsMap["powerWatts"] = game.metrics.powerWatts;
    metricsMap["fps"] = game.metrics.fps;
    metricsMap["updatedAt"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    map["metrics"] = metricsMap;

    QVariantList alertList;
    for (const auto& alert : game.activeAlerts) {
        alertList.append(serializeAlert(alert));
    }
    map["alerts"] = alertList;

    return map;
}

QVariantMap RunningManager::serializeAlert(const Alert& alert) const
{
    QVariantMap map;
    map["type"] = alert.type;
    map["titleId"] = alert.titleId;
    map["message"] = alert.message;
    map["severity"] = severityToString(alert.severity);
    map["timestamp"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    return map;
}

void RunningManager::evaluateAlerts(RunningGame& game)
{
    auto triggerAlert = [&](const QString& key, const QString& message, AlertSeverity severity) {
        auto it = game.activeAlerts.find(key);
        bool updated = false;
        if (it == game.activeAlerts.end()) {
            Alert alert;
            alert.type = key;
            alert.titleId = game.titleId;
            alert.message = message;
            alert.severity = severity;
            game.activeAlerts.insert(key, alert);
            emit alertRaised(game.titleId, serializeAlert(alert));
            updated = true;
        } else {
            Alert& existing = it.value();
            if (existing.message != message || existing.severity != severity) {
                existing.message = message;
                existing.severity = severity;
                emit alertRaised(game.titleId, serializeAlert(existing));
                updated = true;
            }
        }
        if (updated) {
            emit alertsChanged();
        }
    };

    auto clearAlert = [&](const QString& key) {
        if (game.activeAlerts.remove(key) > 0) {
            emit alertCleared(game.titleId, key);
            emit alertsChanged();
        }
    };

    if (game.metrics.temperatureC >= TEMP_ALERT_THRESHOLD) {
        const QString message = tr("%1 is overheating (%2°C)")
            .arg(game.displayName)
            .arg(game.metrics.temperatureC, 0, 'f', 1);
        triggerAlert(QStringLiteral("temperature"), message,
                     game.metrics.temperatureC >= TEMP_ALERT_THRESHOLD + 5.0 ? AlertSeverity::Critical
                                                                           : AlertSeverity::Warning);
    } else {
        clearAlert(QStringLiteral("temperature"));
    }

    if (game.metrics.gpuTemperatureC >= GPU_TEMP_ALERT_THRESHOLD) {
        const QString message = tr("GPU temperature high for %1 (%2°C)")
            .arg(game.displayName)
            .arg(game.metrics.gpuTemperatureC, 0, 'f', 1);
        triggerAlert(QStringLiteral("gpuTemperature"), message,
                     game.metrics.gpuTemperatureC >= GPU_TEMP_ALERT_THRESHOLD + 5.0 ? AlertSeverity::Critical
                                                                                  : AlertSeverity::Warning);
    } else {
        clearAlert(QStringLiteral("gpuTemperature"));
    }

    if (game.metrics.cpuPercent >= CPU_ALERT_THRESHOLD) {
        const QString message = tr("CPU usage high for %1 (%2%)")
            .arg(game.displayName)
            .arg(game.metrics.cpuPercent, 0, 'f', 1);
        triggerAlert(QStringLiteral("cpu"), message, AlertSeverity::Warning);
    } else {
        clearAlert(QStringLiteral("cpu"));
    }

    if (game.metrics.gpuPercent >= GPU_ALERT_THRESHOLD) {
        const QString message = tr("GPU usage high for %1 (%2%)")
            .arg(game.displayName)
            .arg(game.metrics.gpuPercent, 0, 'f', 1);
        triggerAlert(QStringLiteral("gpu"), message, AlertSeverity::Warning);
    } else {
        clearAlert(QStringLiteral("gpu"));
    }

    if (game.metrics.ramPercent >= RAM_ALERT_THRESHOLD_PERCENT) {
        const QString message = tr("Memory usage high for %1 (%2%)")
            .arg(game.displayName)
            .arg(game.metrics.ramPercent, 0, 'f', 1);
        triggerAlert(QStringLiteral("memory"), message, AlertSeverity::Warning);
    } else {
        clearAlert(QStringLiteral("memory"));
    }

    if (game.metrics.powerWatts >= POWER_ALERT_THRESHOLD) {
        const QString message = tr("Power draw unusually high for %1 (%2 W)")
            .arg(game.displayName)
            .arg(game.metrics.powerWatts, 0, 'f', 1);
        triggerAlert(QStringLiteral("power"), message, AlertSeverity::Warning);
    } else {
        clearAlert(QStringLiteral("power"));
    }

    if (game.metrics.fps > 0.0 && game.metrics.fps < FPS_FLOOR_ALERT) {
        const QString message = tr("FPS dropping on %1 (%2 FPS)")
            .arg(game.displayName)
            .arg(game.metrics.fps, 0, 'f', 0);
        triggerAlert(QStringLiteral("fps"), message, AlertSeverity::Warning);
    } else {
        clearAlert(QStringLiteral("fps"));
    }
}

void RunningManager::removeGameAt(int index)
{
    if (index < 0 || index >= m_games.size()) {
        return;
    }

    const QString id = m_games[index].titleId;
    m_games.removeAt(index);
    m_gameIndex.remove(id);

    for (int i = index; i < m_games.size(); ++i) {
        m_gameIndex[m_games[i].titleId] = i;
    }
}

int RunningManager::indexForId(const QString& titleId) const
{
    auto it = m_gameIndex.constFind(titleId);
    if (it == m_gameIndex.constEnd()) {
        return -1;
    }
    return it.value();
}

} // namespace Runtime
