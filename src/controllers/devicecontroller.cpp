#include "controllers/devicecontroller.h"
#include <QDebug>
DeviceController::DeviceController(DeviceModel *model, DeviceSelectionWindow *view, QObject *parent)
    :QObject(parent), model(model), view(view){
    connect(view,&DeviceSelectionWindow::startCapture,this,&DeviceController::handleStartCapture);
}

void DeviceController::handleStartCapture(const QString &devName){
    qDebug() << "Iniciando Captura en: " << devName;
}
