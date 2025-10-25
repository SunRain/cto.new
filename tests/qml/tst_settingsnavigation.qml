import QtQuick
import QtQuick.Controls
import QtTest
import Settings 1.0
import App.Settings 1.0

TestCase {
    name: "SettingsNavigationTest"

    Item {
        id: testRoot
        width: 1280
        height: 720

        SettingsHome {
            id: settingsHome
            anchors.fill: parent
        }
    }

    function initTestCase() {
        waitForRendering(settingsHome)
        settingsHome.forceActiveFocus()
    }

    function test_initialSection() {
        compare(settingsHome.currentPageIndex, 0)
    }

    function test_navigationDown() {
        keyClick(settingsHome, Qt.Key_Down)
        compare(settingsHome.currentPageIndex, 1)
        keyClick(settingsHome, Qt.Key_Down)
        compare(settingsHome.currentPageIndex, 2)
    }

    function test_navigationUp() {
        keyClick(settingsHome, Qt.Key_Down)
        keyClick(settingsHome, Qt.Key_Down)
        compare(settingsHome.currentPageIndex, 2)
        keyClick(settingsHome, Qt.Key_Up)
        compare(settingsHome.currentPageIndex, 1)
    }

    function test_pageSwitchLoadsContent() {
        var initialIndex = settingsHome.currentPageIndex
        keyClick(settingsHome, Qt.Key_Down)
        keyClick(settingsHome, Qt.Key_Down)
        keyClick(settingsHome, Qt.Key_Down)
        wait(50)
        keyClick(settingsHome, Qt.Key_Return)
        wait(50)
        keyClick(settingsHome, Qt.Key_Down)
        wait(50)
        verify(settingsHome.currentPageIndex !== initialIndex)
    }

    function cleanup() {
        SettingsStore.restoreDefaults()
    }
}
