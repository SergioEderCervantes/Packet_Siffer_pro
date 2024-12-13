// ProyectoRedes2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// #include
// Includes generales

#include <cstdlib>
#include <pcap/pcap.h>
#include <cstring>
#include <QApplication>
// #include "views/snifferwindow.h"
// #include "controllers/pcapthread.h"
#include "models/devicemodel.h"
#include "controllers/devicecontroller.h"
#include "views/deviceselectionwindow.h"

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

#pragma comment(lib, "ws2_32.lib") // Vincular la biblioteca de sockets de Windows

// Definir los flags de TCP
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20

#endif


int main(int argc, char *argv[])
{
// #ifdef _WIN32
//     WSADATA wsaData;
//     // Inicializar WinSock
//     if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
//     {
//         // std::cerr << "Error al inicializar WinSock." << std::endl;
//         return 1;
//     }
// #endif


    //Aqui empieza la chota

    QApplication app(argc, argv);

    DeviceModel devModel;
    DeviceSelectionWindow devWindow(&devModel);
    DeviceController devController(&devModel, &devWindow);

    devWindow.show();



    // SnifferWindow window;

    // window.show();  // Muestra la ventana principal

    // PcapThread hilo_paquetes(&window,choosenDevName);

    // hilo_paquetes.start();


// #ifdef _WIN32
//     WSACleanup();
// #endif
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
