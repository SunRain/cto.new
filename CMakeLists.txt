cmake_minimum_required(VERSION 3.21)

project(SettingsExperience VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick QuickControls2 Test)

qt_standard_project_setup()

qt_add_qml_module(settings_store_plugin
    URI App.Settings
    VERSION 1.0
    SOURCES
        src/settings/SettingsStore.cpp
        src/settings/SettingsStore.h
)

target_link_libraries(settings_store_plugin
    PRIVATE Qt6::Core
    PRIVATE Qt6::Qml
    PRIVATE Qt6::Quick
)

enable_testing()

find_program(QMLTESTRUNNER_EXECUTABLE qmltestrunner REQUIRED)

if(NOT QMLTESTRUNNER_EXECUTABLE)
    message(FATAL_ERROR "qmltestrunner not found")
endif()

add_test(NAME SettingsQmlTests
    COMMAND ${QMLTESTRUNNER_EXECUTABLE}
        -input ${CMAKE_CURRENT_SOURCE_DIR}/tests/qml
        -import ${CMAKE_CURRENT_SOURCE_DIR}/qml
        -import ${CMAKE_CURRENT_BINARY_DIR}/qt6/imports
)

set_tests_properties(SettingsQmlTests PROPERTIES
    ENVIRONMENT "QML2_IMPORT_PATH=${CMAKE_CURRENT_SOURCE_DIR}/qml;${CMAKE_CURRENT_BINARY_DIR}/qt6/imports"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
