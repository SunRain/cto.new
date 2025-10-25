#include "SettingsStore.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QtGlobal>
#include <QtMath>

namespace
{
constexpr double kDoubleTolerance = 0.0001;
constexpr int kDefaultAudioVolume = 70;
constexpr double kDefaultScale = 1.0;
constexpr double kDefaultTextScale = 1.0;
constexpr double kDefaultDownloadProgress = 0.0;
}

SettingsStore::SettingsStore(QObject *parent)
    : QObject(parent)
    , m_settings(resolveSettingsFilePath(), QSettings::IniFormat)
    , m_networkModes({QStringLiteral("Auto"), QStringLiteral("Wired"), QStringLiteral("Wireless"), QStringLiteral("Offline")})
    , m_displayScaleOptions({QStringLiteral("0.75"), QStringLiteral("1.0"), QStringLiteral("1.25"), QStringLiteral("1.5"), QStringLiteral("2.0")})
    , m_audioOutputs({QStringLiteral("Default"), QStringLiteral("HDMI"), QStringLiteral("Speakers"), QStringLiteral("Headphones")})
    , m_languages({QStringLiteral("en-US"), QStringLiteral("es-ES"), QStringLiteral("fr-FR"), QStringLiteral("de-DE"), QStringLiteral("ja-JP"), QStringLiteral("zh-CN")})
{
    m_settings.setFallbacksEnabled(false);
    loadFromSettings();
}

SettingsStore *SettingsStore::create(QQmlEngine *engine, QJSEngine *)
{
    static SettingsStore *instance = nullptr;
    if (!instance) {
        instance = new SettingsStore(engine);
        engine->setObjectOwnership(instance, QQmlEngine::CppOwnership);
    }
    return instance;
}

void SettingsStore::loadFromSettings()
{
    const bool previousBlock = m_blockPersistence;
    m_blockPersistence = true;

    setNetworkMode(readValue(QStringLiteral("network"), QStringLiteral("mode"), QStringLiteral("Auto")).toString());
    setNetworkMetered(readValue(QStringLiteral("network"), QStringLiteral("metered"), false).toBool());

    setDisplayScale(readValue(QStringLiteral("display"), QStringLiteral("scale"), kDefaultScale).toDouble());

    setAudioOutput(readValue(QStringLiteral("audio"), QStringLiteral("output"), QStringLiteral("Default")).toString());
    setAudioVolume(readValue(QStringLiteral("audio"), QStringLiteral("volume"), kDefaultAudioVolume).toInt());

    setLanguage(readValue(QStringLiteral("language"), QStringLiteral("code"), QStringLiteral("en-US")).toString());

    setTextScale(readValue(QStringLiteral("accessibility"), QStringLiteral("textScale"), kDefaultTextScale).toDouble());
    setHighContrast(readValue(QStringLiteral("accessibility"), QStringLiteral("highContrast"), false).toBool());
    setCaptionsEnabled(readValue(QStringLiteral("accessibility"), QStringLiteral("captions"), false).toBool());

    setSteamLoggedIn(readValue(QStringLiteral("accounts"), QStringLiteral("steamLoggedIn"), false).toBool());
    setEpicLoggedIn(readValue(QStringLiteral("accounts"), QStringLiteral("epicLoggedIn"), false).toBool());
    setLegendaryCliPath(readValue(QStringLiteral("accounts"), QStringLiteral("legendaryCliPath"), QStringLiteral("/usr/bin/legendary")).toString());

    setDownloadProgress(readValue(QStringLiteral("downloads"), QStringLiteral("currentProgress"), kDefaultDownloadProgress).toDouble());

    m_blockPersistence = previousBlock;
}

void SettingsStore::persistValue(const QString &group, const QString &key, const QVariant &value)
{
    if (m_blockPersistence) {
        return;
    }

    m_settings.setValue(composeKey(group, key), value);
    m_settings.sync();
}

QVariant SettingsStore::readValue(const QString &group, const QString &key, const QVariant &defaultValue) const
{
    return m_settings.value(composeKey(group, key), defaultValue);
}

