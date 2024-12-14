#include "views/snifferWindow.h"
#include <QTableWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QTimer>

SnifferWindow::SnifferWindow(QWidget *parent)
    : QWidget(parent) {
    // Crear la tabla de paquetes
    packetTable = new QTableWidget(this);

    // Definir la cantidad de columnas y encabezados
    packetTable->setColumnCount(11);
    packetTable->setHorizontalHeaderLabels({"Id", "SrcIP", "DstIP", "Tos", "TTL",
                                            "Protocolo", "Flags", "SrcPort",
                                            "DstPort", "ICMPType", "ICMPTypeCode"});

    // Configurar el tamaño de la tabla
    packetTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    packetTable->setSelectionBehavior(QAbstractItemView::SelectRows); // Selección por filas completas
    packetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Deshabilitar edición

    // Crear el layout principal y agregar la tabla
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(packetTable);

    // Establecer el layout en el widget principal
    setLayout(mainLayout);

    // Configurar el tamaño inicial de la ventana
    resize(800, 600);
}


SnifferWindow::~SnifferWindow() {
    // Destructor (puedes liberar recursos si es necesario)
}

void SnifferWindow::addNewPacket() {
    // Agregar una nueva fila con datos simulados
    int row = packetTable->rowCount();
    packetTable->insertRow(row);

    // Llenar la fila con datos (simulados para el ejemplo)
    packetTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1))); // Id
    packetTable->setItem(row, 1, new QTableWidgetItem("192.168.1.1")); // SrcIP
    packetTable->setItem(row, 2, new QTableWidgetItem("192.168.1.2")); // DstIP
    packetTable->setItem(row, 3, new QTableWidgetItem("0x00")); // Tos
    packetTable->setItem(row, 4, new QTableWidgetItem("64")); // TTL
    packetTable->setItem(row, 5, new QTableWidgetItem("TCP")); // Protocolo
    packetTable->setItem(row, 6, new QTableWidgetItem("0x01")); // Flags
    packetTable->setItem(row, 7, new QTableWidgetItem("80")); // SrcPort
    packetTable->setItem(row, 8, new QTableWidgetItem("443")); // DstPort
    packetTable->setItem(row, 9, new QTableWidgetItem("8")); // ICMPType
    packetTable->setItem(row, 10, new QTableWidgetItem("0")); // ICMPTypeCode
}

void SnifferWindow::addPacketToTable(const QStringList &packetData){
    int row = packetTable->rowCount();
    packetTable->insertRow(row);
    for (int col = 0; col < packetData.size(); ++col) {
        packetTable->setItem(row,col,new QTableWidgetItem(packetData[col]));
    }
}
