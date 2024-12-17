#include "views/deviceSelectionWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>

DeviceSelectionWindow::DeviceSelectionWindow(DeviceModel *model, QWidget *parent)
    : QWidget(parent) {

    // Definición de atributos
    this->devLabel = new QLabel("Interfaces disponibles", this);
    this->devLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #333333; }");
    this->tableLabel = new QLabel("Previas capturas guardadas", this);
    this->tableLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #333333; }");

    this->devListView = new QListView(this);
    this->tableListView = new QListView(this);

    this->startCaptureBtn = new QPushButton("Iniciar Captura", this);
    this->openQueryViewBtn = new QPushButton("Open Query View", this);

    this->devModel = new QStandardItemModel(this);
    this->tableModel = new QStandardItemModel(this);

    startCaptureBtn->setEnabled(false);
    openQueryViewBtn->setEnabled(true);

    startCaptureBtn->setFixedHeight(40);
    openQueryViewBtn->setFixedHeight(40);

    // Estilizar Botones

    // Asignar nombres de objeto únicos
    startCaptureBtn->setObjectName("styledButton");
    openQueryViewBtn->setObjectName("styledButton");

    // Establecer estilo compartido
    this->setStyleSheet(R"(
    QPushButton#styledButton {
        background-color: #7e0b99;
        color: white;
        border-radius: 5px;
        padding: 5px 10px;
        border: 1px solid #6b0a86;
    }
    QPushButton#styledButton:hover {
        background-color: #9a22b3;
        border: 1px solid #7e0b99;
    }
    QPushButton#styledButton:pressed {
        background-color: #5e0873;
        border: 1px solid #4a0657;
    }
)");

    // Estilizar QListView
    QString listStyle = "QListView { font-size: 14px; }"
                        "QListView::item { padding: 6px; }";
    devListView->setStyleSheet(listStyle);
    tableListView->setStyleSheet(listStyle);

    // Llenar devModel con interfaces de red
    for (const NetworkInterface &netIf : model->getInterfaces()) {
        auto *item = new QStandardItem(netIf.description);
        item->setData(netIf.name, Qt::UserRole);
        devModel->appendRow(item);
    }

    // Llenar tableModel con nombres de tablas
    for (const QString &tableName : model->getTableNames()) {
        auto *item = new QStandardItem(tableName);
        item->setData(tableName, Qt::UserRole);
        tableModel->appendRow(item);
    }

    devListView->setModel(devModel);
    tableListView->setModel(tableModel);

    devListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Crear menú contextual
    tableContextMenu = new QMenu(this);
    tableContextMenu->addAction("Borrar Tabla", this, &DeviceSelectionWindow::onDeleteTable);
    tableContextMenu->addAction("Renombrar Tabla", this, &DeviceSelectionWindow::onRenameTable);
    tableContextMenu->addAction("Exportar a Excel", this, &DeviceSelectionWindow::onExportTableToExcel);

    tableListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableListView, &QListView::customContextMenuRequested, this, &DeviceSelectionWindow::onTableMenuRequested);

    // Conectar botones
    connect(devListView, &QListView::clicked, this, &DeviceSelectionWindow::onDeviceSelected);
    connect(devListView, &QListView::doubleClicked, this, &DeviceSelectionWindow::onDeviceDoubleClicked);
    connect(tableListView, &QListView::doubleClicked, this, &DeviceSelectionWindow::onTableSelected);
    connect(startCaptureBtn, &QPushButton::clicked, this, &DeviceSelectionWindow::onStartCaptureClicked);
    connect(openQueryViewBtn, &QPushButton::clicked, this, &DeviceSelectionWindow::onOpenQueryView);
    connect(this, &DeviceSelectionWindow::createCSV, model, &DeviceModel::onCreateCSV);
    // Layout principal
    auto *mainLayout = new QHBoxLayout;
    auto *leftLayout = new QVBoxLayout;
    auto *rightLayout = new QVBoxLayout;

    leftLayout->addWidget(devLabel);
    leftLayout->addWidget(devListView);
    leftLayout->addWidget(startCaptureBtn);

    rightLayout->addWidget(tableLabel);
    rightLayout->addWidget(tableListView);
    rightLayout->addWidget(openQueryViewBtn);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    setLayout(mainLayout);
}

void DeviceSelectionWindow::onDeviceSelected(const QModelIndex &index) {
    selectedDevName = index.data(Qt::UserRole).toString();
    startCaptureBtn->setEnabled(true);
}

void DeviceSelectionWindow::onDeviceDoubleClicked(const QModelIndex &index) {
    QString deviceName = index.data(Qt::UserRole).toString();
    emit startCapture(deviceName);  // Emitir la señal directamente
}

void DeviceSelectionWindow::onTableSelected() {
    emit openQueryView();
}

void DeviceSelectionWindow::onTableMenuRequested(const QPoint &pos) {
    QModelIndex index = tableListView->indexAt(pos);
    if (index.isValid()) {
        selectedTableName = index.data(Qt::UserRole).toString();
        tableContextMenu->exec(tableListView->viewport()->mapToGlobal(pos));
    }
}

void DeviceSelectionWindow::onDeleteTable() {
    qDebug() << "Borrar tabla:" << selectedTableName;
}

void DeviceSelectionWindow::onRenameTable() {
    bool ok;
    QString newName = QInputDialog::getText(this, "Renombrar Tabla", "Nuevo nombre:", QLineEdit::Normal, selectedTableName, &ok);
    if (ok && !newName.isEmpty()) {
        qDebug() << "Renombrar tabla a:" << newName;
    }
}

void DeviceSelectionWindow::onExportTableToExcel() {
    // Obtener la ruta para guardar el archivo CSV
    QString filePath = QFileDialog::getSaveFileName(this, "Exportar a CSV", "", "Archivos CSV (*.csv)");
    if (filePath.isEmpty()) {
        qDebug() << "Exportación cancelada.";
        return;
    }

    // Asegurar que el archivo tenga la extensión .csv
    if (!filePath.endsWith(".csv")) {
        filePath += ".csv";
    }
    emit createCSV(filePath, selectedTableName);

}

void DeviceSelectionWindow::onStartCaptureClicked() {
    emit startCapture(selectedDevName);
}

void DeviceSelectionWindow::onOpenQueryView() {
    emit openQueryView();
}

DeviceSelectionWindow::~DeviceSelectionWindow() {}
