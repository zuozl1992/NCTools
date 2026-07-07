cmake_minimum_required(VERSION 3.21)
project(MySSCOM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Widgets Qml Quick QuickControls2 SerialPort Network LinguistTools)

qt_standard_project_setup(REQUIRES 6.8)

set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc")

qt_add_executable(MySSCOM
    WIN32 MACOSX_BUNDLE
    main.cpp
    src/backend/transportinterface.h
    src/backend/serialtransport.h
    src/backend/serialtransport.cpp
    src/backend/tcpclienttransport.h
    src/backend/tcpclienttransport.cpp
    src/backend/tcpservertransport.h
    src/backend/tcpservertransport.cpp
    src/backend/udptransport.h
    src/backend/udptransport.cpp
    src/backend/transportmanager.h
    src/backend/transportmanager.cpp
    ${APP_ICON_RESOURCE}
)

set_target_properties(MySSCOM PROPERTIES
    MACOSX_BUNDLE_ICON_FILE control.png
    MACOSX_BUNDLE_BUNDLE_NAME "MySSCOM"
    MACOSX_BUNDLE_BUNDLE_VERSION "2.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "2.0"
)

qt_add_qml_module(MySSCOM
    URI MySSCOM
    VERSION 1.0
    QML_FILES
        src/ui/qml/main.qml
)

qt_add_translations(
    TARGETS MySSCOM
    TS_FILES MySSCOM_zh_CN.ts
)

target_link_libraries(MySSCOM
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Qml
        Qt::Quick
        Qt::QuickControls2
        Qt::SerialPort
        Qt::Network
)

install(TARGETS MySSCOM
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MySSCOM
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
