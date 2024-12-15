#include "views/snifferWindow.h"
#include <QSplitter>
#include <QTreeWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

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

    // ------------------------------
    // Crear el cuadro de consultas SQL
    // ------------------------------
    sqlTextArea = new QTextEdit(this);
    sqlTextArea->setPlaceholderText("Escribe tu consulta SQL aquí (solo comandos SELECT)");
    executeSqlButton = new QPushButton("Ejecutar Consulta", this);

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
    connect(executeSqlButton, &QPushButton::clicked, this, [this]() {
        QString query = sqlTextArea->toPlainText().trimmed();

        // Validar que el comando comience con SELECT
        if (!query.startsWith("SELECT", Qt::CaseInsensitive)) {
            QMessageBox::warning(this, "Consulta no válida",
                                 "Solo se permiten comandos SELECT en las consultas SQL.");
            return;
        }

        // Procesar la consulta
        processSqlQuery(query);
    });
}

snifferWindow::~snifferWindow() {}

void snifferWindow::applyTrafficFilter(const QString &query, const QString &filterType) {
    // Mostrar en consola el filtro aplicado
    qDebug() << "Filtrando con query:" << query << "y tipo de tráfico:" << filterType;

    // Iterar sobre las filas de la tabla
    for (int row = 0; row < packetTable->rowCount(); ++row) {
        bool match = true;

        // Verificar el tipo de tráfico (columna "Protocolo", índice 5)
        QString protocol = packetTable->item(row, 5)->text();
        if (filterType != "Todos" && protocol != filterType) {
            match = false;
        }

        // Verificar si el texto de búsqueda está presente
        if (!query.isEmpty()) {
            bool queryFound = false;
            for (int col = 0; col < packetTable->columnCount(); ++col) {
                if (packetTable->item(row, col) &&
                    packetTable->item(row, col)->text().contains(query, Qt::CaseInsensitive)) {
                    queryFound = true;
                    break;
                }
            }
            if (!queryFound) {
                match = false;
            }
        }

        // Mostrar/ocultar la fila según el resultado
        packetTable->setRowHidden(row, !match);
    }
}

void snifferWindow::processSqlQuery(const QString &query) {
    // Simulación de ejecución de consulta
    QMessageBox::information(this, "Consulta SQL", "Consulta ejecutada:\n" + query);

    // Filtrar filas según una lógica de SQL (simulación)
    if (query.contains("WHERE", Qt::CaseInsensitive)) {
        QString filterValue = query.split("WHERE", Qt::SkipEmptyParts).last().trimmed();

        // Aplicar el filtro como ejemplo
        applyTrafficFilter(filterValue, "Todos");
    }
}

void snifferWindow::addNewPacket() {
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

void snifferWindow::addPacketToTable(const QStringList &packetData) {
    int row = packetTable->rowCount();
    packetTable->insertRow(row);
    for (int col = 0; col < packetData.size(); ++col) {
        packetTable->setItem(row, col, new QTableWidgetItem(packetData[col]));
    }
}
