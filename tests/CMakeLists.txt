enable_testing()

add_test(NAME QmlSmokeTest
    COMMAND qmltestrunner -input ${CMAKE_SOURCE_DIR}/tests/qmltests)
