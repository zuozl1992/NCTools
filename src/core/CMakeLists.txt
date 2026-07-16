# Core library - no UI dependency

add_library(nctools_core STATIC
    # DataProcessor
    dataprocessor/dataprocessor.h
    dataprocessor/dataprocessor.cpp
    # SettingsManager
    settings/settingsmanager.h
    settings/settingsmanager.cpp
    # LanguageManager
    language/languagemanager.h
    language/languagemanager.cpp
    # Transport layer
    transport/abstracttransport.h
    transport/abstracttransport.cpp
    transport/transporttype.h
    transport/transportconfig.h
    transport/transportfactory.h
    transport/transportfactory.cpp
    transport/serialtransport.h
    transport/serialtransport.cpp
    transport/tcpservertransport.h
    transport/tcpservertransport.cpp
    transport/tcpclienttransport.h
    transport/tcpclienttransport.cpp
    transport/udptransport.h
    transport/udptransport.cpp
)

target_include_directories(nctools_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(nctools_core PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::SerialPort
    Qt6::Network
)
