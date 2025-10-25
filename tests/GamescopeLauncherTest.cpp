#include "launch/GamescopeLauncher.hpp"
#include "launch/PerformanceProfile.hpp"
#include "runtime/RunningManager.hpp"
#include "runtime/ProcessMetricsProvider.hpp"

#include <QSignalSpy>
#include <QTest>
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

private:
    QHash<qint64, Runtime::ProcessMetrics> m_metrics;
};

class GamescopeLauncherTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testDefaultPerformanceProfile();
    void testPerformanceProfileFromVariantMap();
    void testPerformanceProfileToVariantMap();
    void testScalingModeConversion();
    void testWindowModeConversion();
    void testBuildGamescopeArguments_Default();
    void testBuildGamescopeArguments_FpsCap();
    void testBuildGamescopeArguments_FSR();
    void testBuildGamescopeArguments_Resolution();
    void testBuildGamescopeArguments_WindowModes();
    void testBuildGamescopeArguments_VSync();
    void testBuildEnvironment_Native();
    void testBuildEnvironment_Proton();
    void testBuildEnvironment_Wine();
    void testBuildEnvironment_Steam();
    void testBuildGamescopeCommand_NativeGame();
    void testBuildGamescopeCommand_ProtonGame();
    void testBuildGamescopeCommand_WineGame();
    void testBuildGamescopeCommand_WithoutGamescope();
    void testPlatformConversion();
    void testLaunchValidation();

private:
    std::shared_ptr<MockMetricsProvider> m_mockProvider;
    std::unique_ptr<Runtime::RunningManager> m_manager;
    std::unique_ptr<Launch::GamescopeLauncher> m_launcher;
};

void GamescopeLauncherTest::initTestCase()
{
}

void GamescopeLauncherTest::cleanupTestCase()
{
}

void GamescopeLauncherTest::init()
{
    m_mockProvider = std::make_shared<MockMetricsProvider>();
    m_manager = std::make_unique<Runtime::RunningManager>(m_mockProvider);
    m_launcher = std::make_unique<Launch::GamescopeLauncher>(m_manager.get());
}

void GamescopeLauncherTest::cleanup()
{
    m_launcher.reset();
    m_manager.reset();
    m_mockProvider.reset();
}

void GamescopeLauncherTest::testDefaultPerformanceProfile()
{
    Launch::PerformanceProfile profile = Launch::PerformanceProfile::defaultProfile();

    QCOMPARE(profile.fpsCap, 0);
    QCOMPARE(profile.scaling, Launch::ScalingMode::Native);
    QVERIFY(profile.vsync);
    QCOMPARE(profile.windowMode, Launch::WindowMode::Fullscreen);
    QVERIFY(profile.isValid());
}

void GamescopeLauncherTest::testPerformanceProfileFromVariantMap()
{
    QVariantMap map;
    map["fpsCap"] = 60;
    map["scaling"] = "fsr";
    map["vsync"] = false;
    map["windowMode"] = "borderless";
    map["renderWidth"] = 1920;
    map["renderHeight"] = 1080;
    map["fsrSharpness"] = 10;

    Launch::PerformanceProfile profile = Launch::PerformanceProfile::fromVariantMap(map);

    QCOMPARE(profile.fpsCap, 60);
    QCOMPARE(profile.scaling, Launch::ScalingMode::FSR);
    QVERIFY(!profile.vsync);
    QCOMPARE(profile.windowMode, Launch::WindowMode::Borderless);
    QCOMPARE(profile.renderWidth, 1920);
    QCOMPARE(profile.renderHeight, 1080);
    QCOMPARE(profile.fsrSharpness, 10);
}

void GamescopeLauncherTest::testPerformanceProfileToVariantMap()
{
    Launch::PerformanceProfile profile;
    profile.fpsCap = 120;
    profile.scaling = Launch::ScalingMode::FSR;
    profile.vsync = false;
    profile.windowMode = Launch::WindowMode::Windowed;

    QVariantMap map = profile.toVariantMap();

    QCOMPARE(map.value("fpsCap").toInt(), 120);
    QCOMPARE(map.value("scaling").toString(), QString("fsr"));
    QVERIFY(!map.value("vsync").toBool());
    QCOMPARE(map.value("windowMode").toString(), QString("windowed"));
}

