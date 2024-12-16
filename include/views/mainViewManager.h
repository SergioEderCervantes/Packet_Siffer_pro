#ifndef MAINVIEWMANAGER_H
#define MAINVIEWMANAGER_H


#include <QMainWindow>
#include <QComboBox>

#include <QStackedWidget>
#include "views/deviceSelectionWindow.h"
#include "views/snifferWindow.h"
#include <QCloseEvent>
class mainViewManager : public QMainWindow {
    Q_OBJECT

public:
    explicit mainViewManager(QWidget *parent = nullptr);
    snifferWindow* getSnifferWindow();
    void setCurrentView(QWidget *view);
    QString getFilterType();
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
    void handleSaveAndExit();
    void handleDiscardAndExit();
    void resetFilters();
private slots:
    void handleExitFromCapture(QCloseEvent *event = nullptr);
protected:
    void closeEvent(QCloseEvent *event) override;
signals:
    void deleteTable();
    void killThread();
    void clear();
    void requestExit(QCloseEvent *event = nullptr);
};

#endif // MAINVIEWMANAGER_H
