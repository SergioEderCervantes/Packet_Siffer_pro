#include "views/deviceSelectionWindow.h"
#include <QVBoxLayout>
#include <QDebug>
DeviceSelectionWindow::DeviceSelectionWindow(DeviceModel *model, QWidget *parent)
    :QMainWindow(parent){
    // Definicion de atributos
    this->devListView = new QListView(this);
    this->startCaptureBtn = new QPushButton("Iniciar Captura", this);
    this->devModel = new QStandardItemModel(this);

    startCaptureBtn->setEnabled(false);

    // Llenar la el devModel con todas las interfaces
    for (const NetworkInterface &netIf : model->getInterfaces()){
        auto *item = new QStandardItem(netIf.description);
        item->setData(netIf.name,Qt::UserRole);
        devModel->appendRow(item);
    }

    // Asociar devModel con la list view
    devListView->setModel(devModel);

    devListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Conectar las se;ales y los slots
    connect(devListView, &QListView::clicked, this, &DeviceSelectionWindow::onDeviceSelected);
    connect(devListView, &QListView::doubleClicked, this, &DeviceSelectionWindow::onDeviceDoubleClicked);
    connect(startCaptureBtn, &QPushButton::clicked, this, &DeviceSelectionWindow::onStartCaptureClicked);

    // Dise;o de la vista
    auto *layout = new QVBoxLayout;
    layout->addWidget(devListView);
    layout->addWidget(startCaptureBtn);

    auto *widget = new QWidget(this);
    widget->setLayout(layout);
    this->setCentralWidget(widget);
    this->resize(800,600);
}

void DeviceSelectionWindow::onDeviceSelected(const QModelIndex &index) {
    selectedDevName = index.data(Qt::UserRole).toString();  // Obtener el nombre técnico
    startCaptureBtn->setEnabled(true);  // Habilitar el botón
}

void DeviceSelectionWindow::onStartCaptureClicked() {
    if (!selectedDevName.isEmpty()) {
        emit startCapture(selectedDevName);  // Emitir la señal
    }
}

void DeviceSelectionWindow::onDeviceDoubleClicked(const QModelIndex &index) {
    QString deviceName = index.data(Qt::UserRole).toString();
    emit startCapture(deviceName);  // Emitir la señal directamente
}

DeviceSelectionWindow::~DeviceSelectionWindow(){}