void GamescopeLauncherTest::testScalingModeConversion()
{
    QCOMPARE(Launch::scalingModeToString(Launch::ScalingMode::Native), QString("native"));
    QCOMPARE(Launch::scalingModeToString(Launch::ScalingMode::FSR), QString("fsr"));
    QCOMPARE(Launch::scalingModeToString(Launch::ScalingMode::Integer), QString("integer"));

    QCOMPARE(Launch::scalingModeFromString("native"), Launch::ScalingMode::Native);
    QCOMPARE(Launch::scalingModeFromString("FSR"), Launch::ScalingMode::FSR);
    QCOMPARE(Launch::scalingModeFromString("INTEGER"), Launch::ScalingMode::Integer);
}

void GamescopeLauncherTest::testWindowModeConversion()
{
    QCOMPARE(Launch::windowModeToString(Launch::WindowMode::Fullscreen), QString("fullscreen"));
    QCOMPARE(Launch::windowModeToString(Launch::WindowMode::Borderless), QString("borderless"));
    QCOMPARE(Launch::windowModeToString(Launch::WindowMode::Windowed), QString("windowed"));

    QCOMPARE(Launch::windowModeFromString("fullscreen"), Launch::WindowMode::Fullscreen);
    QCOMPARE(Launch::windowModeFromString("BORDERLESS"), Launch::WindowMode::Borderless);
    QCOMPARE(Launch::windowModeFromString("Windowed"), Launch::WindowMode::Windowed);
}

void GamescopeLauncherTest::testBuildGamescopeArguments_Default()
{
    Launch::PerformanceProfile profile = Launch::PerformanceProfile::defaultProfile();
    QStringList args = m_launcher->buildGamescopeArguments(profile);

    QVERIFY(args.contains("--fullscreen"));
    QVERIFY(args.contains("--vsync"));
    QVERIFY(args.contains("--mangoapp"));
    QVERIFY(args.contains("--"));
}

void GamescopeLauncherTest::testBuildGamescopeArguments_FpsCap()
{
    Launch::PerformanceProfile profile = Launch::PerformanceProfile::defaultProfile();
    profile.fpsCap = 60;

    QStringList args = m_launcher->buildGamescopeArguments(profile);

    QVERIFY(args.contains("--fps"));
    QVERIFY(args.contains("60"));
    QVERIFY(args.contains("--game-fps"));
}

void GamescopeLauncherTest::testBuildGamescopeArguments_FSR()
{
    Launch::PerformanceProfile profile = Launch::PerformanceProfile::defaultProfile();
    profile.scaling = Launch::ScalingMode::FSR;
    profile.fsrSharpness = 10;

    QStringList args = m_launcher->buildGamescopeArguments(profile);

    QVERIFY(args.contains("--fsr"));
    QVERIFY(args.contains("--fsr-sharpness"));
    QVERIFY(args.contains("10"));
}

void GamescopeLauncherTest::testBuildGamescopeArguments_Resolution()
{
    Launch::PerformanceProfile profile = Launch::PerformanceProfile::defaultProfile();
    profile.renderWidth = 1920;
    profile.renderHeight = 1080;
    profile.outputWidth = 2560;
    profile.outputHeight = 1440;

    QStringList args = m_launcher->buildGamescopeArguments(profile);

    QVERIFY(args.contains("-w"));
    QVERIFY(args.contains("1920"));
    QVERIFY(args.contains("-h"));
    QVERIFY(args.contains("1080"));
    QVERIFY(args.contains("-W"));
    QVERIFY(args.contains("2560"));
    QVERIFY(args.contains("-H"));
    QVERIFY(args.contains("1440"));
}

