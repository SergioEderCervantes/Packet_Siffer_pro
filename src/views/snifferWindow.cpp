#include "views/snifferWindow.h"
#include <QSplitter>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QMessageBox>

SnifferWindow::SnifferWindow(QWidget *parent)
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

    // ------------------------------
    // Crear el cuadro de consultas SQL
    // ------------------------------
    QTextEdit *sqlTextArea = new QTextEdit(this);
    sqlTextArea->setPlaceholderText("Escribe tu consulta SQL aquí (solo comandos SELECT)");
    QPushButton *executeSqlButton = new QPushButton("Ejecutar Consulta", this);

    // Crear un layout para el área de consultas SQL
    QWidget *sqlQueryWidget = new QWidget(this);
    QVBoxLayout *sqlLayout = new QVBoxLayout(sqlQueryWidget);
    sqlLayout->addWidget(sqlTextArea);
    sqlLayout->addWidget(executeSqlButton);
    sqlQueryWidget->setLayout(sqlLayout);

    // ------------------------------
    // Crear el visor de detalles
    // ------------------------------
    QTreeWidget *detailsView = new QTreeWidget(this);
    detailsView->setHeaderLabel("Detalles del paquete");

    // ------------------------------
    // Crear el visor de datos crudos
    // ------------------------------
    QTextEdit *rawDataView = new QTextEdit(this);
    rawDataView->setReadOnly(true);
    rawDataView->setText("Aquí se mostrarán los datos crudos del paquete seleccionado.");

    // ------------------------------
    // Crear divisores principales (QSplitter)
    // ------------------------------
    // Divisor horizontal superior: tabla de paquetes y consultas SQL
    QSplitter *topSplitter = new QSplitter(Qt::Horizontal, this);
    topSplitter->addWidget(packetTable);
    topSplitter->addWidget(sqlQueryWidget);

    // Divisor vertical principal: parte superior y visor de detalles + datos crudos
    QSplitter *verticalSplitter = new QSplitter(Qt::Vertical, this);
    verticalSplitter->addWidget(topSplitter);

    // Divisor horizontal inferior: detalles y datos crudos
    QSplitter *bottomSplitter = new QSplitter(Qt::Horizontal, this);
    bottomSplitter->addWidget(detailsView);
    bottomSplitter->addWidget(rawDataView);

    // Agregar el divisor inferior al divisor principal
    verticalSplitter->addWidget(bottomSplitter);

    // ------------------------------
    // Layout principal
    // ------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(verticalSplitter);
    setLayout(mainLayout);

    // Configurar el tamaño inicial de la ventana
    resize(1000, 800);

    // ------------------------------
    // Conectar el botón de consulta SQL
    // ------------------------------
    connect(executeSqlButton, &QPushButton::clicked, this, [sqlTextArea, this]() {
        QString query = sqlTextArea->toPlainText().trimmed();

        // Validar que el comando comience con SELECT
        if (!query.startsWith("SELECT", Qt::CaseInsensitive)) {
            QMessageBox::warning(this, "Consulta no válida",
                                 "Solo se permiten comandos SELECT en las consultas SQL.");
            return;
        }

        // Aquí puedes procesar la consulta y mostrar los resultados
        // processSqlQuery(query);
    });
}

SnifferWindow::~SnifferWindow() {
    // Destructor
}

void SnifferWindow::addNewPacket() {
    int row = packetTable->rowCount();
    packetTable->insertRow(row);

    // Llenar la fila con datos simulados
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

void SnifferWindow::addPacketToTable(const QStringList &packetData) {
    int row = packetTable->rowCount();
    packetTable->insertRow(row);
    for (int col = 0; col < packetData.size(); ++col) {
        packetTable->setItem(row, col, new QTableWidgetItem(packetData[col]));
    }
}

// void SnifferWindow::processSqlQuery(const QString &query) {
//     // Procesar la consulta SQL (simulación)
//     QMessageBox::information(this, "Consulta SQL", "Consulta ejecutada:\n" + query);

//     // Aquí podrías agregar lógica para procesar la consulta y mostrar resultados
// }
