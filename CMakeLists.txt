cmake_minimum_required(VERSION 3.16)

project(tuniwaste VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)

set(PROJECT_SOURCES
        main.cpp
        gestion_des_poubelles.cpp
        gestion_des_poubelles.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(tuniwaste
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(tuniwaste ${PROJECT_SOURCES})
endif()

target_link_libraries(tuniwaste PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(tuniwaste)
endif()
