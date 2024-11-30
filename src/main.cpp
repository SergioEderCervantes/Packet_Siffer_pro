// ProyectoRedes2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// #include
// Includes generales
#include <pcap/pcap.h>
#include <cstring>
#include <QApplication>
#include "snifferwindow.h"
#include "pcapthread.h"

int main(int argc, char *argv[])
{
    // pcap_if_t *alldevs; // Lista de dispositivos
    // char errbuf[PCAP_ERRBUF_SIZE];

    // // Encuentra todos los dispositivos disponibles
    // if (pcap_findalldevs(&alldevs, errbuf) == -1)
    // {
    //     std::cerr << "Error al encontrar dispositivos: " << errbuf << std::endl;
    //     return 1;
    // }

    // // Itera sobre la lista de dispositivos e imprime información
    // std::cout << "Interfaces de red disponibles:\n";
    // int i = 1;

    // for (pcap_if_t *dev = alldevs; dev != nullptr; dev = dev->next)
    // {

    //     std::cout << i << ".-";
    //     if (dev->description)
    //     {
    //         std::cout << dev->description << std::endl;
    //     }
    //     else
    //     {
    //         std::cout << "  Descripción: No disponible, nombre de la interfaz: " << dev->name << std::endl;
    //     }
    //     i++;
    // }

    // int option = 4;
    // std::cout << "Dame el numero de la interfaz de red que quieres usar: ";
    // // std::cin >> option;

    // pcap_if_t *dev = alldevs;

    // // Recorre la lista hasta el dispositivo seleccionado
    // for (int j = 1; j < option; ++j)
    // {
    //     if (dev)
    //         dev = dev->next; // Mueve al siguiente dispositivo
    // }

    // std::cout << choosenDevName << std::endl;
    // std::cout << dev->description;
    // // Libera la lista de dispositivos
    // pcap_freealldevs(alldevs);




    //Aqui empieza la chota

    std::string choosenDevName("\\Device\\NPF_{9CBF4109-9C55-4615-BFEA-221D0D8A1A00}");

    QApplication app(argc, argv);
    SnifferWindow window;

    window.show();  // Muestra la ventana principal

    PcapThread hilo_paquetes(&window,choosenDevName);

    hilo_paquetes.start();

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
