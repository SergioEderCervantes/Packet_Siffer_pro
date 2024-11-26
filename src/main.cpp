// ProyectoRedes2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//Includes generales
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <pcap/pcap.h>
#include <cstring>
#ifndef _WIN32

//Includes de linux
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>


#else

//Includes y definiciones de windows
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

struct ip
{
    unsigned char ip_hl : 4; // Longitud del encabezado (en palabras de 32 bits)
    unsigned char ip_v : 4;  // Versión del protocolo (IPv4 = 4)
    unsigned char ip_tos;    // Tipo de servicio
    unsigned short ip_len;   // Longitud total del paquete (encabezado + datos)
    unsigned short ip_id;    // Identificación del paquete
    unsigned short ip_off;   // Offset de fragmento y flags
    unsigned char ip_ttl;    // Tiempo de vida (TTL)
    unsigned char ip_p;      // Protocolo (por ejemplo, TCP = 6, UDP = 17)
    unsigned short ip_sum;   // Suma de verificación del encabezado
    struct in_addr ip_src;   // Dirección IP de origen
    struct in_addr ip_dst;   // Dirección IP de destino
};

#pragma pack(push, 1)
struct icmp
{
    uint8_t icmp_type;
    uint8_t icmp_code;
    uint16_t icmp_checksum;
    uint16_t icmp_id;
    uint16_t icmp_sequence;
};

struct tcphdr
{
    uint16_t th_sport;
    uint16_t th_dport;
    uint32_t th_seq;
    uint32_t th_ack_seq;
    uint16_t th_res1 : 4;
    uint16_t th_doff : 4;
    uint16_t th_flags : 8;
    uint16_t th_window;
    uint16_t th_check;
    uint16_t th_urg_ptr;
};

struct udphdr
{
    uint16_t uh_sport;
    uint16_t uh_dport;
    uint16_t uh_len;
    uint16_t uh_check;
};
#pragma pack(pop)
#endif
int link_hdr_lenght = 0;


// Dispositivos:
#ifdef _WIN32
const char* device = "\\Device\\NPF_{9CBF4109-9C55-4615-BFEA-221D0D8A1A00}";    //WIFI
// const char *device = "\\Device\\NPF_{68CD2555-CB87-445E-97A6-93ECF9F06E66}";    //Ethernet de la maquina virtual

#else
//const char* device = "enp0s8";    //WIFI
const char *device = "enp0s3";    //Ethernet de la maquina virtual

#endif

