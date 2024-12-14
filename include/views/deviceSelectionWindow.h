#ifndef DEVICESELECTIONWINDOW_H
#define DEVICESELECTIONWINDOW_H

#include <QListView>
#include <QPushButton>
#include <QStandardItemModel>
#include "models/deviceModel.h"

class DeviceSelectionWindow : public QWidget
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
