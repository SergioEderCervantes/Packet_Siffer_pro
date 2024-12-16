#ifndef SQLITETHREAD_H
#define SQLITETHREAD_H

#include <QThread>
#include <QStringList>
#include <sqlite3.h>
#include <string>

class SQLiteThread : public QThread{
    Q_OBJECT

public:
    explicit SQLiteThread(QString interfaceName, QObject *parent = nullptr);
    ~SQLiteThread();
    void printStoredData();

    void run() override;
signals:
    void rowDataResponse(QStringList &packetdata,QByteArray &rawData);
    void finished();

public slots:
    void savePacket(const QStringList &packetData, const QByteArray &rawData);
    void onDeleteTable();
    void onFetchRowData(int row);
    void handleKiller();
private:
    static const std::string DATABASE_NAME;
    sqlite3 *db;
    const QString tableName;

    QString generateTableName(const QString &interfaceName);
    void retrieveBlobWithStatement(int packetId);
};

#endif // SQLITETHREAD_H
