#include "controllers/deviceController.h"
#include <QDebug>
#include "models/pcapThread.h"
#include "models/SQLiteThread.h"

DeviceController::DeviceController(DeviceModel *model, DeviceSelectionWindow *view, mainViewManager *MVM, QObject *parent)
    :  QObject(parent), model(model), view(view), MVM(MVM)  {
    connect(view,  &DeviceSelectionWindow::startCapture,  this,  &DeviceController::handleStartCapture);
    connect(view, &DeviceSelectionWindow::openQueryView, this, &DeviceController::handleOpenQueryView);
}

    // Obtener la ventana de captura
void DeviceController::handleStartCapture(const QString &devName) {
    qDebug() << "Iniciando captura en dispositivo:" << devName;

    // Obtener la ventana de captura
    snifferWindow *target = this->MVM->getSnifferWindow();

    // Crear el hilo de captura
    PcapThread *pcapThread = new PcapThread(devName.toUtf8().data(), this->MVM->getFilterType(), this);

    // Crear el hilo de SQLite
    SQLiteThread *sqliteThread = new SQLiteThread(devName, this);

    // Pasar el hilo PcapThread al snifferWindow (para manejar pausa/reanudación)

    // Conectar señales del hilo PcapThread
    connect(pcapThread, &PcapThread::packetCaptured, target, &snifferWindow::addPacketToTable);
    connect(pcapThread, &PcapThread::sendPacketToDB, sqliteThread, &SQLiteThread::savePacket);

    // Conectar SQLite para manejar consultas
    connect(target, &snifferWindow::fetchRowData, sqliteThread, &SQLiteThread::onFetchRowData);
    connect(sqliteThread, &SQLiteThread::rowDataResponse, target, &snifferWindow::onRowDataResponse);
    connect(target, &snifferWindow::exec_query, sqliteThread, &SQLiteThread::onExec_query);

    // Conectar señales de finalización para limpiar recursos
    connect(pcapThread, &PcapThread::finished, this, [=]() {
        qDebug() << "Hilo de captura finalizado.";
        pcapThread->deleteLater();
    });
    connect(sqliteThread, &SQLiteThread::finished, this, [=]() {
        qDebug() << "Hilo de base de datos finalizado.";
        sqliteThread->deleteLater();
    });

    // Conectar señales de `mainViewManager` para detener los hilos
    connect(this->MVM, &mainViewManager::killThread, pcapThread, &PcapThread::handlerKiller);
    connect(this->MVM, &mainViewManager::killThread, sqliteThread, &SQLiteThread::handleKiller);
    connect(this->MVM, &mainViewManager::clear, target, &snifferWindow::clearPacketTableAndOthers);
    connect(target, &snifferWindow::pauseAction, pcapThread, &PcapThread::stop);
    connect(target, &snifferWindow::resumeAction, pcapThread, &PcapThread::resume);
    // Iniciar los hilos
    pcapThread->start();
    sqliteThread->start();

    // Cambiar la vista al snifferWindow
    this->MVM->setCurrentView(target);
}
void DeviceController::handleOpenQueryView(){
    queryViewWindow *target = this->MVM->getQueryViewWindow();
    this->MVM->setCurrentView(target);
}
