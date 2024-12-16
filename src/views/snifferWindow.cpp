#include "views/snifferWindow.h"
#include <QSplitter>
#include <QTreeWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
snifferWindow::snifferWindow(QWidget *parent)
    : QWidget(parent) {
    // ------------------------------
    // Crear la tabla de paquetes
    // ------------------------------
    packetTable = new QTableWidget(this);
    packetTable->setColumnCount(11);
    packetTable->setHorizontalHeaderLabels({"Id", "SrcIP", "DstIP", "Tos", "TTL",
                                            "Protocolo", "Flags", "SrcPort",
                                            "DstPort", "ICMPType", "ICMPTypeCode"});
    packetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    packetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(packetTable,&QTableWidget::itemSelectionChanged,this,&snifferWindow::onRowSelected);

    // ------------------------------
    // Botón para abrir el panel de consultas
    // ------------------------------
    openSqlButton = new QPushButton("Abrir Panel de Consultas", this);
    openSqlButton->setFixedHeight(40);
    openSqlButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    openSqlButton->setMaximumWidth(200); // Aproximadamente el 20% si la ventana mide 1000px
    connect(openSqlButton, &QPushButton::clicked, this, &snifferWindow::openSqlPopup);

    // ------------------------------
    // Crear el visor de datos crudos con un título
    // ------------------------------
    QWidget *rawContainer = new QWidget(this);
    QVBoxLayout *rawLayout = new QVBoxLayout(rawContainer);
    QLabel *rawLabel = new QLabel("Raw del paquete", rawContainer);
    rawLabel->setAlignment(Qt::AlignLeft);
    rawDataView = new QTextEdit(this);
    rawDataView->setReadOnly(true);
    rawDataView->setText("Aquí se mostrarán los datos crudos del paquete seleccionado.");

    rawLayout->addWidget(rawLabel);
    rawLayout->addWidget(rawDataView);
    rawLayout->setContentsMargins(10, 10, 10, 10); // Padding para raw
    rawContainer->setLayout(rawLayout);

    // ------------------------------
    // Crear el visor de detalles con padding
    // ------------------------------
    QWidget *detailsContainer = new QWidget(this);
    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsContainer);
    QLabel *detailsLabel = new QLabel("Detalles del paquete", detailsContainer);
    detailsLabel->setAlignment(Qt::AlignLeft);
    detailsView = new QTreeWidget(this);
    detailsView->setHeaderLabel("Detalles del paquete");

    detailsLayout->addWidget(detailsLabel);
    detailsLayout->addWidget(detailsView);
    detailsLayout->setContentsMargins(10, 10, 10, 10); // Padding similar al de raw
    detailsContainer->setLayout(detailsLayout);

    // ------------------------------
    // Crear divisores principales (QSplitter)
    // ------------------------------
    // Divisor derecho: datos crudos (arriba) y detalles (abajo)
    QSplitter *rightSplitter = new QSplitter(Qt::Vertical, this);
    rightSplitter->addWidget(rawContainer);
    rightSplitter->addWidget(detailsContainer);

    // Divisor principal: tabla de paquetes (izquierda) y panel derecho
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(packetTable);
    mainSplitter->addWidget(rightSplitter);

    // Establecer tamaños iniciales: 70% para la tabla, 30% para el panel derecho
    QList<int> sizes;
    sizes << 700 << 300; // Asumiendo 1000px de ancho total
    mainSplitter->setSizes(sizes);

    // ------------------------------
    // Layout principal
    // ------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(openSqlButton, 0, Qt::AlignLeft); // Botón para abrir consultas SQL
    mainLayout->addWidget(mainSplitter);
    setLayout(mainLayout);

    // Configurar el tamaño inicial de la ventana
    resize(1000, 800);
}

snifferWindow::~snifferWindow() {}


