#ifndef MAINVIEWMANAGER_H
#define MAINVIEWMANAGER_H


#include <QMainWindow>
#include <QComboBox>

#include <QStackedWidget>
#include "views/deviceSelectionWindow.h"
#include "views/snifferWindow.h"
class mainViewManager : public QMainWindow {
    Q_OBJECT

public:
    explicit mainViewManager(QWidget *parent = nullptr);
    snifferWindow* getSnifferWindow();
    void setCurrentView(QWidget *view);
private:
    //Contenedor principal
    QStackedWidget *mainContainer;
     QComboBox *filterType;
    //Vistas agregadas
    DeviceSelectionWindow *devSelectionWind;
    snifferWindow *captureWind;

    //Funciones extra
    void setupViews();
    void setupMenuBar();
    void setupToolBar();
    void handleExitFromCapture();
    void saveCapturedData();

};

#endif // MAINVIEWMANAGER_H