void GamescopeLauncherTest::testBuildGamescopeArguments_WindowModes()
{
    Launch::PerformanceProfile profileFullscreen = Launch::PerformanceProfile::defaultProfile();
    profileFullscreen.windowMode = Launch::WindowMode::Fullscreen;
    QStringList argsFullscreen = m_launcher->buildGamescopeArguments(profileFullscreen);
    QVERIFY(argsFullscreen.contains("--fullscreen"));

    Launch::PerformanceProfile profileBorderless = Launch::PerformanceProfile::defaultProfile();
    profileBorderless.windowMode = Launch::WindowMode::Borderless;
    QStringList argsBorderless = m_launcher->buildGamescopeArguments(profileBorderless);
    QVERIFY(argsBorderless.contains("--borderless"));

    Launch::PerformanceProfile profileWindowed = Launch::PerformanceProfile::defaultProfile();
    profileWindowed.windowMode = Launch::WindowMode::Windowed;
    QStringList argsWindowed = m_launcher->buildGamescopeArguments(profileWindowed);
    QVERIFY(argsWindowed.contains("--windowed"));
}

void GamescopeLauncherTest::testBuildGamescopeArguments_VSync()
{
    Launch::PerformanceProfile profileVSync = Launch::PerformanceProfile::defaultProfile();
    profileVSync.vsync = true;
    QStringList argsVSync = m_launcher->buildGamescopeArguments(profileVSync);
    QVERIFY(argsVSync.contains("--vsync"));

    Launch::PerformanceProfile profileNoVSync = Launch::PerformanceProfile::defaultProfile();
    profileNoVSync.vsync = false;
    QStringList argsNoVSync = m_launcher->buildGamescopeArguments(profileNoVSync);
    QVERIFY(argsNoVSync.contains("--no-vsync"));
}

void GamescopeLauncherTest::testBuildEnvironment_Native()
{
    Launch::LaunchConfig config;
    config.platform = Launch::LaunchPlatform::Native;

    QVariantMap env = m_launcher->buildEnvironment(config);

    QVERIFY(env.contains("__GL_THREADED_OPTIMIZATIONS"));
    QCOMPARE(env.value("__GL_THREADED_OPTIMIZATIONS").toString(), QString("1"));
}

void GamescopeLauncherTest::testBuildEnvironment_Proton()
{
    Launch::LaunchConfig config;
    config.platform = Launch::LaunchPlatform::Proton;
    config.protonVersion = "8.0";
    config.winePrefix = "/home/user/.wine";

    QVariantMap env = m_launcher->buildEnvironment(config);

    QVERIFY(env.contains("PROTON_VERSION"));
    QCOMPARE(env.value("PROTON_VERSION").toString(), QString("8.0"));
    QVERIFY(env.contains("WINEPREFIX"));
    QCOMPARE(env.value("WINEPREFIX").toString(), QString("/home/user/.wine"));
    QVERIFY(env.contains("DXVK_HUD"));
    QVERIFY(env.contains("DXVK_STATE_CACHE"));
}

void GamescopeLauncherTest::testBuildEnvironment_Wine()
{
    Launch::LaunchConfig config;
    config.platform = Launch::LaunchPlatform::Wine;
    config.winePrefix = "/home/user/.wine";

    QVariantMap env = m_launcher->buildEnvironment(config);

    QVERIFY(env.contains("WINEPREFIX"));
    QCOMPARE(env.value("WINEPREFIX").toString(), QString("/home/user/.wine"));
    QVERIFY(env.contains("WINEDEBUG"));
    QCOMPARE(env.value("WINEDEBUG").toString(), QString("-all"));
}

void GamescopeLauncherTest::testBuildEnvironment_Steam()
{
    Launch::LaunchConfig config;
    config.platform = Launch::LaunchPlatform::Steam;

    QVariantMap env = m_launcher->buildEnvironment(config);

    QVERIFY(env.contains("STEAM_RUNTIME"));
    QCOMPARE(env.value("STEAM_RUNTIME").toString(), QString("1"));
}

void GamescopeLauncherTest::testBuildGamescopeCommand_NativeGame()
{
    Launch::LaunchConfig config;
    config.titleId = "game1";
    config.displayName = "Test Game";
    config.executable = "/usr/bin/testgame";
    config.platform = Launch::LaunchPlatform::Native;
    config.profile = Launch::PerformanceProfile::defaultProfile();
    config.profile.fpsCap = 60;

    QString command = m_launcher->buildGamescopeCommand(config);

    QVERIFY(command.contains("gamescope"));
    QVERIFY(command.contains("--fps 60"));
    QVERIFY(command.contains("--game-fps 60"));
    QVERIFY(command.contains("/usr/bin/testgame"));
}

