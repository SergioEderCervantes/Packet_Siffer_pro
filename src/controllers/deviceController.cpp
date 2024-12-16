#include "controllers/deviceController.h"
#include <QDebug>
#include "views/mainViewManager.h"
#include "models/pcapThread.h"
DeviceController::DeviceController(DeviceModel *model, DeviceSelectionWindow *view, mainViewManager *MVM, QObject *parent)
    :QObject(parent), model(model), view(view), MVM(MVM){
    connect(view,&DeviceSelectionWindow::startCapture,this,&DeviceController::handleStartCapture);
}

void DeviceController::handleStartCapture(const QString &devName){
    qDebug() << "Iniciando Captura en: " << devName.toStdString();
    //Sacar el sniffer Window
    snifferWindow *target = this->MVM->getSnifferWindow();
    //Inicializar el Hilo de la captura de
    PcapThread *pcapThread = new PcapThread(devName.toUtf8().data(), this->MVM->getFilterType(), this);
    // Conectar las se;ales
    connect(pcapThread, &PcapThread::packetCaptured, target, &snifferWindow::addPacketToTable);
    connect(pcapThread, &PcapThread::finished, pcapThread, &PcapThread::deleteLater);
    //Conexion hilo killer
    connect(this->MVM, &mainViewManager::killThread, pcapThread, &PcapThread::handlerKiller);
    connect(this->MVM, &mainViewManager::clear,target, &snifferWindow::clearPacketTable);
    connect(pcapThread, &PcapThread::finished, this, [=]() {
        qDebug() << "Hilo terminado correctamente.";
        pcapThread->deleteLater();

    });

    //Iniciar el hilo
    pcapThread->start();

    //Cambiar de vista
    this->MVM->setCurrentView(target);

}
