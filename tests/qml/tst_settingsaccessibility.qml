import QtQuick
import QtTest
import App.Shared 1.0
import App.Settings 1.0

TestCase {
    name: "SettingsAccessibilityTest"

    function init() {
        SettingsStore.restoreDefaults()
    }

    function test_themeReactsToHighContrast() {
        var initialBgColor = AppTheme.backgroundColor
        SettingsStore.highContrast = true
        wait(10)
        verify(AppTheme.backgroundColor !== initialBgColor)
        compare(AppTheme.highContrast, true)
    }

    function test_themeReactsToTextScale() {
        var initialFontSize = AppTheme.bodyFontSize
        SettingsStore.textScale = 1.5
        wait(10)
        verify(AppTheme.bodyFontSize > initialFontSize)
    }

    function test_themeReactsToDisplayScale() {
        var initialMargin = AppTheme.margin
        SettingsStore.displayScale = 1.25
        wait(10)
        verify(AppTheme.margin > initialMargin)
    }

    function test_highContrastColorValues() {
        SettingsStore.highContrast = false
        var normalBackground = AppTheme.backgroundColor
        SettingsStore.highContrast = true
        verify(AppTheme.backgroundColor === "#000000")
    }

    function test_focusBorderWidth() {
        SettingsStore.highContrast = false
        compare(AppTheme.focusBorderWidth, 2)
        SettingsStore.highContrast = true
        compare(AppTheme.focusBorderWidth, 3)
    }

    function test_radiusInHighContrast() {
        SettingsStore.highContrast = false
        verify(AppTheme.radius > 2)
        SettingsStore.highContrast = true
        compare(AppTheme.radius, 2)
    }

    function test_captions() {
        SettingsStore.captionsEnabled = true
        compare(SettingsStore.captionsEnabled, true)
        SettingsStore.captionsEnabled = false
        compare(SettingsStore.captionsEnabled, false)
    }
}
