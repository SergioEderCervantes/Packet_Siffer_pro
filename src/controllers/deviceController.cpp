#include "controllers/deviceController.h"
#include <QDebug>
#include "models/pcapThread.h"
#include "models/SQLiteThread.h"
DeviceController::DeviceController(DeviceModel *model, DeviceSelectionWindow *view, mainViewManager *MVM, QObject *parent)
    :QObject(parent), model(model), view(view), MVM(MVM){
    connect(view,&DeviceSelectionWindow::startCapture,this,&DeviceController::handleStartCapture);
}

void DeviceController::handleStartCapture(const QString &devName){
    qDebug() << "Iniciando Captura en: " << devName.toStdString();
    //Sacar el sniffer Window
    snifferWindow *target = this->MVM->getSnifferWindow();
    //Inicializar el Hilo de la captura de PCAP
    PcapThread *pcapThread = new PcapThread(devName.toUtf8().data(), this);
    // Inicializar el Hilo de la base de datos
    SQLiteThread *sqliteThread = new SQLiteThread(devName, this); // TODO: Cambiar esto dinamicamente

    // Conectar las se;ales
    connect(pcapThread, &PcapThread::packetCaptured, target, &snifferWindow::addPacketToTable);
    connect(pcapThread, &PcapThread::finished, pcapThread, &PcapThread::deleteLater);
    connect(pcapThread, &PcapThread::sendPacketToDB,sqliteThread, &SQLiteThread::savePacket);
    connect(sqliteThread, &SQLiteThread::finished, sqliteThread, &SQLiteThread::deleteLater);


    //Iniciar los hilos
    pcapThread->start();
    sqliteThread->start();

    //Cambiar de vista
    this->MVM->setCurrentView(target);

}
