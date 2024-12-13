#include "models/devicemodel.h"
#include <pcap/pcap.h>
#include <QDebug>

DeviceModel::DeviceModel(QObject *parent)
    : QObject{parent}
{
    fetchInterfaces();
}

void DeviceModel::fetchInterfaces(){
    pcap_if_t *alldevs; // Lista de dispositivos
    char errbuf[PCAP_ERRBUF_SIZE];

    // Encuentra todos los dispositivos disponibles
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        qWarning("Error al obtener interfaces: %s", errbuf);
        return;
    }


    for (pcap_if_t *dev = alldevs; dev != nullptr; dev = dev->next)
    {


        NetworkInterface netIf;
        netIf.description = dev->description ? QString(dev->description): "Sin descripcion";
        netIf.name = dev->name ? QString(dev->name) : "Nombre desconocido";
        this->interfaces.append(netIf);
    }

    pcap_freealldevs(alldevs);
}

QVector<NetworkInterface> DeviceModel::getInterfaces() const {
    return interfaces;
}
