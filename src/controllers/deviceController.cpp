#include "controllers/deviceController.h"
#include <QDebug>
#include "models/pcapThread.h"
DeviceController::DeviceController(DeviceModel *model, DeviceSelectionWindow *view, mainViewManager *MVM, QObject *parent)
    :QObject(parent), model(model), view(view), MVM(MVM){
    connect(view,&DeviceSelectionWindow::startCapture,this,&DeviceController::handleStartCapture);
}

void DeviceController::handleStartCapture(const QString &devName){
    qDebug() << "Iniciando Captura en: " << devName.toStdString();
    //Sacar el Sniffer Window
    SnifferWindow *target = this->MVM->getSnifferWindow();
    //Inicializar el Hilo de la captura de PCAP
    PcapThread *pcapThread = new PcapThread(devName.toUtf8().data(), this);
    // Conectar las se;ales
    connect(pcapThread, &PcapThread::packetCaptured, target, &SnifferWindow::addPacketToTable);
    connect(pcapThread, &PcapThread::finished, pcapThread, &PcapThread::deleteLater);
    //Iniciar el hilo
    pcapThread->start();

    //Cambiar de vista
    this->MVM->setCurrentView(target);

}
