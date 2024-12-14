#ifndef MAINVIEWMANAGER_H
#define MAINVIEWMANAGER_H


#include <QMainWindow>
#include <QStackedWidget>
#include "views/deviceSelectionWindow.h"
#include "views/snifferWindow.h"
class mainViewManager : public QMainWindow {
    Q_OBJECT

public:
    explicit mainViewManager(QWidget *parent = nullptr);
    SnifferWindow* getSnifferWindow();
    void setCurrentView(QWidget *view);
private:
    //Contenedor principal
    QStackedWidget *mainContainer;

    //Vistas agregadas
    DeviceSelectionWindow *devSelectionWind;
    SnifferWindow *captureWind;

    //Funciones extra
    void setupViews();
    void setupMenuBar();
    void setupToolBar();

};

#endif // MAINVIEWMANAGER_H
