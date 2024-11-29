cmake_minimum_required(VERSION 3.10)

#Nombre de proyecto y estandar de C++
project(Paquet_sniffer_pro VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#Config multiplataforma
if(WIN32)
    message(STATUS "Compilacion para windows")
    add_definitions(-DWIN32)
    include_directories("C:\\Develop\\npcap-sdk-1.13\\Include")
    link_directories("C:\\Develop\\npcap-sdk-1.13\\Lib\\x64")
    set(PCAP_LIBS ws2_32 wpcap)
elseif(UNIX)
    message(STATUS "Compulacion para Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PCAP REQUIRED libpcap)
    include_directories(${PCAP_INCLUDE_DIRS})
    link_directories(${PCAP_LIBRARY_DIRS})
    set(PCAP_LIBS ${PCAP_LIBRARIES})
endif()

# Necesitas encontrar y añadir las librerías de Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets LinguistTools)



set(PROJECT_SOURCES
        main.cpp
        snifferwindow.cpp
        snifferwindow.h
        snifferwindow.ui
)
# Archivos de Cabecera (cuando haya)
# set(HEADERS include/utils.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Packet_sniffer_pro
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        pcapthread.h pcapthread.cpp
    )

else()
    add_executable(Packet_sniffer_pro
        ${PROJECT_SOURCES}
    )

endif()

target_link_libraries(Packet_sniffer_pro PRIVATE ${PCAP_LIBS} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)
