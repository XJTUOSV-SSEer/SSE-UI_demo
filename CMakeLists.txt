cmake_minimum_required(VERSION 3.16)

project(SSE-UI_demo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets sql )
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets sql )

set(PROJECT_SOURCES
        main.cpp
        initwindow.cpp
        initwindow.h
        initwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SSE-UI_demo
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mymsg.h mymsg.cpp
        msgsocket.h msgsocket.cpp

    )


target_link_libraries(SSE-UI_demo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(${PROJECT_NAME} PUBLIC $<$<BOOL:${MINGW}>:ws2_32>)
