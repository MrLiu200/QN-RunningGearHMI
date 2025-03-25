#include "m_dbapi.h"
#include <QMessageBox>
M_DbAPI *M_DbAPI::self = 0;
M_DbAPI *M_DbAPI::Instance()
{
    if(!self){
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if(!self){
            self = new M_DbAPI;
        }
    }
    return self;
}

M_DbAPI::M_DbAPI(QObject *parent) : QObject(parent)
{
    LocalDBIsConnect = false;
    DataDBIsConnect = false;
    connect(M_NetworkAPI::Instance(),&M_NetworkAPI::ReturnDBFile,this,&M_DbAPI::ReceiveDbFile);
}

bool M_DbAPI::ConnectLocalDB()
{
    //判断本地数据库文件是否存在，若不存在则重新拷贝一份
    App::LocalDataBaseFile = CoreHelper::APPPath() + "/HMI.db";
    QFile dbFile(App::LocalDataBaseFile);
    if (!dbFile.exists() || dbFile.size() == 0) {
        qDebug()<<"get new sql file";
        dbFile.remove();
        CoreHelper::copyFile(":/db/RunningGear.db", App::LocalDataBaseFile);
    }
    //连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","Local");
    db.setDatabaseName(App::LocalDataBaseFile);
    bool ok = db.open();
    if(!ok){
          qDebug() << "数据库连接失败：" << db.lastError().text();
    }
    LocalDBIsConnect = ok;
    return ok;
}

bool M_DbAPI::ConnectDataDB(QString filename)
{
    //连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","DataDB");
    db.setDatabaseName(filename);
    bool ok = db.open();
    if(!ok){
          qDebug() << "数据库连接失败：" << db.lastError().text();
    }else{
        qDebug()<<"connect Data DB ok";
    }
    DataDBIsConnect = ok;
    return ok;
}

QSqlDatabase M_DbAPI::GetLocalDBHandle()
{
    return QSqlDatabase::database("Local");
}

QSqlDatabase M_DbAPI::GetDataDBHandle()
{
    return QSqlDatabase::database("DataDB");
}

bool M_DbAPI::GetLocalConnectState()
{
    return LocalDBIsConnect;
}

bool M_DbAPI::GetDataConnectState()
{
    return DataDBIsConnect;
}

void M_DbAPI::AddLoaclLog(QString action, QString time, QString content)
{
    QSqlQuery query(GetLocalDBHandle());
    QString addsql = "INSERT INTO LogInfo (User, Action, TriggerTime, LogContent) VALUES (?, ?, ?, ?)";
    query.prepare(addsql);
    query.bindValue(0,App::CurrentUser);
    query.bindValue(1,action);
    query.bindValue(2,time);
    query.bindValue(3,content);
    query.exec();
//    qDebug() << "SQL:" << addsql;
//    qDebug() << "Bound values:" << App::CurrentUser << action << time << content;
}

void M_DbAPI::ReceiveDbFile(QString name)
{
    if(!ConnectDataDB(name)){
        QMessageBox::warning(nullptr,"数据库错误","open Data datebase fail");
        Q_EMIT OpenDataDB(false);
    }else{
        Q_EMIT OpenDataDB(true);
    }
}
