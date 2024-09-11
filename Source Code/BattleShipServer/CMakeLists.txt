cmake_minimum_required(VERSION 3.14)

project(BattleShipServer LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

add_executable(BattleShipServer
  main.cpp
  server.h server.cpp
)
target_link_libraries(BattleShipServer Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

include(GNUInstallDirs)
install(TARGETS BattleShipServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
