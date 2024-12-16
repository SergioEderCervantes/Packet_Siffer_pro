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
    QAction *homeAction = new QAction("Inicio", this);
    connect(homeAction, &QAction::triggered, [this]() {
        mainContainer->setCurrentWidget(devSelectionWind);
    });

    QAction *captureAction = new QAction("Captura", this);
    connect(captureAction, &QAction::triggered, [this]() {
        mainContainer->setCurrentWidget(captureWind);
    });

    toolbar->addAction(homeAction);
    toolbar->addAction(captureAction);

    // Separador flexible
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    // Filtro de tráfico
    this->filterType = new QComboBox(this);
    filterType->addItems({"Todos", "TCP", "UDP", "ICMP"});
    filterType->setEnabled(true); // Deshabilitar por defecto
    toolbar->addWidget(filterType);

    // Detectar cambio de pestañas para habilitar/deshabilitar el filtro
    connect(mainContainer, &QStackedWidget::currentChanged, this, [this](int index) {
        if (mainContainer->currentWidget() == captureWind) {
            filterType->setEnabled(false);
        } else {
            filterType->setEnabled(true);
        }
    });


    // Opción para salir de captura
    QAction *exitCaptureAction = new QAction("Salir de Captura", this);
    connect(exitCaptureAction, &QAction::triggered, [this]() {
        handleExitFromCapture();

        // Restablecer filtro

    });
    toolbar->addAction(exitCaptureAction);
}

void mainViewManager::handleExitFromCapture() {
    // Verifica si estamos en la pantalla de captura antes de mostrar el diálogo
    if (mainContainer->currentWidget() == captureWind) {
        // Mostrar un cuadro de diálogo para preguntar al usuario
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Guardar datos",
            "¿Deseas guardar los datos antes de salir?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

        if (reply == QMessageBox::Yes) {
            // Lógica para guardar los datos
            qDebug() << "Guardando datos en la base de datos...";
            saveCapturedData();
            filterType->setEnabled(true);
            filterType->setCurrentIndex(0); // Seleccionar "Todos" por defecto
            // Cambiar a la pantalla de selección de dispositivos
            emit clear();
            emit killThread();
            setCurrentView(devSelectionWind);
        } else if (reply == QMessageBox::No) {
            // No guardar, solo redirigir
            qDebug() << "No se guardaron los datos.";
            filterType->setEnabled(true);
            filterType->setCurrentIndex(0); // Seleccionar "Todos" por defecto
            emit clear();
            emit killThread();

            setCurrentView(devSelectionWind);
        } else {
            // Cancelar, no hacer nada
            qDebug() << "El usuario canceló la acción.";
        }
    } else {
        qDebug() << "No estás en la pantalla de captura.";
    }
}

void mainViewManager::saveCapturedData() {
    // Aquí implementa la lógica para guardar los datos capturados en la base de datos
    qDebug() << "Datos guardados exitosamente.";
    // Por ejemplo:
    // captureWind->saveToDatabase();
}

snifferWindow* mainViewManager::getSnifferWindow() {
    return this->captureWind;
}
QString mainViewManager::getFilterType(){

    QString filter = filterType->currentText();

    return filter;
}
void mainViewManager::setCurrentView(QWidget* view) {
    mainContainer->setCurrentWidget(view);
}
