#include "views/mainViewManager.h"
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include "models/deviceModel.h"
#include "controllers/deviceController.h"
mainViewManager::mainViewManager(QWidget *parent)
    : QMainWindow(parent), mainContainer(new QStackedWidget(this)){
    setupViews();
    setupMenuBar();
    setupToolBar();
    setCentralWidget(mainContainer);

    mainContainer->setCurrentWidget(devSelectionWind);
}

//Funcion para inicializar todas las vistas en el mainContainer
void mainViewManager::setupViews(){
    DeviceModel *devModel = new DeviceModel(this);
    this->devSelectionWind = new DeviceSelectionWindow(devModel, this);

    DeviceController *devController = new DeviceController(devModel,devSelectionWind,this);
    this->captureWind = new SnifferWindow(this);

    mainContainer->addWidget(devSelectionWind);
    mainContainer->addWidget(captureWind);
}

void mainViewManager::setupMenuBar(){
    // Crear menús
    QMenu *fileMenu = menuBar()->addMenu("Archivo");
    QMenu *helpMenu = menuBar()->addMenu("Ayuda");

    // Agregar acciones al menú "Archivo"
    QAction *exitAction = new QAction("Salir", this);
    connect(exitAction, &QAction::triggered, this, &mainViewManager::close);
    fileMenu->addAction(exitAction);

    // Agregar acciones al menú "Ayuda"
    QAction *aboutAction = new QAction("Acerca de", this);
    connect(aboutAction, &QAction::triggered, this, []() {
        QMessageBox::information(nullptr, "Acerca de", "Esta es una aplicación de ejemplo.");
    });
    helpMenu->addAction(aboutAction);
}

void mainViewManager::setupToolBar() {
    // Crear una barra de herramientas
    QToolBar *toolbar = addToolBar("Barra de herramientas");

    // Agregar acciones a la barra de herramientas
    QAction *homeAction = new QAction("Inicio", this);
    connect(homeAction, &QAction::triggered, [this]() {
        mainContainer->setCurrentWidget(devSelectionWind);
    });

    QAction *captureAction = new QAction("Captura", this);
    connect(captureAction, &QAction::triggered, [this]() {
        qDebug() << "Captura";
    });

    toolbar->addAction(homeAction);
    toolbar->addAction(captureAction);
}

SnifferWindow* mainViewManager::getSnifferWindow(){
    return this->captureWind;
}

void mainViewManager::setCurrentView(QWidget* view){
    mainContainer->setCurrentWidget(view);
}
