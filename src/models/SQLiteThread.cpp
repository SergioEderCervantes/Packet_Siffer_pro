#include "models/SQLiteThread.h"
#include <QDebug>
#include <QDate>
#include <QUuid>
#include <QRegularExpression>


const std::string SQLiteThread::DATABASE_NAME = "network_sniffer.db";


SQLiteThread::SQLiteThread(QString interfaceName, QObject *parent) : tableName(generateTableName(interfaceName)), QThread(parent){}


QString SQLiteThread::generateTableName(const QString &interfaceName){
    // Obtener la fecha actual en formato ddMMyyyy
    QString fechaActual = QDate::currentDate().toString("dd_MM_yyyy");

    // Extraer la parte relevante del nombre técnico (UUID u otra parte única)
    QRegularExpression regex("\\{(.*?)\\}");  // Buscar contenido entre llaves
    QRegularExpressionMatch match = regex.match(interfaceName);
    QString nomRelevante = match.hasMatch() ? match.captured(1) : "UnknownDevice";

    // Eliminar los guiones del nombre relevante
    nomRelevante.replace("-", "");  // También podrías usar "_"

    // Generar un UUID único
    QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);

    // Formar el nombre de la tabla
    return QString("Date_%1_Device_%2_%3").arg(fechaActual, nomRelevante , uuid);
}

SQLiteThread::~SQLiteThread(){
    if(this->db){
        sqlite3_close(db);
    }
}

void SQLiteThread::run(){
    // HILO ESPECIAL PARA MANEJAR LA CAPTURA Y CONSULTA DE PAQUETES ENTRANTES EN LA VISTA DE CAPTURA,
    // ESTE HILO NO ESTA DISEÑADO PARA SER USADO EN LA VISTA DE CONSULTAS
    //Abrir la base de datos
    int rc = sqlite3_open(DATABASE_NAME.c_str(),&db);
    if (rc != SQLITE_OK){
        qDebug() << "No se pudo abrir la base de datos: " << sqlite3_errmsg(db);
        return;
    }

    // Creamos la tabla, porque se crea apenas
    QString createTableQuery = QString(
        "CREATE TABLE IF NOT EXISTS %1 ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "packet_id TEXT,"
        "srcIP TEXT, "
        "dstIP TEXT,"
        "tos TEXT, "
        "ttl TEXT,"
        "protocolo TEXT,"
        "flags TEXT,"
        "srcPort TEXT,"
        "dstPort TEXT,"
        "ICMPType TEXT,"
        "ICMPTypeCode TEXT,"
        "raw BLOB);"
                                   ).arg(this->tableName);

    rc = sqlite3_exec(db,createTableQuery.toUtf8().data(),nullptr,nullptr,nullptr);
    if (rc != SQLITE_OK){
        qDebug() << "Error en la creacion de la tabla: " << sqlite3_errmsg(db);
        return;
    }
    exec();
}

void SQLiteThread::savePacket(const QStringList &packedData, const QByteArray &rawData){
    if (!db) return;
    // Imprimir en formato hexadecimal
    qDebug() << "Raw (hex):" << rawData.toHex(' ');

    // Imprimir cada byte como valor entero para mayor claridad
    QString binaryRepresentation;
    for (char byte : rawData) {
        binaryRepresentation += QString::asprintf("%02X ", static_cast<unsigned char>(byte));
    }
    qDebug() << "Raw (binario):" << binaryRepresentation;

    QStringList columns = {"packet_id", "srcIP", "dstIP", "tos", "ttl", "protocolo", "flags", "srcPort",
                           "dstPort", "ICMPType", "ICMPTypeCode", "raw"};


    // Preparar la Query
    QString insertQuery = QString("INSERT INTO %1 (%2) VALUES (").arg(tableName).arg(columns.join(", "));

    // Crea el espacio para los values de packedData
    for (int i = 0; i < packedData.size(); ++i) {
        insertQuery += (i == 0 ? "?" : ", ?");
    }
    // Crea el espacio para el value de raw y cierra la query
    insertQuery += ", ?);";
    // Crear el Statement
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db,insertQuery.toUtf8().data(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK){
        qDebug() << "ERROR AL PREPARAR EL STMT: " << sqlite3_errmsg(db);
        return;
    }

    // Vincular cada elemento del QStringList al statement
    for (int i = 0; i < packedData.size(); ++i) {
        sqlite3_bind_text(stmt, i + 1, packedData[i].toUtf8().data(), -1, SQLITE_TRANSIENT);
    }

    // Vincula el raw
    sqlite3_bind_blob(stmt,packedData.size() + 1, rawData.data(), rawData.size(),SQLITE_TRANSIENT);

    // Ejecutar la consulta
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE){
        qDebug() << "ERROR AL EJECUTAR LA CONSULTA: " << sqlite3_errmsg(db);
        return;
    }

    // Finalizar el Statement
    sqlite3_finalize(stmt);

}


void SQLiteThread::printStoredData() {
    QString selectQuery = QString("SELECT * FROM %1;").arg(tableName);

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, selectQuery.toUtf8().data(), callback, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        qDebug() << "Error ejecutando SELECT:" << errMsg;
        sqlite3_free(errMsg);
    }
}

int SQLiteThread::callback(void *unused, int argc, char **argv, char**colNames){
    Q_UNUSED(unused);
    QStringList row;
    for (int i = 0; i < argc; i++) {
        QString col = colNames[i] ? QString(colNames[i]) : "NULL";
        QString value = argv[i] ? QString(argv[i]) : "NULL";
        row << QString("%1: %2").arg(col).arg(value);
    }
    qDebug() << row.join(" | ");
    return 0;
}


void SQLiteThread::retrieveBlobWithStatement(int packetId) {
    // Crear la consulta con un marcador de parámetro
    QString query = QString("SELECT raw FROM %1 WHERE id = ?;").arg(tableName);

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query.toUtf8().data(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "Error preparando la consulta: " << sqlite3_errmsg(db);
        return;
    }

    // Vincular el parámetro (ID del paquete)
    sqlite3_bind_int(stmt, 1, packetId);

    // Ejecutar el statement y procesar los resultados
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Obtener el BLOB y su tamaño
        const void *blobData = sqlite3_column_blob(stmt, 0);
        int blobSize = sqlite3_column_bytes(stmt, 0);

        if (blobData && blobSize > 0) {
            // Convertir el BLOB en un QByteArray
            QByteArray rawData(reinterpret_cast<const char *>(blobData), blobSize);

            // Imprimir el BLOB en formato hexadecimal
            qDebug() << "Raw recuperado (hex): " << rawData.toHex(' ').toStdString();

            // (Opcional) Mostrar como ASCII legible
            QString rawAscii;
            for (char c : rawData) {
                rawAscii += (c >= 32 && c <= 126) ? c : '.';
            }
            qDebug() << "Raw (ASCII): " << rawAscii.toStdString();
        } else {
            qDebug() << "El BLOB está vacío o no existe.";
        }
    } else {
        qDebug() << "No se encontró ningún resultado para el ID del paquete: " << packetId;
    }

    // Finalizar el statement
    sqlite3_finalize(stmt);
}
