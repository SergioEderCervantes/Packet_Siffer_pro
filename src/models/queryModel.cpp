#include "models/queryModel.h"
#include <sqlite3.h>
#include <QDebug>
queryModel::queryModel(QObject *parent) :QObject(parent){
    fetchTableNames();
}


void queryModel::fetchTableNames() {
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

QVector<QString> queryModel::getTableNames() const{
    return this->tableNames;
}

void queryModel::onExecuteQuery(const QString &query) {
    const QString DATABASE_NAME = "network_sniffer.db";
    sqlite3 *db;

    if (sqlite3_open(DATABASE_NAME.toUtf8().data(), &db) != SQLITE_OK) {
        emit queryError(QString("Error al abrir la base de datos: %1").arg(sqlite3_errmsg(db)));
        return;
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query.toUtf8().data(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        emit queryError(QString("Error en la consulta: %1").arg(sqlite3_errmsg(db)));
        sqlite3_close(db);
        return;
    }

    // Procesar columnas (encabezados)
    QStringList headers;
    int columnCount = sqlite3_column_count(stmt);
    for (int i = 0; i < columnCount; ++i) {
        headers << QString(sqlite3_column_name(stmt, i));
    }

    // Procesar las filas de la consulta
    QVector<QVector<QString>> data;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QVector<QString> row;
        for (int i = 0; i < columnCount; ++i) {
            int columnType = sqlite3_column_type(stmt, i);
            QString columnName = sqlite3_column_name(stmt, i);

            // Procesar la columna 'raw' como QByteArray y convertirla a Hex
            if (columnName == "Raw" && columnType == SQLITE_BLOB) {
                const void *blobData = sqlite3_column_blob(stmt, i);
                int blobSize = sqlite3_column_bytes(stmt, i);
                QByteArray byteArray = QByteArray(static_cast<const char *>(blobData), blobSize);
                row.append(byteArray.toHex(' ')); // Convertir a formato hexadecimal
            } else {
                // Procesar otras columnas como texto normal
                const char *value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
                row.append(value ? QString::fromUtf8(value) : "NULL");
            }
        }
        data.append(row);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Emitir señal con el resultado
    emit queryResultReady(data, headers);
}

