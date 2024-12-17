#ifndef QUERYMODEL_H
#define QUERYMODEL_H
#include <QObject>


class queryModel : public QObject
{
    Q_OBJECT
public:
    explicit queryModel(QObject *parent = nullptr);
    QVector<QString> getTableNames() const;
private:
    QVector<QString> tableNames;

    void fetchTableNames();
public slots:
    void onExecuteQuery(const QString &query);

signals:
    void queryError(const QString &errorMessage);
    void queryResultReady(const QVector<QVector<QString>> &data, const QStringList &headers);
};
#endif // QUERYMODEL_H
