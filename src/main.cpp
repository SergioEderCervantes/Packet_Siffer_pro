// ProyectoRedes2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// #include
// Includes generales

#include <cstdlib>
#include <pcap/pcap.h>
#include <cstring>
#include <QApplication>
#include "views/mainViewManager.h"

int main(int argc, char *argv[])
{
    // Aqui empieza la chota

    QApplication app(argc, argv);

    mainViewManager mainWindow;
    mainWindow.showMaximized();

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
