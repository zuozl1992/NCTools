# Bridge library - connects core to QML

add_library(mysscom_bridge STATIC
    transportcontroller.h
    transportcontroller.cpp
    quicksendmodel.h
    quicksendmodel.cpp
    appcontroller.h
    appcontroller.cpp
)

target_include_directories(mysscom_bridge PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mysscom_bridge PUBLIC
    Qt6::Core
    Qt6::Qml
    mysscom_core
)
