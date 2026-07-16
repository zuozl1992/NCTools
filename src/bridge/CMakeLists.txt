# Bridge library - connects core to QML

add_library(nctools_bridge STATIC
    transportcontroller.h
    transportcontroller.cpp
    quicksendmodel.h
    quicksendmodel.cpp
    appcontroller.h
    appcontroller.cpp
)

target_include_directories(nctools_bridge PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(nctools_bridge PUBLIC
    Qt6::Core
    Qt6::Qml
    nctools_core
)
