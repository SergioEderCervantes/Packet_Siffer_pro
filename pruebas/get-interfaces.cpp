#include <pcap/pcap.h>
#include <iostream>
#include <string>

int main() {
    pcap_if_t *alldevs;   // Lista de dispositivos
    char errbuf[PCAP_ERRBUF_SIZE]; // Buffer para errores

    // Encuentra todos los dispositivos disponibles
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error al encontrar dispositivos: " << errbuf << std::endl;
        return 1;
    }

    // Itera sobre la lista de dispositivos e imprime información
    std::cout << "Interfaces de red disponibles:\n";
    for (pcap_if_t *dev = alldevs; dev != nullptr; dev = dev->next) {
        std::cout << "- Nombre: " << dev->name << std::endl;
        if (dev->description) {
            std::cout << "  Descripción: " << dev->description << std::endl;
        } else {
            std::cout << "  Descripción: No disponible" << std::endl;
        }
    }

    // Libera la lista de dispositivos
    pcap_freealldevs(alldevs);

    return 0;
}
