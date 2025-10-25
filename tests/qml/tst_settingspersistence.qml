import QtQuick
import QtTest
import App.Settings 1.0

TestCase {
    id: testCase
    name: "SettingsPersistenceTest"

    function init() {
        SettingsStore.restoreDefaults()
    }

    function test_networkSettings() {
        compare(SettingsStore.networkMode, "Auto")
        SettingsStore.networkMode = "Wireless"
        compare(SettingsStore.networkMode, "Wireless")

        SettingsStore.networkMetered = true
        compare(SettingsStore.networkMetered, true)
    }

    function test_displaySettings() {
        compare(SettingsStore.displayScale, 1.0)
        SettingsStore.displayScale = 1.5
        compare(SettingsStore.displayScale, 1.5)
    }

    function test_audioSettings() {
        compare(SettingsStore.audioOutput, "Default")
        SettingsStore.audioOutput = "HDMI"
        compare(SettingsStore.audioOutput, "HDMI")

        compare(SettingsStore.audioVolume, 70)
        SettingsStore.audioVolume = 50
        compare(SettingsStore.audioVolume, 50)
    }

    function test_languageSettings() {
        compare(SettingsStore.language, "en-US")
        SettingsStore.language = "es-ES"
        compare(SettingsStore.language, "es-ES")
    }

    function test_accessibilitySettings() {
        compare(SettingsStore.textScale, 1.0)
        SettingsStore.textScale = 1.25
        compare(SettingsStore.textScale, 1.25)

        compare(SettingsStore.highContrast, false)
        SettingsStore.highContrast = true
        compare(SettingsStore.highContrast, true)

        compare(SettingsStore.captionsEnabled, false)
        SettingsStore.captionsEnabled = true
        compare(SettingsStore.captionsEnabled, true)
    }

    function test_accountSettings() {
        compare(SettingsStore.steamLoggedIn, false)
        SettingsStore.steamLoggedIn = true
        compare(SettingsStore.steamLoggedIn, true)
        verify(SettingsStore.steamTroubleshootingHint.length > 0)

        compare(SettingsStore.epicLoggedIn, false)
        SettingsStore.epicLoggedIn = true
        compare(SettingsStore.epicLoggedIn, true)
        verify(SettingsStore.epicTroubleshootingHint.length > 0)

        compare(SettingsStore.legendaryCliPath, "/usr/bin/legendary")
        SettingsStore.legendaryCliPath = "/usr/local/bin/legendary"
        compare(SettingsStore.legendaryCliPath, "/usr/local/bin/legendary")
    }

    function test_downloadSettings() {
        compare(SettingsStore.downloadProgress, 0.0)
        SettingsStore.downloadProgress = 0.5
        compare(SettingsStore.downloadProgress, 0.5)
    }

    function test_restoreDefaults() {
        SettingsStore.displayScale = 2.0
        SettingsStore.textScale = 1.5
        SettingsStore.highContrast = true
        SettingsStore.audioVolume = 20
        SettingsStore.steamLoggedIn = true
        SettingsStore.epicLoggedIn = true
        SettingsStore.legendaryCliPath = "/tmp/custom"
        SettingsStore.downloadProgress = 0.8
        
        SettingsStore.restoreDefaults()
        
        compare(SettingsStore.displayScale, 1.0)
        compare(SettingsStore.textScale, 1.0)
        compare(SettingsStore.highContrast, false)
        compare(SettingsStore.audioVolume, 70)
        compare(SettingsStore.steamLoggedIn, false)
        compare(SettingsStore.epicLoggedIn, false)
        compare(SettingsStore.legendaryCliPath, "/usr/bin/legendary")
        compare(SettingsStore.downloadProgress, 0.0)
    }

    function test_settingsSync() {
        SettingsStore.networkMode = "Wired"
        SettingsStore.sync()
        compare(SettingsStore.networkMode, "Wired")
    }

    function test_rawValueAccess() {
        SettingsStore.networkMode = "Offline"
        var value = SettingsStore.rawValue("network", "mode")
        compare(value, "Offline")
    }

    function test_settingsReload() {
        SettingsStore.displayScale = 1.5
        SettingsStore.sync()
        SettingsStore.reload()
        compare(SettingsStore.displayScale, 1.5)
    }
}
