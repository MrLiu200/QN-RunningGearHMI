#ifndef MYHELPER_H
#define MYHELPER_H

#include <QObject>
#include <QDateTime>
#define TIMEMS          qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#define DATETIME        qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
class MyHelper : public QObject
{
    Q_OBJECT
public:
    enum TabType{
        Log = 0,                //日志
        TrendChart = 1,         //历史趋势
        RealTime = 2,           //实时波形、频谱
        HostConfig = 3,         //本机设置
        RemoteConfig = 4        //远程设置
    };


};

#endif // MYHELPER_H
