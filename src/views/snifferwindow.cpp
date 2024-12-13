#include "views/snifferwindow.h"
#include <QTableWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QTimer>

SnifferWindow::SnifferWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Crear el QTabWidget y las pestañas
    tabWidget = new QTabWidget(this);

    // Pestaña de Paquetes
    QWidget *packetTab = new QWidget();
    packetTable = new QTableWidget(packetTab);

    // Definir la cantidad de columnas (sin cambiar)
    packetTable->setColumnCount(11); // Número de columnas
    packetTable->setHorizontalHeaderLabels({"Id", "SrcIP", "DstIP", "Tos", "TTL",
                                            "Protocolo", "Flags", "SrcPort",
                                            "DstPort", "ICMPType", "ICMPTypeCode"});

    // Hacer que la tabla se ajuste al tamaño de la pestaña
    packetTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // Expande en ambas direcciones

    // Crear el layout para la pestaña de paquetes
    QVBoxLayout *packetLayout = new QVBoxLayout(packetTab);
    packetLayout->addWidget(packetTable);
    packetTab->setLayout(packetLayout);  // Establecer el layout en la pestaña de paquetes

    // Llamar a adjustSize() para que la pestaña ajuste su tamaño según la tabla
    packetTab->adjustSize();  // Ajusta el tamaño de la pestaña según su contenido (la tabla)

    // Pestaña de Filtros
    QWidget *filterTab = new QWidget();
    filterCombo = new QComboBox(filterTab);
    filterCombo->addItem("TCP");
    filterCombo->addItem("UDP");
    filterCombo->addItem("ICMP");

    // Hacer que el combo box se ajuste al tamaño de la pestaña
    filterCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // Expande horizontalmente, fijo en altura

    // Crear el layout para la pestaña de filtros
    QVBoxLayout *filterLayout = new QVBoxLayout(filterTab);
    filterLayout->addWidget(filterCombo);
    filterTab->setLayout(filterLayout);  // Establecer el layout en la pestaña de filtros

    // Pestaña de Configuración
    QWidget *configTab = new QWidget();
    configText = new QTextEdit(configTab);
    configText->setPlainText("Configuración...");

    // Hacer que el QTextEdit se ajuste al tamaño de la pestaña
    configText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // Expande en ambas direcciones

    // Crear el layout para la pestaña de configuración
    QVBoxLayout *configLayout = new QVBoxLayout(configTab);
    configLayout->addWidget(configText);
    configTab->setLayout(configLayout);  // Establecer el layout en la pestaña de configuración

    // Agregar las pestañas al QTabWidget
    tabWidget->addTab(packetTab, "Paquetes");
    tabWidget->addTab(filterTab, "Filtros");
    tabWidget->addTab(configTab, "Configuración");

    // Configurar la ventana principal
    setCentralWidget(tabWidget);

    // Cambiar el tamaño de la ventana (puedes ajustarlo a tu preferencia)
    this->resize(800, 600);  // Ancho de 800px y altura de 600px

    connect(this,&SnifferWindow::packetCaptured, this, &SnifferWindow::addPacketToTable);

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
