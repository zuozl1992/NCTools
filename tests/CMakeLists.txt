enable_testing()

# DataProcessor test
qt_add_executable(test_dataprocessor
    test_dataprocessor.cpp
)
target_link_libraries(test_dataprocessor PRIVATE
    nctools_core
    Qt6::Test
)
target_include_directories(test_dataprocessor PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
)
add_test(NAME dataprocessor COMMAND test_dataprocessor)

# SettingsManager test
qt_add_executable(test_settingsmanager
    test_settingsmanager.cpp
)
target_link_libraries(test_settingsmanager PRIVATE
    nctools_core
    Qt6::Test
)
target_include_directories(test_settingsmanager PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
)
add_test(NAME settingsmanager COMMAND test_settingsmanager)

# TransportFactory test
qt_add_executable(test_transportfactory
    test_transportfactory.cpp
)
target_link_libraries(test_transportfactory PRIVATE
    nctools_core
    Qt6::Test
)
target_include_directories(test_transportfactory PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
)
add_test(NAME transportfactory COMMAND test_transportfactory)
