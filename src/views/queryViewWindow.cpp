#include "views/queryViewWindow.h"
#include <QLabel>
#include <QDebug>
#include <QSpacerItem>
#include <QTableWidget>
queryViewWindow::queryViewWindow(queryModel *model, QWidget *parent)
    : QWidget(parent), model(model)
{
    // ----------------------
    // Sidebar (ListView)
    // ----------------------
    sidebarListView = new QListView(this);

    sidebarListView->setStyleSheet("QListView { background-color: #2E2E2E; color: white; font-size: 16px; }"
                                   "QListView::item { padding: 8px; }"
                                   "QListView::item:selected { background-color: #4A4A4A; }");

    // ----------------------
    // Query Area (TextEdit + Button)
    // ----------------------
    queryTextArea = new QTextEdit(this);
    queryTextArea->setFixedHeight(40);
    queryTextArea->setPlaceholderText("Escribe tu consulta aquí...");

    executeButton = new QPushButton("Ejecutar", this);
    executeButton->setFixedSize(100, 40);

    // Layout para el Query TextArea y Botón
    auto *queryLayout = new QHBoxLayout;
    queryLayout->addWidget(queryTextArea);
    queryLayout->addWidget(executeButton);

    // ----------------------
    // Contenedor de Tabla Dinámica (Espacio Vacío)
    // ----------------------
    tableContainer = new QWidget(this);

    tableContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ----------------------
    // Layout Principal
    // ----------------------
    auto *mainLayout = new QHBoxLayout(this);

    // Sidebar en un layout vertical
    auto *sidebarLayout = new QVBoxLayout;
    QLabel *sidebarLabel = new QLabel("Capturas Guardadas", this);
    sidebarLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #333333; }");
    sidebarLayout->addWidget(sidebarLabel);
    sidebarLayout->addWidget(sidebarListView);

    // Centro: Query + Contenedor de tabla
    auto *centerLayout = new QVBoxLayout;
    centerLayout->addLayout(queryLayout);
    centerLayout->addWidget(tableContainer);

    mainLayout->addLayout(sidebarLayout, 3); // Sidebar ocupa el 1/4 del espacio
    mainLayout->addLayout(centerLayout, 8);  // Centro ocupa 3/4 del espacio

    setLayout(mainLayout);

    // ----------------------
    // Conexiones
    // ----------------------
    connect(executeButton, &QPushButton::clicked, this, &queryViewWindow::onExecuteButtonClicked);
    connect(this, &queryViewWindow::executeQuery, model, &queryModel::onExecuteQuery);
    connect(model, &queryModel::queryError,this,&queryViewWindow::onQueryError);
    connect(model,&queryModel::queryResultReady,this,&queryViewWindow::onQueryResultReady);

    // Poblar la sidebar

    this->fillSidebar();
}

void queryViewWindow::fillSidebar(){
    QStandardItemModel *sideBarModel = new QStandardItemModel(this);
    // Llenar tableModel con nombres de tablas
    for (const QString &tableName : model->getTableNames()) {
        auto *item = new QStandardItem(tableName);
        item->setData(tableName, Qt::UserRole);
        sideBarModel->appendRow(item);
    }
    this->sidebarListView->setModel(sideBarModel);

}



void queryViewWindow::onExecuteButtonClicked() {
    QString query = queryTextArea->toPlainText().trimmed();
    if (query.isEmpty()) {
        qDebug() << "El área de texto está vacía.";
        return;
    }
    emit executeQuery(query);
    qDebug() << "Ejecutar consulta:" << query;
}

queryViewWindow::~queryViewWindow() {}

void queryViewWindow::onQueryResultReady(const QVector<QVector<QString>> &data, const QStringList &headers) {
    // Limpiar el contenedor anterior
    QLayout *layout = tableContainer->layout();
    if (layout) {
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        delete layout;
    }

    // Crear QTableWidget dinámicamente
    QTableWidget *tableWidget = new QTableWidget(tableContainer);
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setRowCount(data.size());

    for (int row = 0; row < data.size(); ++row) {
        for (int col = 0; col < headers.size(); ++col) {
            tableWidget->setItem(row, col, new QTableWidgetItem(data[row][col]));
        }
    }

    tableWidget->resizeColumnsToContents();

    // Añadir tabla al contenedor
    QVBoxLayout *newLayout = new QVBoxLayout;
    newLayout->addWidget(tableWidget);
    tableContainer->setLayout(newLayout);
}

void queryViewWindow::onQueryError(const QString &errorMessage) {
    QLayout *layout = tableContainer->layout();
    if (layout) {
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        delete layout;
    }

    QLabel *errorLabel = new QLabel("Error: " + errorMessage, tableContainer);
    errorLabel->setStyleSheet("QLabel { color: red; font-size: 16px; }");

    QVBoxLayout *newLayout = new QVBoxLayout;
    newLayout->addWidget(errorLabel);
    tableContainer->setLayout(newLayout);
}
