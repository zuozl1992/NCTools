# Application executable
set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/AppIcon.icns")

qt_add_executable(MySSCOM
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${ICON_FILE}
)

# QML files from local directory
qt_add_qml_module(MySSCOM
    URI MySSCOM
    VERSION 1.0
    QML_FILES
        Main.qml
        Style.qml
        HeaderBar.qml
        SerialConfigPanel.qml
        TcpServerConfigPanel.qml
        TcpClientConfigPanel.qml
        UdpConfigPanel.qml
        DataDisplay.qml
        SendPanel.qml
        MyToolBar.qml
        QuickSendPanel.qml
        FileSendProgress.qml
        StatusBar.qml
        AboutDialog.qml
        SerialSettingsDialog.qml
    RESOURCES
        control.png
)

set_target_properties(MySSCOM PROPERTIES
    MACOSX_BUNDLE_ICON_FILE AppIcon.icns
    MACOSX_BUNDLE_BUNDLE_NAME "MySSCOM"
    MACOSX_BUNDLE_BUNDLE_VERSION "2.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "2.0"
    RESOURCE "${ICON_FILE}"
)

qt_add_translations(
    TARGETS MySSCOM
    TS_FILES
        MySSCOM_zh_CN.ts
        MySSCOM_en_US.ts
)

target_link_libraries(MySSCOM PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    mysscom_core
    mysscom_bridge
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
