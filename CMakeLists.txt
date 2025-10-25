cmake_minimum_required(VERSION 3.21)

project(QtQuickApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Qml
    Quick
    QuickControls2
    DBus
    LinguistTools
    Core5Compat
)

include(QtHelpers)
qt_standard_project_setup()

set(PROJECT_SOURCES
    src/main.cpp
    src/ui/NavigationModel.h
    src/ui/NavigationModel.cpp
    src/ui/DataModel.h
    src/ui/DataModel.cpp
    src/ui/TranslationStore.h
    src/ui/TranslationStore.cpp
)

set(PROJECT_RESOURCES
    qml/qml.qrc
)

qt_add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_RESOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Core5Compat
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE QT_QML_DEBUG)
endif()

enable_testing()
add_subdirectory(tests)
