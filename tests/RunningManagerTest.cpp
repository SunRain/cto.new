#include "runtime/RunningManager.hpp"
#include "runtime/ProcessMetricsProvider.hpp"

#include <QSignalSpy>
#include <QTest>
#include <QVariantList>
#include <QVariantMap>
#include <memory>

class MockMetricsProvider : public Runtime::ProcessMetricsProvider {
public:
    MockMetricsProvider() = default;
    ~MockMetricsProvider() override = default;

    Runtime::ProcessMetrics metricsForPid(qint64 pid) override
    {
        auto it = m_metrics.constFind(pid);
        if (it == m_metrics.constEnd()) {
            return {};
        }
        return it.value();
    }

    void setMetrics(qint64 pid, const Runtime::ProcessMetrics& metrics)
    {
        m_metrics[pid] = metrics;
    }

    void clearMetrics(qint64 pid)
    {
        m_metrics.remove(pid);
    }

private:
    QHash<qint64, Runtime::ProcessMetrics> m_metrics;
};

class RunningManagerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testRegisterGame();
    void testMarkGameExited();
    void testMetricsUpdate();
    void testAlerts_HighTemperature();
    void testAlerts_HighCpu();
    void testAlerts_HighMemory();
    void testAlerts_LowFps();
    void testSuspendResume_Supported();
    void testSuspendResume_Unsupported();
    void testForceQuit();
    void testMultipleGames();
    void testAlertThresholds();
    void testMetricsInvalidProcess();

private:
    std::shared_ptr<MockMetricsProvider> m_mockProvider;
    std::unique_ptr<Runtime::RunningManager> m_manager;
};

void RunningManagerTest::initTestCase()
{
}

void RunningManagerTest::cleanupTestCase()
{
}

void RunningManagerTest::init()
{
    m_mockProvider = std::make_shared<MockMetricsProvider>();
    m_manager = std::make_unique<Runtime::RunningManager>(m_mockProvider);
}

void RunningManagerTest::cleanup()
{
    m_manager.reset();
    m_mockProvider.reset();
}

void RunningManagerTest::testRegisterGame()
{
    QSignalSpy spy(m_manager.get(), &Runtime::RunningManager::gamesChanged);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");

    QCOMPARE(spy.count(), 1);
    QCOMPARE(m_manager->games().size(), 1);

    QVariantMap game = m_manager->games().at(0).toMap();
    QCOMPARE(game.value("titleId").toString(), QString("game1"));
    QCOMPARE(game.value("displayName").toString(), QString("Test Game 1"));
    QCOMPARE(game.value("pid").toLongLong(), 12345LL);
    QCOMPARE(game.value("supportsSuspend").toBool(), true);
    QCOMPARE(game.value("state").toString(), QString("running"));
}

void RunningManagerTest::testMarkGameExited()
{
    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");

    QSignalSpy closedSpy(m_manager.get(), &Runtime::RunningManager::gameClosed);
    QSignalSpy changedSpy(m_manager.get(), &Runtime::RunningManager::gamesChanged);

    m_manager->markGameExited("game1");

    QCOMPARE(closedSpy.count(), 1);
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(m_manager->games().size(), 0);
}

void RunningManagerTest::testMetricsUpdate()
{
    Runtime::ProcessMetrics metrics;
    metrics.pid = 12345;
    metrics.cpuPercent = 45.5;
    metrics.gpuPercent = 60.0;
    metrics.ramMb = 2048.0;
    metrics.temperatureC = 70.0;
    metrics.powerWatts = 50.0;
    metrics.fps = 60.0;
    metrics.valid = true;

    m_mockProvider->setMetrics(12345, metrics);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");
    m_manager->refreshNow();

    QVariantMap game = m_manager->games().at(0).toMap();
    QVariantMap gameMetrics = game.value("metrics").toMap();

    QCOMPARE(gameMetrics.value("cpuPercent").toDouble(), 45.5);
    QCOMPARE(gameMetrics.value("gpuPercent").toDouble(), 60.0);
    QCOMPARE(gameMetrics.value("ramMb").toDouble(), 2048.0);
    QCOMPARE(gameMetrics.value("temperatureC").toDouble(), 70.0);
    QCOMPARE(gameMetrics.value("fps").toDouble(), 60.0);
}

