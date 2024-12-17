// ProyectoRedes2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// #include
// Includes generales

#include <cstdlib>
#include <pcap/pcap.h>
#include <cstring>
#include <QApplication>
#include "views/mainViewManager.h"
#include <sqlite3.h>


// Callback para manejar los resultados de la consulta
int callback(void *data, int argc, char **argv, char **azColName) {
    QStringList *tableNames = static_cast<QStringList *>(data);

    for (int i = 0; i < argc; i++) {
        if (argv[i]) {
            tableNames->append(QString::fromUtf8(argv[i]));
        }
    }

    return 0;
}

// Función para listar las tablas
QStringList listTables(sqlite3 *db) {
    QStringList tableNames;
    const char *query = "SELECT name FROM sqlite_master WHERE type = 'table';";

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query, callback, &tableNames, &errMsg);

    if (rc != SQLITE_OK) {
        qDebug() << "Error ejecutando la consulta:" << errMsg;
        sqlite3_free(errMsg);
    }

    return tableNames;
}

int main(int argc, char *argv[])
{
    // Aqui empieza la chota

    QApplication app(argc, argv);

    mainViewManager mainWindow;
    mainWindow.showMaximized();

    return app.exec();  // Inicia el bucle de eventos de Qt


    sqlite3 *db;
    if (sqlite3_open("network_sniffer.db", &db) == SQLITE_OK) {
        QStringList tables = listTables(db);
        if (!tables.isEmpty()) {
            qDebug() << "Tablas en la base de datos:";
            for (const QString &table : tables) {
                qDebug() << table;
            }
        } else {
            qDebug() << "No se encontraron tablas en la base de datos.";
        }
        sqlite3_close(db);
    } else {
        qDebug() << "No se pudo abrir la base de datos.";
    }
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
