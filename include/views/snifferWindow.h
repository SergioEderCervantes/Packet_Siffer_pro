#ifndef SNIFFERWINDOW_H
#define SNIFFERWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QTabWidget>
#include <QPushButton>

class snifferWindow : public QWidget
{
    Q_OBJECT

public:
    explicit snifferWindow(QWidget *parent = nullptr);
    ~snifferWindow();

    // Métodos para aplicar filtros y procesar consultas SQL
    void applyTrafficFilter(const QString &filterType);
    void processSqlQuery(const QString &query);

private:
    QTableWidget *packetTable;    // Tabla para mostrar los paquetes capturados
    QComboBox *filterCombo;       // ComboBox para seleccionar tipo de tráfico
    QTextEdit *sqlTextArea;       // Área de texto para consultas SQL
    QPushButton *executeSqlButton;// Botón para ejecutar consultas SQL

public slots:
    void addPacketToTable(const QStringList &packetData); // Añadir paquetes a la tabla
};

#endif // SNIFFERWINDOW_H
