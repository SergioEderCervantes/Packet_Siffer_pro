cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto y estándar de C++
project(Paquet_sniffer_pro VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Configuración multiplataforma
if(WIN32)
    message(STATUS "Compilación para Windows")
    add_definitions(-DWIN32)
    include_directories(
            ${CMAKE_SOURCE_DIR}/include
            "C:\\ncap\\Include")
    link_directories("C:\\ncap\\Lib\\x64")
    set(PCAP_LIBS ws2_32 wpcap)
elseif(UNIX)
    message(STATUS "Compilación para Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PCAP REQUIRED libpcap)
    include_directories(${PCAP_INCLUDE_DIRS})
    link_directories(${PCAP_LIBRARY_DIRS})
    set(PCAP_LIBS ${PCAP_LIBRARIES})
endif()

# Necesitas encontrar y añadir las librerías de Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets LinguistTools)

# Fuentes del proyecto
set(PROJECT_SOURCES
    src/main.cpp
    src/controllers/pcapThread.cpp
    src/views/snifferWindow.cpp
    src/views/deviceSelectionWindow.cpp
    src/models/deviceModel.cpp
    src/controllers/deviceController.cpp
    src/views/snifferWindow.ui
    src/views/mainViewManager.cpp
)

# Archivos de cabecera
include_directories(include)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Packet_sniffer_pro
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        include/controllers/pcapThread.h
        include/views/snifferWindow.h
        include/views/deviceSelectionWindow.h
        include/models/deviceModel.h
        include/controllers/deviceController.h
        include/views/mainViewManager.h
        src/views/MainWindow.ui
    )
else()
    add_executable(Packet_sniffer_pro
        ${PROJECT_SOURCES}
        include/controllers/pcapThread.h
        include/views/snifferWindow.h
        include/views/deviceSelectionWindow.h
        include/models/deviceModel.h
        include/controllers/deviceController.h
        include/views/MainViewManager.h
    )
endif()

# Vincular librerías
target_link_libraries(Packet_sniffer_pro PRIVATE ${PCAP_LIBS} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

