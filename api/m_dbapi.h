#ifndef M_DBAPI_H
#define M_DBAPI_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "m_networkapi.h"
class M_DbAPI : public QObject
{
    Q_OBJECT
public:
    static M_DbAPI *Instance();
    explicit M_DbAPI(QObject *parent = nullptr);

private:
    static M_DbAPI *self;
    bool LocalDBIsConnect;
    bool DataDBIsConnect;

public:
    bool ConnectLocalDB();
    bool ConnectDataDB(QString filename);
    QSqlDatabase GetLocalDBHandle();
    QSqlDatabase GetDataDBHandle();
    bool GetLocalConnectState();
    bool GetDataConnectState();

    void AddLoaclLog(QString action,QString time,QString content);
private slots:
public slots:
    void ReceiveDbFile(QString name);
signals:
    void OpenDataDB(bool ok);
};

#endif // M_DBAPI_H
