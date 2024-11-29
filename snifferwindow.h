#ifndef SNIFFERWINDOW_H
#define SNIFFERWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QTabWidget>
#include <QTimer>

class SnifferWindow : public QMainWindow
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
};

#endif // SNIFFERWINDOW_H
