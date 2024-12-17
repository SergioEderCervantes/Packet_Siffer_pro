#ifndef SNIFFERWINDOW_H
#define SNIFFERWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QTabWidget>
#include <QPushButton>
#include <models/pcapThread.h>
#include <QTreeWidget>
class snifferWindow : public QWidget
{
    Q_OBJECT

public:
    explicit snifferWindow(QWidget *parent = nullptr,PcapThread *thread=nullptr);
    ~snifferWindow();
    // Métodos para aplicar filtros y procesar consultas SQL
    void applyTrafficFilter(const QString &filterType);
    void clearPacketTableAndOthers();
private:
    // Secciones Principales de la vista
    QTableWidget *packetTable;    // Tabla para mostrar los paquetes capturados
    QTextEdit *sqlTextArea;       // Área de texto para consultas SQL
    QPushButton *executeSqlButton;// Botón para ejecutar consultas SQL
    QTreeWidget *detailsView;       //Cuadro de detalles
    QTextEdit *rawDataView;         //Cuadro de raw
    QPushButton *openSqlButton;     //Boton para abrir sql
    QPushButton *pauseButton;
    QPushButton *resumeButton;
    PcapThread *pcapThread;
    // metodos varios
    bool validateQueryFormat(const QString &query);
public slots:
    void addPacketToTable(const QStringList &packetData); // Añadir paquetes a la tabla
    void onRowSelected();
    void onRowDataResponse(QStringList &packetData, QByteArray &rawData);

private slots:
    void openSqlPopup();
    void pausar();
    void reanudar();
signals:
    void fetchRowData(int row);
    void exec_query(QString &query, QTextEdit *resultView);
    void pauseAction();
    void resumeAction();

};

#endif // SNIFFERWINDOW_H
