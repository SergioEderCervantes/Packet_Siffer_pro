#ifndef SQLITETHREAD_H
#define SQLITETHREAD_H

#include <QThread>
#include <QStringList>
#include <sqlite3.h>
#include <string>
#include <QTextEdit>
class SQLiteThread : public QThread{
    Q_OBJECT

public:
    explicit SQLiteThread(QString interfaceName, QObject *parent = nullptr);
    ~SQLiteThread();

    void run() override;
signals:
    void rowDataResponse(QStringList &packetdata,QByteArray &rawData);
    void finished();

public slots:
    void savePacket(const QStringList &packetData, const QByteArray &rawData);
    void onDeleteTable();
    void onFetchRowData(int row);
    void handleKiller();
    void onExec_query(QString &query, QTextEdit *resultView);
private:
    static const std::string DATABASE_NAME;
    sqlite3 *db;
    const QString tableName;

    QString generateTableName(const QString &interfaceName);
    void retrieveBlobWithStatement(int packetId);
};

#endif // SQLITETHREAD_H