void snifferWindow::openSqlPopup() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Panel de Consultas SQL");
    dialog->setModal(false); // No bloquear la ventana principal

    // Crear el layout del popup
    QVBoxLayout *layout = new QVBoxLayout(dialog);

    // Leyenda al inicio del panel
    QLabel *infoLabel = new QLabel("En este panel solo es posible hacer consulta de actual tabla, la cual se referencia con la palabra 'actual'", dialog);
    infoLabel->setWordWrap(true);

    // Crear el área de texto para consultas SQL
    QTextEdit *sqlInput = new QTextEdit(dialog);
    sqlInput->setPlaceholderText("i.e: SELECT * FROM actual WHERE protocol = 'TCP';");
    sqlInput->setFixedHeight(100);

    // Crear el botón de ejecutar consulta
    QPushButton *executeButton = new QPushButton("Ejecutar", dialog);
    QTextEdit *resultView = new QTextEdit(dialog);
    resultView->setReadOnly(true);
    resultView->setPlaceholderText("Aquí se mostrarán los resultados de la consulta.");

    layout->addWidget(infoLabel);
    layout->addWidget(sqlInput);
    layout->addWidget(executeButton);
    layout->addWidget(resultView);

    connect(executeButton, &QPushButton::clicked, this, [sqlInput, resultView]() {
        QString query = sqlInput->toPlainText().trimmed();
        if (!query.startsWith("SELECT", Qt::CaseInsensitive)) {
            resultView->setText("Solo se permiten consultas SELECT.");
            return;
        }
        // Simulación de ejecución de consulta
        resultView->setText("Ejecutando consulta:\n" + query);
    });

    dialog->setLayout(layout);
    dialog->resize(400, 300);
    dialog->show();
}



void snifferWindow::addPacketToTable(const QStringList &packetData) {
    int row = packetTable->rowCount();
    packetTable->insertRow(row);
    for (int col = 0; col < packetData.size(); ++col) {

        packetTable->setItem(row, col, new QTableWidgetItem(packetData[col]));
    }
}
void snifferWindow::clearPacketTableAndOthers() {
    packetTable->clearContents(); // Limpia los datos de las celdas
    packetTable->setRowCount(0);  // Borra todas las filas
    qDebug() << "Tabla de paquetes limpiada.";
    rawDataView->clear();
    rawDataView->setText("Aquí se mostrarán los datos crudos del paquete seleccionado.");
    detailsView->clear();
    detailsView->setHeaderLabel("Detalles del paquete");
}



void snifferWindow::onRowSelected(){
    // Obtener la fila seleccionada
    QList<QTableWidgetItem*> selectedItems = packetTable->selectedItems();
    if (!selectedItems.isEmpty()) {
        int row = packetTable->currentRow();  // Número de renglón seleccionado

        // Enviar la fila al hilo de base de datos
        emit fetchRowData(row);  // Emite una señal para que el hilo procese la query
    }
}

void snifferWindow::onRowDataResponse(QStringList &packetData, QByteArray &rawData){
    // Limpiar el contenido actual de los visores
    detailsView->clear();
    rawDataView->clear();

    // Llenar el QTreeWidget con la información de packetData
    QStringList labels = {
        "Packet_id" , "IP de Origen", "IP de Destino", "ToS", "TTL",
        "Protocolo", "Flags", "Puerto de Origen", "Puerto de Destino",
        "Tipo ICMP", "Código ICMP"
    };

    for (int i = 0; i < labels.size() && i < packetData.size(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(detailsView);
        item->setText(0, QString("%1: %2").arg(labels[i], packetData[i]));
    }

    // Asegurarse de que todos los nodos sean visibles
    detailsView->expandAll();

    // Llenar el QTextEdit con rawData en formato hexadecimal
    if (!rawData.isEmpty()) {
        QString rawHex = rawData.toHex(' ').toUpper(); // Convertir a Hexadecimal
        rawDataView->setText(rawHex);
    } else {
        rawDataView->setText("No hay datos crudos disponibles.");
    }
}
