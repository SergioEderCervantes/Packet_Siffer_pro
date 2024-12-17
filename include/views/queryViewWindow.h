#ifndef QUERYVIEWWINDOW_H
#define QUERYVIEWWINDOW_H

#include <QWidget>
#include <QListView>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include "models/queryModel.h"
class queryViewWindow : public QWidget
{
    Q_OBJECT

public:
    explicit queryViewWindow(queryModel *model, QWidget *parent = nullptr);
    ~queryViewWindow();

signals:
    void executeQuery(const QString &query);

private slots:
    void onExecuteButtonClicked();
    void onQueryError(const QString &error);
    void onQueryResultReady(const QVector<QVector<QString>> &data, const QStringList &headers);


private:
    QListView *sidebarListView;      // ListView estilo sidebar
    QTextEdit *queryTextArea;        // Área de texto para escribir la consulta
    QPushButton *executeButton;      // Botón para ejecutar acciones
    QWidget *tableContainer;         // Contenedor vacío para insertar tablas dinámicamente

    queryModel *model;              //modelo


    void fillSidebar();
};

#endif // QUERYVIEWWINDOW_H
