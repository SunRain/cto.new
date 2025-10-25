#pragma once

#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <QQmlEngine>
#include <QJSEngine>

class SettingsStore : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString networkMode READ networkMode WRITE setNetworkMode NOTIFY networkModeChanged)
    Q_PROPERTY(bool networkMetered READ networkMetered WRITE setNetworkMetered NOTIFY networkMeteredChanged)
    Q_PROPERTY(QStringList availableNetworkModes READ availableNetworkModes CONSTANT)

    Q_PROPERTY(double displayScale READ displayScale WRITE setDisplayScale NOTIFY displayScaleChanged)
    Q_PROPERTY(QStringList displayScaleOptions READ displayScaleOptions CONSTANT)

    Q_PROPERTY(QString audioOutput READ audioOutput WRITE setAudioOutput NOTIFY audioOutputChanged)
    Q_PROPERTY(QStringList availableAudioOutputs READ availableAudioOutputs CONSTANT)
    Q_PROPERTY(int audioVolume READ audioVolume WRITE setAudioVolume NOTIFY audioVolumeChanged)

    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages CONSTANT)

    Q_PROPERTY(double textScale READ textScale WRITE setTextScale NOTIFY textScaleChanged)
    Q_PROPERTY(bool highContrast READ highContrast WRITE setHighContrast NOTIFY highContrastChanged)
    Q_PROPERTY(bool captionsEnabled READ captionsEnabled WRITE setCaptionsEnabled NOTIFY captionsEnabledChanged)

    Q_PROPERTY(bool steamLoggedIn READ steamLoggedIn WRITE setSteamLoggedIn NOTIFY steamLoggedInChanged)
    Q_PROPERTY(QString steamTroubleshootingHint READ steamTroubleshootingHint NOTIFY steamTroubleshootingHintChanged)

    Q_PROPERTY(bool epicLoggedIn READ epicLoggedIn WRITE setEpicLoggedIn NOTIFY epicLoggedInChanged)
    Q_PROPERTY(QString epicTroubleshootingHint READ epicTroubleshootingHint NOTIFY epicTroubleshootingHintChanged)

    Q_PROPERTY(QString legendaryCliPath READ legendaryCliPath WRITE setLegendaryCliPath NOTIFY legendaryCliPathChanged)
    Q_PROPERTY(double downloadProgress READ downloadProgress WRITE setDownloadProgress NOTIFY downloadProgressChanged)

public:
    explicit SettingsStore(QObject *parent = nullptr);
    static SettingsStore *create(QQmlEngine *engine, QJSEngine *scriptEngine);

    QString networkMode() const;
    void setNetworkMode(const QString &mode);

    bool networkMetered() const;
    void setNetworkMetered(bool metered);
    QStringList availableNetworkModes() const;

    double displayScale() const;
    void setDisplayScale(double scale);
    QStringList displayScaleOptions() const;

    QString audioOutput() const;
    void setAudioOutput(const QString &output);
    QStringList availableAudioOutputs() const;

    int audioVolume() const;
    void setAudioVolume(int volume);

    QString language() const;
    void setLanguage(const QString &language);
    QStringList availableLanguages() const;

    double textScale() const;
    void setTextScale(double scale);

    bool highContrast() const;
    void setHighContrast(bool enabled);

    bool captionsEnabled() const;
    void setCaptionsEnabled(bool enabled);

    bool steamLoggedIn() const;
    void setSteamLoggedIn(bool loggedIn);
    QString steamTroubleshootingHint() const;

    bool epicLoggedIn() const;
    void setEpicLoggedIn(bool loggedIn);
    QString epicTroubleshootingHint() const;

    QString legendaryCliPath() const;
    void setLegendaryCliPath(const QString &path);

    double downloadProgress() const;
    void setDownloadProgress(double value);

    Q_INVOKABLE void restoreDefaults();
    Q_INVOKABLE void reload();
    Q_INVOKABLE void sync();
    Q_INVOKABLE QVariant rawValue(const QString &group, const QString &key) const;

signals:
    void networkModeChanged();
    void networkMeteredChanged();
    void displayScaleChanged();
    void audioOutputChanged();
    void audioVolumeChanged();
    void languageChanged();
    void textScaleChanged();
    void highContrastChanged();
    void captionsEnabledChanged();
    void steamLoggedInChanged();
    void steamTroubleshootingHintChanged();
    void epicLoggedInChanged();
    void epicTroubleshootingHintChanged();
    void legendaryCliPathChanged();
    void downloadProgressChanged();

private:
    void loadFromSettings();
    void persistValue(const QString &group, const QString &key, const QVariant &value);
    QVariant readValue(const QString &group, const QString &key, const QVariant &defaultValue) const;

    QString composeKey(const QString &group, const QString &key) const;
    static QString resolveSettingsFilePath();

    bool m_blockPersistence = false;
    QSettings m_settings;

    QString m_networkMode;
    bool m_networkMetered = false;
    double m_displayScale = 1.0;
    QString m_audioOutput;
    int m_audioVolume = 70;
    QString m_language;
    double m_textScale = 1.0;
    bool m_highContrast = false;
    bool m_captionsEnabled = false;
    bool m_steamLoggedIn = false;
    bool m_epicLoggedIn = false;
    QString m_legendaryCliPath;
    double m_downloadProgress = 0.0;
    QString m_steamHint;
    QString m_epicHint;

    const QStringList m_networkModes;
    const QStringList m_displayScaleOptions;
    const QStringList m_audioOutputs;
    const QStringList m_languages;
};
