#ifndef SNIFFERWINDOW_H
#define SNIFFERWINDOW_H

#include <QTableWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QTabWidget>
#include <QTimer>

class SnifferWindow : public QWidget
{
    Q_OBJECT

public:
    SnifferWindow(QWidget *parent = nullptr);
    ~SnifferWindow();

private slots:
    void addNewPacket();  // Método para agregar un nuevo paquete a la tabla

private:
    QTabWidget *tabWidget;
    QTableWidget *packetTable;
    QComboBox *filterCombo;
    QTextEdit *configText;


public slots:
    void addPacketToTable(const QStringList &packetData);
};

#endif // SNIFFERWINDOW_H
