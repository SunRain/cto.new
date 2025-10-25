cmake_minimum_required(VERSION 3.16)
project(RunningManagerRuntime LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.4 COMPONENTS Core Quick QuickControls2 Test QUIET)

if(NOT Qt6_FOUND)
    find_package(Qt6 6.2 COMPONENTS Core Quick QuickControls2 Test REQUIRED)
endif()

qt_standard_project_setup()

add_library(runtime_manager STATIC
    src/runtime/RunningManager.hpp
    src/runtime/RunningManager.cpp
    src/runtime/ProcessMetricsProvider.hpp
    src/runtime/ProcessMetricsProvider.cpp
)

target_include_directories(runtime_manager
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(runtime_manager
    PUBLIC
        Qt6::Core
        Qt6::Quick
)

qt_add_executable(runtime_manager_app
    src/main.cpp
)

target_link_libraries(runtime_manager_app
    PRIVATE
        runtime_manager
        Qt6::Core
        Qt6::Quick
        Qt6::QuickControls2
)

qt_add_qml_module(runtime_manager_app
    URI RuntimeOverlay
    VERSION 1.0
    QML_FILES
        qml/runtime/RunningOverlay.qml
)

qt_add_executable(runtime_manager_tests
    tests/RunningManagerTest.cpp
)

target_include_directories(runtime_manager_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(runtime_manager_tests
    PRIVATE
        runtime_manager
        Qt6::Core
        Qt6::Test
)

enable_testing()
add_test(NAME RunningManagerTests COMMAND runtime_manager_tests)
