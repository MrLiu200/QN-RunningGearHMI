#ifndef PREDATA_H
#define PREDATA_H

#include <QObject>

class PreData : public QObject
{
    Q_OBJECT
public:
    enum EnumDeviceState{//设备状态
        DeviceState_Online = 0,
        DeviceState_Offline,
        DeviceState_Waring,
        DeviceState_FirstAlarm,
        DeviceState_Secondary,
        DeviceState_Fault
    };
    struct BEARINGINFO{//轴承信息
        QString Wagon;
        QString Model;          //型号
        float PitchDiameter;    //中径
        float RollingDiameter;  //滚径
        int RollerNum;          //滚子数
        float ContactAngle;     //接触角
    };

    struct DEVICEINFO{//设备信息
        QString Wagon;
        PreData::EnumDeviceState state;
        uint8_t id;
        uint8_t ch;
        QString name;
        QString type;
        float sensitivity;
        float shaftDiameter;
        QString bearing1_name;
        QString bearing1_model;
        QString bearing2_name;
        QString bearing2_model;
        QString bearing3_name;
        QString bearing3_model;
        QString bearing4_name;
        QString bearing4_model;
        QString capstanname;
        int capstanTeethNum;
        QString drivenwheelName;
        int drivenwheelTeethNum;
        QString version;
        bool IsEnable;
    };

    struct PKDATASTRUCT{//对比信息
        QString Wagon;
        uint8_t id;
        uint8_t ch;
        QString name;
        QString time;
        QVector<double> values;
        bool operator==(const PKDATASTRUCT &other) const {
            return id == other.id && ch == other.ch && time == other.time &&
                   Wagon == other.Wagon && name == other.name;
        }
    };

    //设备信息链表
    static QList<BEARINGINFO> bearinglist;
    static QList<DEVICEINFO> devicelist;
    static QList<QString> wagonsList;

    //设备状态链表
    //格式：车厢号|状态|最后一次通信时间 状态：0正常，1离线
    static QList<QString> DeviceState;
    static int DeviceInfo_Count;
    static QList<QString> DeviceInfo_WagonName;             //车厢名称
    static QList<int> DeviceInfo_DeviceID;                  //前置ID
    static QList<int> DeviceInfo_DeviceCH;                  //通道
    static QList<QString> DeviceInfo_DeviceName;            //名称
    static QList<QString> DeviceInfo_DeviceType;            //测点类型
    static QList<float> DeviceInfo_Sensitivity;             //测点灵敏度
    static QList<float> DeviceInfo_ShaftDiameter;           //测点所在轴轴径

    static QList<QString> DeviceInfo_bearing1Name;          //轴承1名称
    static QList<QString> DeviceInfo_bearing1_model;        //轴承1型号
    static QList<QString> DeviceInfo_bearing2Name;          //轴承2名称
    static QList<QString> DeviceInfo_bearing2_model;        //轴承2型号
    static QList<QString> DeviceInfo_bearing3Name;          //轴承3名称
    static QList<QString> DeviceInfo_bearing3_model;        //轴承3型号
    static QList<QString> DeviceInfo_bearing4Name;          //轴承4名称
    static QList<QString> DeviceInfo_bearing4_model;        //轴承4型号

    static QList<QString> DeviceInfo_capstanName;           //主动轮名称
    static QList<int> DeviceInfo_capstanTeethNum;           //主动轮齿数
    static QList<QString> DeviceInfo_DrivenwheelName;       //从动轮名称
    static QList<int> DeviceInfo_DrivenwheelTeethNum;       //从动轮齿数
    static QList<QString> DeviceInfo_version;               //设备版本
    static QList<bool> DeviceInfo_IsEnable;                 //设备使能

    static QList<PKDATASTRUCT> pkdatalist;                  //数据对比列表
    static int PKListSizeMax;                               //数据对比列表最大值

signals:

};

#endif // PREDATA_H
