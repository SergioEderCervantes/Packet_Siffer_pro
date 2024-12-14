#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <QObject>
#include "models/deviceModel.h"
#include "views/DeviceSelectionWindow.h"
#include "views/mainViewManager.h"
class DeviceController : public QObject {
    Q_OBJECT

public:
    explicit DeviceController(DeviceModel *model, DeviceSelectionWindow *view, mainViewManager *MVM, QObject *parent = nullptr);

private slots:
    void handleStartCapture(const QString &deviceName);

private:
    DeviceModel *model;
    DeviceSelectionWindow *view;
    mainViewManager *MVM;
};


#endif // DEVICECONTROLLER_H
