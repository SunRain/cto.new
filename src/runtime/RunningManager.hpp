#pragma once

#include "ProcessMetricsProvider.hpp"

#include <QHash>
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>
#include <memory>

namespace Runtime {

class RunningManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList games READ games NOTIFY gamesChanged)
    Q_PROPERTY(QVariantList alerts READ alerts NOTIFY alertsChanged)
    Q_PROPERTY(int updateIntervalMs READ updateIntervalMs WRITE setUpdateIntervalMs NOTIFY updateIntervalMsChanged)

public:
    explicit RunningManager(QObject* parent = nullptr);
    explicit RunningManager(std::shared_ptr<ProcessMetricsProvider> provider, QObject* parent = nullptr);
    ~RunningManager() override;

    QVariantList games() const;
    QVariantList alerts() const;

    int updateIntervalMs() const;
    void setUpdateIntervalMs(int interval);

    Q_INVOKABLE void registerGame(const QString& titleId,
                                  const QString& displayName,
                                  qint64 pid,
                                  bool supportsSuspend,
                                  const QString& suspendUnsupportedReason = {});
    Q_INVOKABLE void markGameExited(const QString& titleId);
    Q_INVOKABLE void refreshNow();
    Q_INVOKABLE QVariantMap metricsFor(const QString& titleId) const;
    Q_INVOKABLE void focusGame(const QString& titleId);
    Q_INVOKABLE void suspendGame(const QString& titleId);
    Q_INVOKABLE void resumeGame(const QString& titleId);
    Q_INVOKABLE void forceQuit(const QString& titleId);

    void setMetricsProvider(std::shared_ptr<ProcessMetricsProvider> provider);

signals:
    void gamesChanged();
    void alertsChanged();
    void updateIntervalMsChanged();

    void focusRequested(const QString& titleId, qint64 pid);
    void suspendRequested(const QString& titleId, qint64 pid);
    void resumeRequested(const QString& titleId, qint64 pid);
    void forceQuitRequested(const QString& titleId, qint64 pid);

    void alertRaised(const QString& titleId, const QVariantMap& alert);
    void alertCleared(const QString& titleId, const QString& type);

    void gameSuspended(const QString& titleId);
    void gameResumed(const QString& titleId);
    void gameClosed(const QString& titleId);

private slots:
    void updateMetrics();

private:
    enum class GameState {
        Running,
        Suspended
    };

    enum class AlertSeverity {
        Warning,
        Critical
    };

    struct Alert {
        QString type;
        QString titleId;
        QString message;
        AlertSeverity severity = AlertSeverity::Warning;
    };

    struct RunningGame {
        QString titleId;
        QString displayName;
        qint64 pid = 0;
        bool supportsSuspend = false;
        QString suspendUnsupportedReason;
        GameState state = GameState::Running;
        ProcessMetrics metrics;
        QHash<QString, Alert> activeAlerts;
    };

    QVariantMap serializeGame(const RunningGame& game) const;
    QVariantMap serializeAlert(const Alert& alert) const;
    void evaluateAlerts(RunningGame& game);
    void removeGameAt(int index);
    int indexForId(const QString& titleId) const;

    std::shared_ptr<ProcessMetricsProvider> m_metricsProvider;
    QTimer m_updateTimer;
    QVector<RunningGame> m_games;
    QHash<QString, int> m_gameIndex;
    int m_updateIntervalMs = 1000;
};

} // namespace Runtime
