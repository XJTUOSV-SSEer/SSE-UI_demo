cmake_minimum_required(VERSION 3.16)

project(SSE-UI_demo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Sql Network REQUIRED )

add_executable(SSE-UI_demo

        main.cpp
        dbConnection.cpp
        dbConnection.h
        dbConnection.ui
        clientMain.h clientMain.cpp clientMain.ui
        waittingDialog.h waittingDialog.cpp waittingDialog.ui
        myQMsgBox.h
        socket_Client.cpp socket_Client.h socket_Client.ui
        utils.h utils.cpp

)

target_link_libraries(SSE-UI_demo PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Network)
# target_link_libraries(${PROJECT_NAME} PUBLIC $<$<BOOL:${MINGW}>:ws2_32>)
