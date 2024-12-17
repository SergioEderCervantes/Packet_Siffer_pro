#ifndef DEVICESELECTIONWINDOW_H
#define DEVICESELECTIONWINDOW_H

#include <QListView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QMenu>
#include <QLabel>
#include "models/deviceModel.h"

class DeviceSelectionWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceSelectionWindow(DeviceModel *model, QWidget *parent = nullptr);
    ~DeviceSelectionWindow();

signals:
    void startCapture(const QString &devName);
    void openQueryView();
    void deleteTable(const QString &tableName);
    void renameTable(const QString &tableName);
    void exportTableToExcel(const QString &tableName);
    void createCSV(const QString &filePath, const QString &selectedTableName);

private slots:
    void onDeviceSelected(const QModelIndex &index);
    void onDeviceDoubleClicked(const QModelIndex &index);
    void onTableSelected();
    void onStartCaptureClicked();
    void onOpenQueryView();
    void onTableMenuRequested(const QPoint &pos); // Menú contextual
    void onDeleteTable();
    void onRenameTable();
    void onExportTableToExcel();

private:
    QListView *devListView;
    QListView *tableListView;
    QPushButton *startCaptureBtn;
    QPushButton *openQueryViewBtn;

    QStandardItemModel *devModel;
    QStandardItemModel *tableModel;

    QLabel *devLabel;
    QLabel *tableLabel;

    QString selectedDevName;
    QString selectedTableName;

    QMenu *tableContextMenu;
};

#endif // DEVICESELECTIONWINDOW_H
