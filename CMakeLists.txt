cmake_minimum_required(VERSION 3.10)

#Nombre de proyecto y estandar de C++
project(Packet_sniffer_pro LANGUAGES CXX)
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
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Archivos Fuente
set(SOURCES src/main.cpp)

# Archivos de Cabecera (cuando haya)
# set(HEADERS include/utils.h)

# Crear el ejecutable 
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Vincular las bibliotecas necesarias
target_link_libraries(${PROJECT_NAME} ${PCAP_LIBS} Qt6::Core Qt6::Gui Qt6::Widgets)