QString SettingsStore::composeKey(const QString &group, const QString &key) const
{
    return QStringLiteral("%1/%2").arg(group, key);
}

QString SettingsStore::resolveSettingsFilePath()
{
    const QString overridePath = qEnvironmentVariable("SETTINGS_STORE_PATH");
    if (!overridePath.isEmpty()) {
        const QFileInfo overrideInfo(overridePath);
        QDir().mkpath(overrideInfo.absolutePath());
        return overridePath;
    }

    const QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir dir(baseDir);
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }
    return dir.filePath(QStringLiteral("settings.ini"));
}

QString SettingsStore::networkMode() const
{
    return m_networkMode;
}

void SettingsStore::setNetworkMode(const QString &mode)
{
    QString normalized = mode;
    if (!m_networkModes.contains(normalized)) {
        normalized = m_networkModes.first();
    }

    if (m_networkMode != normalized) {
        m_networkMode = normalized;
        persistValue(QStringLiteral("network"), QStringLiteral("mode"), normalized);
        emit networkModeChanged();
    }
}

bool SettingsStore::networkMetered() const
{
    return m_networkMetered;
}

void SettingsStore::setNetworkMetered(bool metered)
{
    if (m_networkMetered != metered) {
        m_networkMetered = metered;
        persistValue(QStringLiteral("network"), QStringLiteral("metered"), metered);
        emit networkMeteredChanged();
    }
}

QStringList SettingsStore::availableNetworkModes() const
{
    return m_networkModes;
}

double SettingsStore::displayScale() const
{
    return m_displayScale;
}

void SettingsStore::setDisplayScale(double scale)
{
    const double bounded = qBound(0.75, scale, 2.0);
    if (qAbs(m_displayScale - bounded) > kDoubleTolerance) {
        m_displayScale = bounded;
        persistValue(QStringLiteral("display"), QStringLiteral("scale"), bounded);
        emit displayScaleChanged();
    }
}

QStringList SettingsStore::displayScaleOptions() const
{
    return m_displayScaleOptions;
}

QString SettingsStore::audioOutput() const
{
    return m_audioOutput;
}

void SettingsStore::setAudioOutput(const QString &output)
{
    QString normalized = output;
    if (!m_audioOutputs.contains(normalized)) {
        normalized = m_audioOutputs.first();
    }

    if (m_audioOutput != normalized) {
        m_audioOutput = normalized;
        persistValue(QStringLiteral("audio"), QStringLiteral("output"), normalized);
        emit audioOutputChanged();
    }
}

QStringList SettingsStore::availableAudioOutputs() const
{
    return m_audioOutputs;
}

int SettingsStore::audioVolume() const
{
    return m_audioVolume;
}

void SettingsStore::setAudioVolume(int volume)
{
    const int bounded = qBound(0, volume, 100);
    if (m_audioVolume != bounded) {
        m_audioVolume = bounded;
        persistValue(QStringLiteral("audio"), QStringLiteral("volume"), bounded);
        emit audioVolumeChanged();
    }
}

QString SettingsStore::language() const
{
    return m_language;
}

void SettingsStore::setLanguage(const QString &language)
{
    QString normalized = language;
    if (!m_languages.contains(normalized)) {
        normalized = m_languages.first();
    }

    if (m_language != normalized) {
        m_language = normalized;
        persistValue(QStringLiteral("language"), QStringLiteral("code"), normalized);
        emit languageChanged();
    }
}

QStringList SettingsStore::availableLanguages() const
{
    return m_languages;
}

double SettingsStore::textScale() const
{
    return m_textScale;
}

void SettingsStore::setTextScale(double scale)
{
    const double bounded = qBound(0.75, scale, 1.75);
    if (qAbs(m_textScale - bounded) > kDoubleTolerance) {
        m_textScale = bounded;
        persistValue(QStringLiteral("accessibility"), QStringLiteral("textScale"), bounded);
        emit textScaleChanged();
    }
}

bool SettingsStore::highContrast() const
{
    return m_highContrast;
}