void GamescopeLauncherTest::testBuildGamescopeCommand_ProtonGame()
{
    Launch::LaunchConfig config;
    config.titleId = "game2";
    config.displayName = "Proton Game";
    config.executable = "C:\\Game\\game.exe";
    config.platform = Launch::LaunchPlatform::Proton;
    config.protonPath = "proton-8.0";
    config.profile = Launch::PerformanceProfile::defaultProfile();

    QString command = m_launcher->buildGamescopeCommand(config);

    QVERIFY(command.contains("gamescope"));
    QVERIFY(command.contains("proton-8.0"));
    QVERIFY(command.contains("run"));
    QVERIFY(command.contains("C:\\Game\\game.exe"));
}

void GamescopeLauncherTest::testBuildGamescopeCommand_WineGame()
{
    Launch::LaunchConfig config;
    config.titleId = "game3";
    config.displayName = "Wine Game";
    config.executable = "C:\\Game\\game.exe";
    config.platform = Launch::LaunchPlatform::Wine;
    config.profile = Launch::PerformanceProfile::defaultProfile();

    QString command = m_launcher->buildGamescopeCommand(config);

    QVERIFY(command.contains("gamescope"));
    QVERIFY(command.contains("wine"));
    QVERIFY(command.contains("C:\\Game\\game.exe"));
}

void GamescopeLauncherTest::testBuildGamescopeCommand_WithoutGamescope()
{
    Launch::LaunchConfig config;
    config.titleId = "game4";
    config.displayName = "Direct Launch";
    config.executable = "/usr/bin/game";
    config.platform = Launch::LaunchPlatform::Native;
    config.useGamescope = false;
    config.profile = Launch::PerformanceProfile::defaultProfile();

    QString command = m_launcher->buildGamescopeCommand(config);

    QVERIFY(!command.contains("gamescope"));
    QVERIFY(command.contains("/usr/bin/game"));
}

void GamescopeLauncherTest::testPlatformConversion()
{
    QCOMPARE(Launch::platformToString(Launch::LaunchPlatform::Native), QString("native"));
    QCOMPARE(Launch::platformToString(Launch::LaunchPlatform::Steam), QString("steam"));
    QCOMPARE(Launch::platformToString(Launch::LaunchPlatform::Epic), QString("epic"));
    QCOMPARE(Launch::platformToString(Launch::LaunchPlatform::Wine), QString("wine"));
    QCOMPARE(Launch::platformToString(Launch::LaunchPlatform::Proton), QString("proton"));

    QCOMPARE(Launch::parsePlatform("native"), Launch::LaunchPlatform::Native);
    QCOMPARE(Launch::parsePlatform("STEAM"), Launch::LaunchPlatform::Steam);
    QCOMPARE(Launch::parsePlatform("Epic"), Launch::LaunchPlatform::Epic);
    QCOMPARE(Launch::parsePlatform("wine"), Launch::LaunchPlatform::Wine);
    QCOMPARE(Launch::parsePlatform("PROTON"), Launch::LaunchPlatform::Proton);
}

void GamescopeLauncherTest::testLaunchValidation()
{
    Launch::LaunchConfig config;
    config.dryRun = true;

    config.titleId = "";
    config.executable = "/usr/bin/game";
    config.profile = Launch::PerformanceProfile::defaultProfile();
    QVERIFY(!m_launcher->launch(config));

    config.titleId = "game1";
    config.executable = "";
    QVERIFY(!m_launcher->launch(config));

    config.executable = "/usr/bin/game";
    config.profile.fpsCap = -10;
    QVERIFY(m_launcher->launch(config));

    config.profile = Launch::PerformanceProfile::defaultProfile();
    QVERIFY(m_launcher->launch(config));
}

QTEST_MAIN(GamescopeLauncherTest)
#include "GamescopeLauncherTest.moc"
