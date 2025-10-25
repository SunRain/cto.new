cmake_minimum_required(VERSION 3.16)
project(gameperf-manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

set(APP_SOURCES
    src/main.cpp
    src/application.cpp
    src/settingsmanager.cpp
    src/sourcemanager.cpp
)

set(APP_HEADERS
    src/application.h
    src/settingsmanager.h
    src/sourcemanager.h
)

qt6_add_resources(APP_RESOURCES
    resources.qrc
)

add_executable(gameperf-manager
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${APP_RESOURCES}
)

target_link_libraries(gameperf-manager PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

target_include_directories(gameperf-manager PRIVATE src)

add_executable(gameperf-daemon
    daemon/daemon.cpp
    daemon/performance.cpp
)

target_link_libraries(gameperf-daemon PRIVATE Qt6::Core)
target_include_directories(gameperf-daemon PRIVATE daemon)

install(TARGETS gameperf-manager RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS gameperf-daemon RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})

install(FILES resources/gameperf-manager.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES resources/icons/hicolor/64x64/apps/gameperf-manager.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps)
install(FILES resources/icons/hicolor/128x128/apps/gameperf-manager.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps)
install(FILES resources/icons/hicolor/scalable/apps/gameperf-manager.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)

install(PROGRAMS scripts/install-performance-daemon.sh DESTINATION lib/gameperf-manager)
install(FILES README.md DESTINATION share/doc/${PROJECT_NAME})
install(FILES LICENSE DESTINATION share/doc/${PROJECT_NAME})

install(FILES systemd/gameperf-daemon.service DESTINATION lib/systemd/system)
install(FILES polkit/org.gameperf.daemon.policy DESTINATION share/polkit-1/actions)
install(FILES udev/99-gameperf.rules DESTINATION lib/udev/rules.d)

set(CPACK_PACKAGE_NAME "gameperf-manager")
set(CPACK_PACKAGE_VENDOR "GamePerf Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gaming performance management and library integration")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "support@gameperf.example.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/packaging")
set(CPACK_GENERATOR "AppImage;DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "GamePerf Team")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt6-base-dev, qml6-module-qtquick, qml6-module-qtquick-controls, qml6-module-qtquick-layouts, qml6-module-qtquick-dialogs, libsdl2-2.0-0, policykit-1, systemd")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "legendary | legendary-gl")
set(CPACK_APPIMAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")

include(CPack)