void SettingsStore::setHighContrast(bool enabled)
{
    if (m_highContrast != enabled) {
        m_highContrast = enabled;
        persistValue(QStringLiteral("accessibility"), QStringLiteral("highContrast"), enabled);
        emit highContrastChanged();
    }
}

bool SettingsStore::captionsEnabled() const
{
    return m_captionsEnabled;
}

void SettingsStore::setCaptionsEnabled(bool enabled)
{
    if (m_captionsEnabled != enabled) {
        m_captionsEnabled = enabled;
        persistValue(QStringLiteral("accessibility"), QStringLiteral("captions"), enabled);
        emit captionsEnabledChanged();
    }
}

bool SettingsStore::steamLoggedIn() const
{
    return m_steamLoggedIn;
}

void SettingsStore::setSteamLoggedIn(bool loggedIn)
{
    if (m_steamLoggedIn != loggedIn) {
        m_steamLoggedIn = loggedIn;
        persistValue(QStringLiteral("accounts"), QStringLiteral("steamLoggedIn"), loggedIn);
        m_steamHint = loggedIn ? QStringLiteral("Connected successfully")
                               : QStringLiteral("Not connected. Check Steam client is running.");
        emit steamLoggedInChanged();
        emit steamTroubleshootingHintChanged();
    }
}

QString SettingsStore::steamTroubleshootingHint() const
{
    return m_steamHint;
}

bool SettingsStore::epicLoggedIn() const
{
    return m_epicLoggedIn;
}

void SettingsStore::setEpicLoggedIn(bool loggedIn)
{
    if (m_epicLoggedIn != loggedIn) {
        m_epicLoggedIn = loggedIn;
        persistValue(QStringLiteral("accounts"), QStringLiteral("epicLoggedIn"), loggedIn);
        m_epicHint = loggedIn ? QStringLiteral("Connected via Legendary")
                              : QStringLiteral("Not connected. Configure Legendary CLI path and authenticate.");
        emit epicLoggedInChanged();
        emit epicTroubleshootingHintChanged();
    }
}

QString SettingsStore::epicTroubleshootingHint() const
{
    return m_epicHint;
}

QString SettingsStore::legendaryCliPath() const
{
    return m_legendaryCliPath;
}

void SettingsStore::setLegendaryCliPath(const QString &path)
{
    const QString trimmed = path.trimmed();
    if (m_legendaryCliPath != trimmed) {
        m_legendaryCliPath = trimmed;
        persistValue(QStringLiteral("accounts"), QStringLiteral("legendaryCliPath"), trimmed);
        emit legendaryCliPathChanged();
    }
}

double SettingsStore::downloadProgress() const
{
    return m_downloadProgress;
}

void SettingsStore::setDownloadProgress(double value)
{
    const double bounded = qBound(0.0, value, 1.0);
    if (qAbs(m_downloadProgress - bounded) > kDoubleTolerance) {
        m_downloadProgress = bounded;
        persistValue(QStringLiteral("downloads"), QStringLiteral("currentProgress"), bounded);
        emit downloadProgressChanged();
    }
}

void SettingsStore::restoreDefaults()
{
    const bool previousBlock = m_blockPersistence;
    m_blockPersistence = false;

    setNetworkMode(QStringLiteral("Auto"));
    setNetworkMetered(false);
    setDisplayScale(kDefaultScale);
    setAudioOutput(QStringLiteral("Default"));
    setAudioVolume(kDefaultAudioVolume);
    setLanguage(QStringLiteral("en-US"));
    setTextScale(kDefaultTextScale);
    setHighContrast(false);
    setCaptionsEnabled(false);
    setSteamLoggedIn(false);
    setEpicLoggedIn(false);
    setLegendaryCliPath(QStringLiteral("/usr/bin/legendary"));
    setDownloadProgress(kDefaultDownloadProgress);

    m_blockPersistence = previousBlock;
    sync();
}

void SettingsStore::reload()
{
    loadFromSettings();
}

void SettingsStore::sync()
{
    m_settings.sync();
}

QVariant SettingsStore::rawValue(const QString &group, const QString &key) const
{
    return readValue(group, key, QVariant());
}
