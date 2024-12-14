#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <QObject>
#include "models/deviceModel.h"
#include "views/DeviceSelectionWindow.h"

class DeviceController : public QObject {
    Q_OBJECT

public:
    explicit DeviceController(DeviceModel *model, DeviceSelectionWindow *view, QObject *parent = nullptr);

private slots:
    void handleStartCapture(const QString &deviceName);

private:
    DeviceModel *model;
    DeviceSelectionWindow *view;
};


#endif // DEVICECONTROLLER_H
