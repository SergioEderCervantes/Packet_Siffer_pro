#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QObject>
#include <QString>
#include <QVector>

struct NetworkInterface {
    QString name;
    QString description;
};

class DeviceModel : public QObject
{
    Q_OBJECT
public:
    explicit DeviceModel(QObject *parent = nullptr);
    QVector<NetworkInterface> getInterfaces() const;
    QVector<QString> getTableNames() const;
private:
    static const std::string DATABASE_NAME;
    QVector<NetworkInterface> interfaces;
    QVector<QString> tableNames;

    void fetchInterfaces();
    void fetchTableNames();
public slots:

    void onCreateCSV(const QString &filePath, const QString &selectedTableName);
};

#endif // DEVICEMODEL_H
