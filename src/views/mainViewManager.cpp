#include "views/mainViewManager.h"
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QLineEdit>
#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include "models/deviceModel.h"
#include "controllers/deviceController.h"

mainViewManager::mainViewManager(QWidget *parent)
    : QMainWindow(parent), mainContainer(new QStackedWidget(this)) {
    setupViews();
    setupMenuBar();
    setupToolBar();
    setCentralWidget(mainContainer);

    mainContainer->setCurrentWidget(devSelectionWind); // Establece la pantalla inicial

}

// Función para inicializar todas las vistas en el mainContainer
void mainViewManager::setupViews() {
    DeviceModel *devModel = new DeviceModel(this);
    this->devSelectionWind = new DeviceSelectionWindow(devModel, this);

    DeviceController *devController = new DeviceController(devModel, devSelectionWind, this);
    this->captureWind = new snifferWindow(this);

    mainContainer->addWidget(devSelectionWind);
    mainContainer->addWidget(captureWind);
}

void mainViewManager::setupMenuBar() {
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

    // Acciones principales
    // QAction *homeAction = new QAction("Inicio", this);
    // connect(homeAction, &QAction::triggered, [this]() {
    //     mainContainer->setCurrentWidget(devSelectionWind);
    // });

    // QAction *captureAction = new QAction("Captura", this);
    // connect(captureAction, &QAction::triggered, [this]() {
    //     mainContainer->setCurrentWidget(captureWind);
    // });

    // toolbar->addAction(homeAction);
    // toolbar->addAction(captureAction);

    // Reemplazar el spacer por un campo de búsqueda y un botón
    this->searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("Buscar...");

    // this->searchButton = new QPushButton("Buscar", this);

    // Añadir el campo de búsqueda y el botón a la barra de herramientas
    toolbar->addWidget(searchBox);
    // toolbar->addWidget(searchButton);

    // // Conectar el botón de búsqueda
    // connect(searchButton, &QPushButton::clicked, this, [this]() {
    //     QString searchText = searchBox->text();
    //     if (!searchText.isEmpty()) {
    //         qDebug() << "Buscando:" << searchText;

    //     } else {
    //         qDebug() << "Campo de búsqueda vacío.";
    //     }
    // });

    // Conectar Enter para la búsqueda en el QLineEdit
    connect(searchBox, &QLineEdit::returnPressed, searchButton, &QPushButton::click);

    // Filtro de tráfico
    this->filterType = new QComboBox(this);
    filterType->addItems({"PacketId", "SrcIP", "DstIP", "TOS", "TTL","Protocolo","None"});
    filterType->setEnabled(true); // Habilitar por defecto
    filterType->setCurrentIndex(6);
    toolbar->addWidget(filterType);

    // Detectar cambio de pestañas para habilitar/deshabilitar el filtro
    connect(mainContainer, &QStackedWidget::currentChanged, this, [this](int index) {
        if (mainContainer->currentWidget() == captureWind) {
            filterType->setEnabled(false);
            searchBox->setEnabled(false);
        } else {
            filterType->setEnabled(true);
            searchBox->setEnabled(true);

        }
    });

    // Opción para salir de captura
    QAction *exitCaptureAction = new QAction("Salir de Captura", this);
    connect(exitCaptureAction, &QAction::triggered, this, [this]() {
        this->handleExitFromCapture(nullptr);
    });
    toolbar->addAction(exitCaptureAction);

    // Evento cuando cierra la aplicación
    connect(this, &mainViewManager::requestExit, this, &mainViewManager::handleExitFromCapture);
}



snifferWindow* mainViewManager::getSnifferWindow() {
    return this->captureWind;
}
QString mainViewManager::getFilterType(){

    return searchBox->text();


}
int mainViewManager::getIndexFilter(){
    return filterType->currentIndex();
}
void mainViewManager::setCurrentView(QWidget* view) {
    mainContainer->setCurrentWidget(view);
}

void mainViewManager::closeEvent(QCloseEvent *event) {
    emit requestExit(event); // Pasar el event
}

void mainViewManager::handleExitFromCapture(QCloseEvent *event) {
    // Verifica si estamos en la pantalla de captura
    if (mainContainer->currentWidget() != captureWind) {
        qDebug() << "No estás en la pantalla de captura.";
        if (event) event->accept();
        return;
    }

    // Mostrar cuadro de diálogo al usuario
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Guardar datos",
        "¿Deseas guardar los datos antes de salir?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
        );

    switch (reply) {
    case QMessageBox::Yes:
        handleSaveAndExit();
        if (event) event->accept();
        break;

    case QMessageBox::No:
        handleDiscardAndExit();
        if (event) event->accept();
        break;

    case QMessageBox::Cancel:
        qDebug() << "El usuario canceló la acción.";
        if (event) event->ignore();
        break;

    default:
        if(event) event->accept();
        break;
    }
}

void mainViewManager::handleSaveAndExit() {
    qDebug() << "Guardando datos en la base de datos...";
    resetFilters();
    emit clear();
    emit killThread();
    setCurrentView(devSelectionWind);
}

void mainViewManager::handleDiscardAndExit() {
    qDebug() << "No se guardaron los datos.";
    emit deleteTable(); // Borrar la tabla de la base de datos
    resetFilters();
    emit clear();
    emit killThread();
    setCurrentView(devSelectionWind);
}

void mainViewManager::resetFilters() {
    filterType->setEnabled(true);
    filterType->setCurrentIndex(6); // Seleccionar "Todos" por defecto
}


