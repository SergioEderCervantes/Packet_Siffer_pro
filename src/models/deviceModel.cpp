#include "models/deviceModel.h"
#include <pcap/pcap.h>
#include <QDebug>
#include <sqlite3.h>
#include <QFile>

const std::string DeviceModel::DATABASE_NAME = "network_sniffer.db";

DeviceModel::DeviceModel(QObject *parent)
    : QObject{parent}
{
    fetchInterfaces();
    fetchTableNames();
}

void DeviceModel::fetchTableNames() {

    sqlite3 *db;
    int rc = sqlite3_open(DATABASE_NAME.c_str(),&db);

    if (rc == SQLITE_OK) {
        const char *query = "SELECT name FROM sqlite_master WHERE type = 'table';";

        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
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

void DeviceModel::onCreateCSV(const QString &filePath, const QString &selectedTableName){

    // Abrir el archivo CSV para escritura
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo para escritura:" << file.errorString();
        return;
    }

    QTextStream stream(&file);

    // Conexión a la base de datos SQLite
    sqlite3 *db;
    int rc = sqlite3_open(DATABASE_NAME.c_str(),&db);
    if (rc != SQLITE_OK) {
        qDebug() << "Error al abrir la base de datos SQLite:" << sqlite3_errmsg(db);
        file.close();
        return;
    }

    // Preparar la consulta SQL
    QString queryStr = QString("SELECT * FROM %1;").arg(selectedTableName);
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, queryStr.toUtf8().constData(), -1, &stmt, nullptr) != SQLITE_OK) {
        qDebug() << "Error al preparar la consulta:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        file.close();
        return;
    }

    // Obtener y escribir los nombres de las columnas
    int columnCount = sqlite3_column_count(stmt);
    for (int i = 0; i < columnCount; ++i) {
        stream << "\"" << sqlite3_column_name(stmt, i) << "\"";
        if (i < columnCount - 1) {
            stream << ",";
        }
    }
    stream << "\n";

    // Escribir los datos de las filas
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        for (int i = 0; i < columnCount; ++i) {
            QString columName = sqlite3_column_name(stmt, i);
            if (sqlite3_column_type(stmt, i) == SQLITE_BLOB && columName == "Raw") {
                // Obtener el BLOB como un QByteArray
                const void *blobData = sqlite3_column_blob(stmt, i);
                int blobSize = sqlite3_column_bytes(stmt, i);

                if (blobData && blobSize > 0) {
                    QByteArray blobArray(static_cast<const char*>(blobData), blobSize);
                    stream << "\"" << blobArray.toHex(' ') << "\"";
                } else {
                    stream << "\"\""; // Campo vacío si el BLOB está vacío
                }
            } else {
                // Para otros tipos (TEXT, INTEGER, etc.)
                const unsigned char *text = sqlite3_column_text(stmt, i);
                stream << "\"" << (text ? reinterpret_cast<const char*>(text) : "") << "\"";
            }

            if (i < columnCount - 1) {
                stream << ",";
            }
        }
        stream << "\n";
    }

    // Limpiar y cerrar
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    file.close();

    qDebug() << "Datos exportados exitosamente a:" << filePath;
}

QVector<QString> DeviceModel::getTableNames() const {
    return tableNames;
}
QVector<NetworkInterface> DeviceModel::getInterfaces() const {
    return interfaces;
}