void RunningManagerTest::testAlerts_HighTemperature()
{
    Runtime::ProcessMetrics metrics;
    metrics.pid = 12345;
    metrics.cpuPercent = 50.0;
    metrics.gpuPercent = 50.0;
    metrics.ramMb = 1024.0;
    metrics.temperatureC = 90.0;
    metrics.powerWatts = 50.0;
    metrics.fps = 60.0;
    metrics.valid = true;

    m_mockProvider->setMetrics(12345, metrics);

    QSignalSpy alertSpy(m_manager.get(), &Runtime::RunningManager::alertRaised);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");
    m_manager->refreshNow();

    QVERIFY(alertSpy.count() > 0);

    bool foundTempAlert = false;
    for (int i = 0; i < alertSpy.count(); ++i) {
        QVariantMap alert = alertSpy.at(i).at(1).toMap();
        if (alert.value("type").toString() == "temperature") {
            foundTempAlert = true;
            QCOMPARE(alert.value("severity").toString(), QString("critical"));
            QVERIFY(alert.value("message").toString().contains("overheating"));
            break;
        }
    }
    QVERIFY(foundTempAlert);
}

void RunningManagerTest::testAlerts_HighCpu()
{
    Runtime::ProcessMetrics metrics;
    metrics.pid = 12345;
    metrics.cpuPercent = 96.0;
    metrics.gpuPercent = 50.0;
    metrics.ramMb = 1024.0;
    metrics.temperatureC = 70.0;
    metrics.powerWatts = 50.0;
    metrics.fps = 60.0;
    metrics.valid = true;

    m_mockProvider->setMetrics(12345, metrics);

    QSignalSpy alertSpy(m_manager.get(), &Runtime::RunningManager::alertRaised);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");
    m_manager->refreshNow();

    QVERIFY(alertSpy.count() > 0);

    bool foundCpuAlert = false;
    for (int i = 0; i < alertSpy.count(); ++i) {
        QVariantMap alert = alertSpy.at(i).at(1).toMap();
        if (alert.value("type").toString() == "cpu") {
            foundCpuAlert = true;
            QCOMPARE(alert.value("severity").toString(), QString("warning"));
            break;
        }
    }
    QVERIFY(foundCpuAlert);
}

void RunningManagerTest::testAlerts_HighMemory()
{
    Runtime::ProcessMetrics metrics;
    metrics.pid = 12345;
    metrics.cpuPercent = 50.0;
    metrics.gpuPercent = 50.0;
    metrics.ramMb = 8192.0;
    metrics.ramPercent = 92.0;
    metrics.temperatureC = 70.0;
    metrics.powerWatts = 50.0;
    metrics.fps = 60.0;
    metrics.valid = true;

    m_mockProvider->setMetrics(12345, metrics);

    QSignalSpy alertSpy(m_manager.get(), &Runtime::RunningManager::alertRaised);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");
    m_manager->refreshNow();

    QVERIFY(alertSpy.count() > 0);

    bool foundMemAlert = false;
    for (int i = 0; i < alertSpy.count(); ++i) {
        QVariantMap alert = alertSpy.at(i).at(1).toMap();
        if (alert.value("type").toString() == "memory") {
            foundMemAlert = true;
            QCOMPARE(alert.value("severity").toString(), QString("warning"));
            break;
        }
    }
    QVERIFY(foundMemAlert);
}

void RunningManagerTest::testAlerts_LowFps()
{
    Runtime::ProcessMetrics metrics;
    metrics.pid = 12345;
    metrics.cpuPercent = 50.0;
    metrics.gpuPercent = 50.0;
    metrics.ramMb = 1024.0;
    metrics.temperatureC = 70.0;
    metrics.powerWatts = 50.0;
    metrics.fps = 10.0;
    metrics.valid = true;

    m_mockProvider->setMetrics(12345, metrics);

    QSignalSpy alertSpy(m_manager.get(), &Runtime::RunningManager::alertRaised);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");
    m_manager->refreshNow();

    QVERIFY(alertSpy.count() > 0);

    bool foundFpsAlert = false;
    for (int i = 0; i < alertSpy.count(); ++i) {
        QVariantMap alert = alertSpy.at(i).at(1).toMap();
        if (alert.value("type").toString() == "fps") {
            foundFpsAlert = true;
            QCOMPARE(alert.value("severity").toString(), QString("warning"));
            break;
        }
    }
    QVERIFY(foundFpsAlert);
}

