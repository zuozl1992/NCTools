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
    nctools_bridge
    Qt6::Test
)
target_include_directories(test_transportfactory PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/bridge
)
add_test(NAME transportfactory COMMAND test_transportfactory)

qt_add_executable(test_appcontroller test_appcontroller.cpp)
target_link_libraries(test_appcontroller PRIVATE nctools_bridge Qt6::Test)
target_include_directories(test_appcontroller PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/bridge
)
add_test(NAME appcontroller COMMAND test_appcontroller)

qt_add_executable(test_qmlflows test_qmlflows.cpp)
target_link_libraries(test_qmlflows PRIVATE nctools_bridge Qt6::Test Qt6::Quick Qt6::QuickControls2)
target_include_directories(test_qmlflows PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/bridge
)
target_compile_definitions(test_qmlflows PRIVATE NCTOOLS_APP_SOURCE_DIR="${CMAKE_SOURCE_DIR}/src/app")
add_test(NAME qmlflows COMMAND test_qmlflows)
set_tests_properties(qmlflows PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
