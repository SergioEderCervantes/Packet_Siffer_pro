#include "models/deviceModel.h"
#include <pcap/pcap.h>
#include <QDebug>
#include <sqlite3.h>

DeviceModel::DeviceModel(QObject *parent)
    : QObject{parent}
{
    fetchInterfaces();
    fetchTableNames();
}

void DeviceModel::fetchTableNames() {
    const QString DATABASE_NAME = "network_sniffer.db";

    sqlite3 *db;
    if (sqlite3_open(DATABASE_NAME.toUtf8().data(), &db) == SQLITE_OK) {
        const char *query = "SELECT name FROM sqlite_master WHERE type = 'table';";

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            qDebug() << "Error al preparar la consulta:" << sqlite3_errmsg(db);
            sqlite3_close(db);
            return;
        }

        // Limpiar el QVector antes de agregar los nuevos resultados
        tableNames.clear();

        // Ejecutar la consulta y recorrer los resultados
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char *tableName = sqlite3_column_text(stmt, 0);
            if (tableName && QString::fromUtf8(reinterpret_cast<const char *>(tableName)) != "sqlite_sequence") {
                tableNames.append(QString::fromUtf8(reinterpret_cast<const char *>(tableName)));
            }
        }

        // Finalizar el statement y cerrar la base de datos
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        qDebug() << "Tablas encontradas:" << tableNames;
    } else {
        qDebug() << "No se pudo abrir la base de datos:" << sqlite3_errmsg(db);
    }
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

QVector<QString> DeviceModel::getTableNames() const {
    return tableNames;
}
QVector<NetworkInterface> DeviceModel::getInterfaces() const {
    return interfaces;
}