// Callback para manejar paquetes capturados
void call_me(u_char* user, const struct pcap_pkthdr* pkthdr, const u_char* packetd_ptr)
{
    packetd_ptr += 14;
    struct ip* ip_hdr = (struct ip*)packetd_ptr;

    // inet_ntoa() writes it's result to an address and returns this address,
    // but subsequent calls to inet_ntoa() will also write to the same address,
    // so we need to copy the result to a buffer.
    char packet_srcip[INET_ADDRSTRLEN]; // source ip address
    char packet_dstip[INET_ADDRSTRLEN]; // destination ip address
    strcpy(packet_srcip, inet_ntoa(ip_hdr->ip_src));
    strcpy(packet_dstip, inet_ntoa(ip_hdr->ip_dst));
    int packet_id = ntohs(ip_hdr->ip_id),   // identification
        packet_ttl = ip_hdr->ip_ttl,        // Time To Live
        packet_tos = ip_hdr->ip_tos,        // Type Of Service
        packet_len = ntohs(ip_hdr->ip_len), // header length + data length
        packet_hlen = ip_hdr->ip_hl;        // header length


    // Print it
    std::cout << "************************\n";
    std::cout << " | ID: " << packet_id 
        << " | SRC: " << packet_srcip
        << " | DST: " << packet_dstip
        << " | TOS: " << packet_tos
        << " | TTL: " << packet_ttl
        << " | " << std::endl;

    packetd_ptr += (4 * packet_hlen);
    int protocol_type = ip_hdr->ip_p;

    struct tcphdr* tcp_header;
    struct udphdr* udp_header;
    struct icmp* icmp_header;
    int src_port, dst_port;

    switch (protocol_type)
    {
    case IPPROTO_TCP: {
        tcp_header = (struct tcphdr*)packetd_ptr;
        src_port = tcp_header->th_sport;
        dst_port = tcp_header->th_dport;
        // Extracting SYN, ACK and URG flags
        std::cout << "PROTO: TCP | FLAGS: " 
            << (tcp_header->th_flags & TH_SYN ? 'S' : '-') << "/"
            << (tcp_header->th_flags & TH_ACK ? 'A' : '-') << "/"
            << (tcp_header->th_flags & TH_URG ? 'U' : '-') 
            << " | SPORT: " << src_port << " | DPORT: " << dst_port << " | " << std::endl;
        break;
    }
    case IPPROTO_UDP: {
        udp_header = (struct udphdr*)packetd_ptr;
        src_port = udp_header->uh_sport;
        dst_port = udp_header->uh_dport;
        std::cout << "PROTO: UDP | SPORT: " << src_port << " | DPORT: " << dst_port << " | " << std::endl;
        break;
    }
    case IPPROTO_ICMP:
    {
        icmp_header = (struct icmp*)packetd_ptr;
        // Get ICMP type and code
        int icmp_type = icmp_header->icmp_type;
        int icmp_type_code = icmp_header->icmp_code;
        std::cout << "PROTO: ICMP | TYPE: " << icmp_type << " | CODE: " << icmp_type_code << " | " << std::endl;
        break;
    }
    default:
        std::cout << "Protocolo desconocido, protocol_type:" << protocol_type << std::endl;
        break;
    }
}

// Imprime el raw
void print_packet(const u_char* packet_ptr, int length) {
    std::cout << "Packet Data (Hexadecimal):\n";
    for (int i = 0; i < length; i++) {
        std::cout << ("%02x ", packet_ptr[i]);
        if ((i + 1) % 16 == 0) std::cout << "\n"; // Salto a 16 bytes
    }

    std::cout << "\n";
}

int main(int argc, char const* argv[])
{
    #ifdef _WIN32
    WSADATA wsaData;
    // Inicializar WinSock
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        std::cerr << "Error al inicializar WinSock." << std::endl;
        return 1;
    }
    #endif

    /*
     * Declare the device name and error buffer size
     * PCAP_ERRBUF_SIZE is defined in pcap.h
     */
    char errbuf[PCAP_ERRBUF_SIZE];

    /*
     * BUFSIZ is defined in stdio.h, 0 to disable promiscuous mode and -1 to
     * disable timeout.
     */
    pcap_t* capdev = pcap_open_live(device, BUFSIZ, 0, -1, errbuf);
    /*
     * if capdev is null that means something went wrong, so we print the
     * error (which is stored in error_buffer) and exit the program.
     */
    if (capdev == NULL)
    {
        std::cout << "Error: pcap_open_live " << errbuf << std::endl;
        exit(1);
    }

    int link_hdr_type = pcap_datalink(capdev);
    switch (link_hdr_type)
    {
    case DLT_NULL:
        link_hdr_lenght = 4;
        break;
    case DLT_EN10MB:
        link_hdr_lenght = 14;
    default:
        link_hdr_lenght = 0;
        break;
    }

    // lets limit the capture to 5 packets.
    int packets_count = 0;
    /*
     * pcap_loop returns 0 upon success and -1 if it fails,
     * we listen to this return value and print an error if
     * pcap_loop failed
     */
    if (pcap_loop(capdev, packets_count, call_me, (u_char*)NULL))
    {
        std::cout << "ERROR: pcap_loop() failed!" << std::endl
            << errbuf << std::endl;
        exit(1);
    }

    pcap_close(capdev);
    #ifdef _WIN32
    WSACleanup();
    #endif

    return 0;
}
