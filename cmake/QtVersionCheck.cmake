# Verify Qt version is at least 6.8
if(Qt6_VERSION VERSION_LESS "6.8")
    message(FATAL_ERROR "This project requires Qt 6.8 or later. Found Qt ${Qt6_VERSION}")
else()
    message(STATUS "Using Qt ${Qt6_VERSION}")
endif()
