SET(IMAGE_PLUGIN_SOURCES
    GAIAnimationIO.cpp
    GIImageIO.cpp
    PSDImageIO.cpp
    HAIAnimationIO.cpp
    QtOpenSRImagePlugin.cpp
)

SET(IMAGE_PLUGIN_HEADERS
    GAIAnimationIO.h
    GIImageIO.h
    PSDImageIO.h
    HAIAnimationIO.h
    QtOpenSRImagePlugin.h
)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/include
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
)

QT5_WRAP_CPP(IMAGE_PLUGIN_MOC_SOURCES ${IMAGE_PLUGIN_HEADERS})
ADD_LIBRARY(QtOpenSRImagePlugin SHARED ${IMAGE_PLUGIN_SOURCES} ${IMAGE_PLUGIN_MOC_SOURCES})
TARGET_LINK_LIBRARIES(QtOpenSRImagePlugin RangerQt)
IF(WIN32)
    SET_TARGET_PROPERTIES(QtOpenSRImagePlugin PROPERTIES PREFIX "")
ENDIF()
