# Application executable
set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/AppIcon.icns")

qt_add_executable(NCTools
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${ICON_FILE}
)

# QML files from local directory
qt_add_qml_module(NCTools
    URI NCTools
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

set_target_properties(NCTools PROPERTIES
    MACOSX_BUNDLE_ICON_FILE AppIcon.icns
    MACOSX_BUNDLE_BUNDLE_NAME "NCTools"
    MACOSX_BUNDLE_BUNDLE_VERSION "2.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "2.0"
    RESOURCE "${ICON_FILE}"
)

qt_add_translations(
    TARGETS NCTools
    TS_FILES
        NCTools_zh_CN.ts
        NCTools_en_US.ts
)

target_link_libraries(NCTools PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    mysscom_core
    mysscom_bridge
)

install(TARGETS NCTools
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET NCTools
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
