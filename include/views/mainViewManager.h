#ifndef MAINVIEWMANAGER_H
#define MAINVIEWMANAGER_H


#include <QMainWindow>
#include <QStackedWidget>
#include "views/deviceSelectionWindow.h"

class mainViewManager : public QMainWindow {
    Q_OBJECT

public:
    explicit mainViewManager(QWidget *parent = nullptr);

private:
    QStackedWidget *mainContainer;

    DeviceSelectionWindow *devSelectionWind;

    void setupViews();
    void setupMenuBar();
    void setupToolBar();
private slots:
    void onStartCapture(const QString &deviceName);
};

#endif // MAINVIEWMANAGER_H
