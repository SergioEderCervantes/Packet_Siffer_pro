#include "models/pcapThread.h"
//Includes generales
#include <iostream>
#include <pcap/pcap.h>
#include <cstring>
#include <cstdlib>

//Includes de QT

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
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Vincular la biblioteca de sockets de Windows
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


PcapThread::PcapThread(char* name, QObject *parent) :QThread(parent)
{
    choosenDevName = strdup(name);  // Usar strdup para copiar la cadena
}
PcapThread::~PcapThread(){
    free(choosenDevName);
}
void PcapThread::run(){
    std::cout << std::endl <<  "Name: " << this->choosenDevName  << std::endl;

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *capdev = pcap_open_live(this->choosenDevName, BUFSIZ, 0, -1, errbuf);


    if (capdev == NULL)
    {
        std::cout << "Error: pcap_open_live " << errbuf << std::endl;
        exit(1);
    }

    // TODO: Tener en cuenta que el header_lenght esta en 14 pero deberiamos de ponerlo dinamicamente, de esta manera:
    // int link_hdr_type = pcap_datalink(capdev);
    // switch (link_hdr_type)
    // {
    // case DLT_NULL:
    //     link_hdr_lenght = 4;
    //     break;
    // case DLT_EN10MB:
    //     link_hdr_lenght = 14;
    // default:
    //     link_hdr_lenght = 0;
    //     break;
    // }

    // lets limit the capture to 5 packets.
    int packets_count = 0;
    /*
         * pcap_loop returns 0 upon success and -1 if it fails,
         * we listen to this return value and print an error if
         * pcap_loop failed
         */
    if (pcap_loop(capdev, packets_count, PcapThread::staticPacketHandler, reinterpret_cast<u_char *>(this)) < 0)
    {
        std::cout << "ERROR: pcap_loop() failed!" << std::endl
                  << errbuf << std::endl;
        exit(1);
    }

    pcap_close(capdev);
    emit finished();
}

//Callback intermediario entre pcap_loop y packetHandler
void PcapThread::staticPacketHandler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packetd_ptr) {
    // Convertir el puntero `user` a una instancia de `PcapThread`
    PcapThread *thread = reinterpret_cast<PcapThread *>(user);

    // Llamar al método no estático de la clase
    thread->packetHandler(user, pkthdr, packetd_ptr);
}

// Callback de pcap_loop
void PcapThread::packetHandler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packetd_ptr)
{

    // Procesar el paquete (extraer los datos relevantes)
    QStringList packetData;

    packetd_ptr += 14;  // TODO: aqui es donde deberia de ser dinamico
    struct ip *ip_hdr = (struct ip *)packetd_ptr;

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


    packetd_ptr += (4 * packet_hlen);
    int protocol_type = ip_hdr->ip_p;

    struct tcphdr *tcp_header;
    struct udphdr *udp_header;
    struct icmp *icmp_header;

    int src_port, dst_port;

    QString proto("-"),
        flags("-"),
        srcPort("-"),
        dstPort("-"),
        icmpType("-"),
        icmpTypeCode("-");

    switch (protocol_type)
    {
    case IPPROTO_TCP:
    {
        tcp_header = (struct tcphdr *)packetd_ptr;
        src_port = tcp_header->th_sport;
        dst_port = tcp_header->th_dport;

        proto = "TCP";
        flags = QString("%1/%2/%3")
                    .arg((tcp_header->th_flags & TH_SYN ? 'S' : '-'))  // SYN
                    .arg((tcp_header->th_flags & TH_ACK ? 'A' : '-'))  // ACK
                    .arg((tcp_header->th_flags & TH_URG ? 'U' : '-')); // URG
        srcPort = QString::number(src_port);
        dstPort = QString::number(dst_port);
        break;
    }
    case IPPROTO_UDP:
    {
        udp_header = (struct udphdr *)packetd_ptr;
        src_port = udp_header->uh_sport;
        dst_port = udp_header->uh_dport;
        proto = "UDP";
        srcPort = QString::number(src_port);
        dstPort = QString::number(dst_port);
        break;
    }
    case IPPROTO_ICMP:
    {
        icmp_header = (struct icmp *)packetd_ptr;
        // Get ICMP type and code
        proto = "ICMP";
        int icmp_type = icmp_header->icmp_type;
        int icmp_type_code = icmp_header->icmp_code;
        icmpType = QString::number(icmp_type);
        icmpTypeCode = QString::number(icmp_type_code);
        break;
    }
    default:
        proto = "Desconocido: ";
        proto.append(QString::number(protocol_type));
        break;
    }
    // Aquí debes extraer los datos del paquete, como IP de origen, destino, protocolo, etc.
    // Este es solo un ejemplo simplificado
    packetData << QString::number(packet_id);  // ID (ejemplo)
    packetData << QString::fromUtf8(packet_srcip);  // SrcIP (ejemplo)
    packetData << QString::fromUtf8(packet_dstip); // DstIP (ejemplo)
    packetData << QString::number(packet_tos);  // Tos (ejemplo)
    packetData << QString::number(packet_ttl);  // TTL (ejemplo)
    packetData << proto;  // Protocolo (ejemplo)
    packetData << flags;  // Flags (ejemplo)
    packetData << srcPort;  // SrcPort (ejemplo)
    packetData << dstPort;  // DstPort (ejemplo)
    packetData << icmpType;  // ICMPType (ejemplo)
    packetData << icmpTypeCode;  // ICMPTypeCode (ejemplo)



    // Emitir la señal para agregar el paquete a la GUI

    emit packetCaptured(packetData);
}
