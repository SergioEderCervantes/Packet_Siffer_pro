#ifndef MAINVIEWMANAGER_H
#define MAINVIEWMANAGER_H


#include <QMainWindow>
#include <QComboBox>
#include <QCloseEvent>
#include <QStackedWidget>

#include "views/deviceSelectionWindow.h"
#include "views/snifferWindow.h"
#include "views/queryViewWindow.h"
class mainViewManager : public QMainWindow {
    Q_OBJECT

public:
    explicit mainViewManager(QWidget *parent = nullptr);
    snifferWindow* getSnifferWindow();
    queryViewWindow *getQueryViewWindow();
    void setCurrentView(QWidget *view);
    QString getFilterType();
    int getIndexFilter();
private:
    //Contenedor principal
    QStackedWidget *mainContainer;
    QComboBox *filterType;
    QLineEdit *searchBox;
    QPushButton *searchButton;
    //Vistas agregadas
    DeviceSelectionWindow *devSelectionWind;
    snifferWindow *captureWind;
    queryViewWindow *queryWind;

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
