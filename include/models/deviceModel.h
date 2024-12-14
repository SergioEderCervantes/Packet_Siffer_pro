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

private:
    QVector<NetworkInterface> interfaces;

    void fetchInterfaces();
};

#endif // DEVICEMODEL_H
