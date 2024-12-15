#include "views/mainViewManager.h"
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QLineEdit>
#include <QComboBox>
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

    // Agregar acciones a la barra de herramientas
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

    // ------------------------------
    // Añadir buscador y filtro a la barra de herramientas
    // ------------------------------
    // Campo de texto para buscar
    QLineEdit *searchField = new QLineEdit(this);
    searchField->setPlaceholderText("Buscar tráfico...");

    // ComboBox para seleccionar tipo de filtro
    QComboBox *filterType = new QComboBox(this);
    filterType->addItems({"TCP", "UDP", "ICMP", "Todos"});
    filterType->setCurrentIndex(3); // Por defecto, "Todos"

    // Conexión para aplicar filtros
    connect(searchField, &QLineEdit::returnPressed, [this, searchField, filterType]() {
        QString query = searchField->text();
        QString filter = filterType->currentText();

        // Aplica la lógica de filtrado
        if (captureWind) {
            captureWind->applyTrafficFilter(query, filter);
        }
    });

    connect(filterType, &QComboBox::currentTextChanged, [this, searchField, filterType]() {
        QString query = searchField->text();
        QString filter = filterType->currentText();

        // Aplica la lógica de filtrado automáticamente al cambiar el filtro
        if (captureWind) {
            captureWind->applyTrafficFilter(query, filter);
        }
    });

    // Añadir los widgets a la barra de herramientas
    toolbar->addWidget(searchField);
    toolbar->addWidget(filterType);

    // ------------------------------
    // Opción para salir de captura
    // ------------------------------
    QAction *exitCaptureAction = new QAction("Salir de Captura", this);
    connect(exitCaptureAction, &QAction::triggered, [this]() {
        handleExitFromCapture();
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
            // Cambiar a la pantalla de selección de dispositivos
            setCurrentView(devSelectionWind);
        } else if (reply == QMessageBox::No) {
            // No guardar, solo redirigir
            qDebug() << "No se guardaron los datos.";
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

void mainViewManager::setCurrentView(QWidget* view) {
    mainContainer->setCurrentWidget(view);
}
