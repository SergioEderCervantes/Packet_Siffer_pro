// ProyectoRedes2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// #include
// Includes generales
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <pcap/pcap.h>
#include <cstring>
#include <QApplication>

#ifndef _WIN32

// Includes de linux
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#else

// Includes y definiciones de windows
#include <winsock2.h>
#include <ws2tcpip.h>
#include "snifferwindow.h"
#include "pcapthread.h"
#pragma comment(lib, "ws2_32.lib") // Vincular la biblioteca de sockets de Windows

// Definir los flags de TCP
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20

#endif

// Dispositivos:
#ifdef _WIN32
// const char *device = "\\Device\\NPF_{9CBF4109-9C55-4615-BFEA-221D0D8A1A00}"; // WIFI
// const char *device = "\\Device\\NPF_{68CD2555-CB87-445E-97A6-93ECF9F06E66}";    //Ethernet de la maquina virtual

#else
// const char* device = "enp0s8";    //WIFI
const char *device = "enp0s3"; // Ethernet de la maquina virtual

#endif

int main(int argc, char *argv[])
{
    pcap_if_t *alldevs; // Lista de dispositivos
    char errbuf[PCAP_ERRBUF_SIZE];

    // Encuentra todos los dispositivos disponibles
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        std::cerr << "Error al encontrar dispositivos: " << errbuf << std::endl;
        return 1;
    }

    // Itera sobre la lista de dispositivos e imprime información
    std::cout << "Interfaces de red disponibles:\n";
    int i = 1;

    for (pcap_if_t *dev = alldevs; dev != nullptr; dev = dev->next)
    {

        std::cout << i << ".-";
        if (dev->description)
        {
            std::cout << dev->description << std::endl;
        }
        else
        {
            std::cout << "  Descripción: No disponible, nombre de la interfaz: " << dev->name << std::endl;
        }
        i++;
    }

    int option = 4;
    std::cout << "Dame el numero de la interfaz de red que quieres usar: ";
    // std::cin >> option;

    pcap_if_t *dev = alldevs;

    // Recorre la lista hasta el dispositivo seleccionado
    for (int j = 1; j < option; ++j)
    {
        if (dev)
            dev = dev->next; // Mueve al siguiente dispositivo
    }

    std::string choosenDevName = dev->name;
    std::cout << choosenDevName << std::endl;
    std::cout << dev->description;
    // Libera la lista de dispositivos
    pcap_freealldevs(alldevs);

#ifdef _WIN32
    WSADATA wsaData;
    // Inicializar WinSock
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
    {
        std::cerr << "Error al inicializar WinSock." << std::endl;
        return 1;
    }
#endif


    //Aqui empieza la chota

    QApplication app(argc, argv);
    SnifferWindow window;

    window.show();  // Muestra la ventana principal

    PcapThread hilo_paquetes(&window,choosenDevName);

    hilo_paquetes.start();


#ifdef _WIN32
    WSACleanup();
#endif
    return app.exec();  // Inicia el bucle de eventos de Qt
}





// Imprime el raw
// void print_packet(const u_char *packet_ptr, int length)
// {
//     std::cout << "Packet Data (Hexadecimal):\n";
//     for (int i = 0; i < length; i++)
//     {
//         std::cout << ("%02x ", packet_ptr[i]);
//         if ((i + 1) % 16 == 0)
//             std::cout << "\n"; // Salto a 16 bytes
//     }

//     std::cout << "\n";
// }
