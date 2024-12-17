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
    qDebug("Entrando al destructor");
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
        "PacketId TEXT,"
        "SrcIP TEXT, "
        "DstIP TEXT,"
        "TOS TEXT, "
        "TTL TEXT,"
        "Protocolo TEXT,"
        "Flags TEXT,"
        "SrcPort TEXT,"
        "DstPort TEXT,"
        "ICMPType TEXT,"
        "ICMPTypeCode TEXT,"
        "Raw BLOB);"
                                   ).arg(this->tableName);

    rc = sqlite3_exec(db,createTableQuery.toUtf8().data(),nullptr,nullptr,nullptr);
    if (rc != SQLITE_OK){
        qDebug() << "Error en la creacion de la tabla: " << sqlite3_errmsg(db);
        return;
    }
    exec();
}

void SQLiteThread::savePacket(const QStringList &packedData, const QByteArray &rawData){
    if (!db) {
        qDebug() << "La base de datos no está inicializada.";
        return;
    }

    // Iniciar una transacción para mejorar el rendimiento
    char *errMsg = nullptr;
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);

    if (errMsg) {
        qDebug() << "Error al iniciar la transacción: " << errMsg;
        sqlite3_free(errMsg);
        return;
    }

    QStringList columns = {"PacketId", "SrcIP", "DstIP", "TOS", "TTL", "Protocolo", "Flags", "SrcPort",
                           "DstPort", "ICMPType", "ICMPTypeCode", "Raw"};

    QString insertQuery = QString("INSERT INTO %1 (%2) VALUES (").arg(tableName).arg(columns.join(", "));
    for (int i = 0; i < packedData.size(); ++i) {
        insertQuery += (i == 0 ? "?" : ", ?");
    }
    insertQuery += ", ?);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, insertQuery.toUtf8().data(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "Error al preparar el statement: " << sqlite3_errmsg(db);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    // Vincular valores
    for (int i = 0; i < packedData.size(); ++i) {
        sqlite3_bind_text(stmt, i + 1, packedData[i].toUtf8().data(), -1, SQLITE_STATIC);
    }
    sqlite3_bind_blob(stmt, packedData.size() + 1, rawData.data(), rawData.size(), SQLITE_STATIC);

    // Ejecutar la consulta
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        qDebug() << "Error al ejecutar la consulta: " << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    // Finalizar el statement
    sqlite3_finalize(stmt);

    // Finalizar la transacción
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
    if (errMsg) {
        qDebug() << "Error al finalizar la transacción: " << errMsg;
        sqlite3_free(errMsg);
    }
}


void SQLiteThread::onFetchRowData(int row){
    row ++;
    QString query = QString("SELECT * FROM %1 WHERE id = ?;").arg(tableName);
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, query.toUtf8().data(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        qDebug() << "Error preparando la consulta: " << sqlite3_errmsg(db);
        return;
    }

    // Vincular el parámetro (ID del paquete)
    rc = sqlite3_bind_int(stmt, 1, row);
    if (rc != SQLITE_OK) {
        qDebug() << "Error vinculando el parámetro ID:" << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return;
    }

    QStringList packetData;
    QByteArray rawData;
    // Ejecutar el statement
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Limpiar cualquier dato previo en packetData
        packetData.clear();

        // Extraer las columnas 2 a 12 como QStrings y agregar a packetData
        for (int col = 1; col <= 11; ++col) { // Columnas 2 a 12 son de índice 1 a 11
            const unsigned char *text = sqlite3_column_text(stmt, col);
            packetData.append(text ? reinterpret_cast<const char *>(text) : "");
        }

        // Extraer la columna 13 (raw) como un QByteArray
        const void *blobData = sqlite3_column_blob(stmt, 12);
        int blobSize = sqlite3_column_bytes(stmt, 12);

        if (blobData && blobSize > 0) {
            rawData = QByteArray(reinterpret_cast<const char *>(blobData), blobSize);
        } else {
            rawData.clear();
            qDebug() << "El campo raw está vacío o no existe.";
        }
    } else {
        qDebug() << "No se encontró ningún resultado para el ID:" << row;
        sqlite3_finalize(stmt);
        return;
    }

    // Finalizar el statement
    sqlite3_finalize(stmt);

    emit rowDataResponse(packetData,rawData);
}
void SQLiteThread::onExec_query(QString &query, QTextEdit *resultView) {
    if (!db) return;

    // Reemplazar "actual" por el valor real de tableName
    query.replace("actual", tableName);
    qDebug() << "Ejecutando query: " << query;

    sqlite3_stmt *stmt = nullptr;

    // Preparar la consulta
    int rc = sqlite3_prepare_v2(db, query.toUtf8().data(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        QString errorMsg = QString("Error preparando la consulta: %1").arg(sqlite3_errmsg(db));
        qDebug() << errorMsg;
        resultView->setText(errorMsg);
        return;
    }

    // Construir una tabla ASCII para mostrar en resultView
    QString resultText;
    int columnCount = sqlite3_column_count(stmt);

    // Encabezados de la tabla
    QStringList headers;
    for (int i = 0; i < columnCount; ++i) {
        headers << QString::fromUtf8(sqlite3_column_name(stmt, i));
    }
    resultText += headers.join(" | ") + "\n";
    resultText += QString("-").repeated(resultText.length()) + "\n";

    // Iterar por las filas de la consulta
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QStringList rowValues;
        for (int i = 0; i < columnCount; ++i) {
            const unsigned char *value = sqlite3_column_text(stmt, i);
            rowValues << (value ? QString::fromUtf8(reinterpret_cast<const char*>(value)) : "NULL");
        }
        resultText += rowValues.join(" | ") + "\n";
    }

    // Finalizar la consulta
    sqlite3_finalize(stmt);

    // Mostrar resultados en el resultView
    if (resultText.isEmpty()) {
        resultText = "La consulta no devolvió ningún resultado.";
    }
    resultView->setText(resultText);

    qDebug() << "Query ejecutada correctamente.";
}

void SQLiteThread::onDeleteTable(){
    if (!db) return;
    QString query = QString("DROP TABLE IF EXISTS %1;").arg(tableName);
    int rc = sqlite3_exec(db,query.toUtf8().data(),nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK){
        qDebug() << "Error al borrar la tabla de la base de datos: " << sqlite3_errmsg(db);
    } else{
        qDebug() << "La tabla fue borrada con exito";
    }
}

void SQLiteThread::handleKiller(){
    qDebug() << "Cerrando hilo SQLiteThread de manera segura...";

    //Cerrar la base de datos si esta abierta
    if (db){
        sqlite3_close(db);
        db = nullptr;
        qDebug() << "Conexion de la base de datos cerrada";
    }

    quit();
}
