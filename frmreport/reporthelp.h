#ifndef REPORTHELP_H
#define REPORTHELP_H

#include <QObject>

class ReportHelp : public QObject
{
    Q_OBJECT
public:
    struct PDFDEVICE{//设备信息
        QString Wagon;
        uint8_t id;
        uint8_t ch;
        QString name;
        QString type;
        int AxisPosition;
        int alarmgrade;
    };
};


#endif // REPORTHELP_H
