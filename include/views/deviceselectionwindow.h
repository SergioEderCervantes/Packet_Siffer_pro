#ifndef DEVICESELECTIONWINDOW_H
#define DEVICESELECTIONWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QPushButton>
#include <QStandardItemModel>
#include "models/devicemodel.h"

class DeviceSelectionWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DeviceSelectionWindow(DeviceModel *model, QWidget *parent = nullptr);
    ~DeviceSelectionWindow();

signals:
    void startCapture(const QString &devName);

private slots:
    void onDeviceSelected(const QModelIndex &index);
    void onStartCaptureClicked();
    void onDeviceDoubleClicked(const QModelIndex &index);


private:
    QListView *devListView;
    QPushButton *startCaptureBtn;
    QStandardItemModel *devModel;
    QString selectedDevName;


};

#endif // DEVICESELECTIONWINDOW_H
