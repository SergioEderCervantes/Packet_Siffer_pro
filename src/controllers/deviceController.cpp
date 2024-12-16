#include "controllers/deviceController.h"
#include <QDebug>
#include "views/mainViewManager.h"
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
    //Inicializar el Hilo de la captura de
    PcapThread *pcapThread = new PcapThread(devName.toUtf8().data(), this->MVM->getFilterType(), this);
    // Inicializar el Hilo de la base de datos
    SQLiteThread *sqliteThread = new SQLiteThread(devName, this); // TODO: Cambiar esto dinamicamente

    // Conectar las se;ales
    connect(pcapThread, &PcapThread::packetCaptured, target, &snifferWindow::addPacketToTable);
    connect(pcapThread, &PcapThread::sendPacketToDB,sqliteThread, &SQLiteThread::savePacket);
    connect(target, &snifferWindow::fetchRowData, sqliteThread, &SQLiteThread::onFetchRowData);
    connect(sqliteThread, &SQLiteThread::rowDataResponse, target, &snifferWindow::onRowDataResponse);
    connect(sqliteThread, &SQLiteThread::finished, sqliteThread, &SQLiteThread::deleteLater);
    connect(this->MVM, &mainViewManager::deleteTable, sqliteThread, &SQLiteThread::onDeleteTable);

    //Conexion hilo killer
    connect(this->MVM, &mainViewManager::killThread, pcapThread, &PcapThread::handlerKiller);
    connect(this->MVM, &mainViewManager::clear,target, &snifferWindow::clearPacketTable);
    connect(pcapThread, &PcapThread::finished, this, [=]() {
        qDebug() << "Hilo terminado correctamente.";
        pcapThread->deleteLater();

    });
    connect(this->MVM, &mainViewManager::killThread, sqliteThread, &SQLiteThread::handleKiller);
    connect(sqliteThread, &SQLiteThread::finished,this, [=] () {
        qDebug("Hilo de la BD terminado correctamente");
        sqliteThread->deleteLater();
    });

    //Iniciar los hilos
    pcapThread->start();
    sqliteThread->start();

    //Cambiar de vista
    this->MVM->setCurrentView(target);

}