void RunningManagerTest::testSuspendResume_Supported()
{
    Runtime::ProcessMetrics metrics;
    metrics.pid = 12345;
    metrics.valid = true;
    m_mockProvider->setMetrics(12345, metrics);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");

    QSignalSpy suspendSpy(m_manager.get(), &Runtime::RunningManager::gameSuspended);
    m_manager->suspendGame("game1");

    QCOMPARE(suspendSpy.count(), 1);
    QVariantMap game = m_manager->games().at(0).toMap();
    QCOMPARE(game.value("state").toString(), QString("suspended"));

    QSignalSpy resumeSpy(m_manager.get(), &Runtime::RunningManager::gameResumed);
    m_manager->resumeGame("game1");

    QCOMPARE(resumeSpy.count(), 1);
    game = m_manager->games().at(0).toMap();
    QCOMPARE(game.value("state").toString(), QString("running"));
}

void RunningManagerTest::testSuspendResume_Unsupported()
{
    Runtime::ProcessMetrics metrics;
    metrics.pid = 12345;
    metrics.valid = true;
    m_mockProvider->setMetrics(12345, metrics);

    m_manager->registerGame("game1", "Test Game 1", 12345, false, "Wine processes cannot be suspended");

    QSignalSpy alertSpy(m_manager.get(), &Runtime::RunningManager::alertRaised);
    m_manager->suspendGame("game1");

    QCOMPARE(alertSpy.count(), 1);
    QVariantMap alert = alertSpy.at(0).at(1).toMap();
    QCOMPARE(alert.value("type").toString(), QString("suspendUnsupported"));
    QVERIFY(alert.value("message").toString().contains("Wine"));
}

void RunningManagerTest::testForceQuit()
{
    Runtime::ProcessMetrics metrics;
    metrics.pid = 12345;
    metrics.valid = true;
    m_mockProvider->setMetrics(12345, metrics);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");

    QSignalSpy quitSpy(m_manager.get(), &Runtime::RunningManager::forceQuitRequested);
    QSignalSpy closedSpy(m_manager.get(), &Runtime::RunningManager::gameClosed);

    m_manager->forceQuit("game1");

    QCOMPARE(quitSpy.count(), 1);
    QCOMPARE(closedSpy.count(), 1);
    QCOMPARE(m_manager->games().size(), 0);
}

void RunningManagerTest::testMultipleGames()
{
    Runtime::ProcessMetrics metrics1;
    metrics1.pid = 12345;
    metrics1.valid = true;
    m_mockProvider->setMetrics(12345, metrics1);

    Runtime::ProcessMetrics metrics2;
    metrics2.pid = 67890;
    metrics2.valid = true;
    m_mockProvider->setMetrics(67890, metrics2);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");
    m_manager->registerGame("game2", "Test Game 2", 67890, true, "");

    QCOMPARE(m_manager->games().size(), 2);

    m_manager->markGameExited("game1");
    QCOMPARE(m_manager->games().size(), 1);

    QVariantMap game = m_manager->games().at(0).toMap();
    QCOMPARE(game.value("titleId").toString(), QString("game2"));
}

void RunningManagerTest::testAlertThresholds()
{
    Runtime::ProcessMetrics normalMetrics;
    normalMetrics.pid = 12345;
    normalMetrics.cpuPercent = 50.0;
    normalMetrics.gpuPercent = 50.0;
    normalMetrics.ramMb = 1024.0;
    normalMetrics.ramPercent = 50.0;
    normalMetrics.temperatureC = 70.0;
    normalMetrics.powerWatts = 50.0;
    normalMetrics.fps = 60.0;
    normalMetrics.valid = true;

    m_mockProvider->setMetrics(12345, normalMetrics);

    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");
    m_manager->refreshNow();

    QCOMPARE(m_manager->alerts().size(), 0);

    normalMetrics.temperatureC = 86.0;
    m_mockProvider->setMetrics(12345, normalMetrics);

    QSignalSpy alertSpy(m_manager.get(), &Runtime::RunningManager::alertRaised);
    m_manager->refreshNow();

    QVERIFY(alertSpy.count() > 0);
    QVERIFY(m_manager->alerts().size() > 0);
}

void RunningManagerTest::testMetricsInvalidProcess()
{
    m_manager->registerGame("game1", "Test Game 1", 12345, true, "");

    QSignalSpy closedSpy(m_manager.get(), &Runtime::RunningManager::gameClosed);
    m_manager->refreshNow();

    QCOMPARE(closedSpy.count(), 1);
    QCOMPARE(m_manager->games().size(), 0);
}

QTEST_MAIN(RunningManagerTest)
#include "RunningManagerTest.moc"
