CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
project (ueye_streamer)

set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
set (VERSION_BUILD 0)
set (VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR})

set(ADDITIONAL_INCLUDEDIR "" CACHE FILEPATH "Path to additonal includes")
set(ADDITIONAL_LIBRARYDIR "" CACHE FILEPATH "Path to additional libraries")
set(USE_QT5 "" CACHE BOOL "Use QT5 toolkit")

set(CMAKE_AUTOMOC ON)

if (USE_QT5)
  find_package (Qt5Widgets)
  set (LIB_LIST
     ${LIB_LIST}
     Qt5::Widgets)
else()
  find_package(Qt4 REQUIRED QtCore QtGui)
  include(${QT_USE_FILE})
endif()



if(WIN32) 
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/streamer.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/streamer.rc
            @ONLY)

    set (RC_SRC_LIST
      ${CMAKE_CURRENT_BINARY_DIR}/streamer.rc
    )
    
    set (LIB_LIST
        ${LIB_LIST}
        Qt5::WinMain)
    
    SET(GUI_TYPE WIN32)

    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/ueye_streamer.ico" DESTINATION "${CMAKE_BINARY_DIR}")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set (SRC_LIST
    ./src/camera.cpp
    ./src/cameralistmodel.cpp
    ./src/camerastream.cpp
    ./src/main.cpp
    ./src/mainwindow.cpp
    ./src/streamer.cpp
    ./src/streamwidget.cpp
    ./src/wizardpagecamera.cpp
    ./src/wizardpagecameraselect.cpp
    ./src/wizardpagefinish.cpp
    ./src/wizardpageintro.cpp
    ./src/wizardpagestream.cpp
    ./src/wizardstreamer.cpp
)

set (HDR_LIST
    ./src/camera.h
    ./src/cameralistmodel.h
    ./src/camerastream.h
    ./src/mainwindow.h
    ./src/streamer.h
    ./src/streamwidget.h
    ./src/wizardpagecamera.h
    ./src/wizardpagecameraselect.h
    ./src/wizardpagefinish.h
    ./src/wizardpageintro.h
    ./src/wizardpagestream.h
    ./src/wizardstreamer.h
)

include_directories(
    ${ADDITIONAL_INCLUDEDIR}
    )

link_directories(
    ${ADDITIONAL_LIBRARYDIR}
    )
    
set (LIB_LIST
  ${LIB_LIST}
  ueye_stream
  )

if (USE_QT5)  
  QT5_ADD_RESOURCES(RC_QT_LIST streamer.qrc)
else()
  QT4_ADD_RESOURCES(RC_QT_LIST streamer.qrc)
  set (LIB_LIST
     ${LIB_LIST}
     ${QT_LIBRARIES}
     )
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8 AND WIN32)
    set (LIB_LIST
        ${LIB_LIST}
        ueye_api_64
        )
else()
    set (LIB_LIST
        ${LIB_LIST}
        ueye_api
        )
endif()

if (UNIX)
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()


add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SRC_LIST} ${HDR_LIST} ${RC_QT_LIST} ${RC_SRC_LIST})

target_link_libraries(${PROJECT_NAME} ${LIB_LIST} )
