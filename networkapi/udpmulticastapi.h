#ifndef UDPMULTICASTAPI_H
#define UDPMULTICASTAPI_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QFile>
#include "predata.h"
#define UDP_PACKET_HEAD1 0xA6
#define UDP_PACKET_HEAD2 0xA6
#define UDP_PACKET_END1  0x6A
#define UDP_PACKET_END2  0x6A
#include <QThread>
#include <QMutex>


#define UDP_DATA_OFFSET 4

class UDPMulticastAPI : public QThread
{
    Q_OBJECT
public:
    static UDPMulticastAPI *Instance();
    explicit UDPMulticastAPI(QObject *parent = nullptr);

private:
    static UDPMulticastAPI *self;
    QUdpSocket *udpServer;
    QHostAddress m_ip;
    bool IsOpen;
    QFile UpdateFile;
    bool Stopped;
    QMutex m_mutex;
    QByteArrayList DataList;
    void WriteDataToList(const QByteArray data);
    qint64 GetDataListSize();
    QByteArray ReadFirstDataFromList();

public:
    bool startJoin(void);
    bool Stop(void);

protected:
    void run() override;

private slots:
    void readData();
    void CheckData(const QByteArray data);
private:
    //发送数据
    void senddata(const QByteArray data);

    //生成发送数据包
    QByteArray GetPackage(uint8_t order, QString data);
    /**********************接收函数*********************/
    /*------------------------------------------------*/
    /*------------------------------------------------*/
    //收到自检内容
    void ReceiveSelfInspection(QString DataContent);

    //收到温度巡检
    void TemInspection(QString DataContent);

    //收到综合巡检
    void ComprehensiveInspection(QString DataContent);

    //收到心跳数据包
    void HeartbeatEvent(QString DataContent);

    //更新转速信息
    void UpdateSynchronous(QString DataContent);

    //板卡状态发生改变
    void BoardStatusEvent(QString DataContent);

    //更新返回
    void UpdateReturn(QString DataContent);

    //前置设备状态返回
    void DeviceStateReturn(QString DataContent);

    //轴承信息返回
    void BearingInfoReturn(QString DataContent);

    //定时任务返回
    void TimerTaskReturn(QString DataContent);

    //车辆信息返回
    void CarInfoReturn(QString DataContent);

    //网络信息返回
    void netInfoReturn(QString DataContent);

    //关联信息返回
    void LinkInfoReturn(QString DataContent);

    //数据保存返回
    void SaveIntervalReturn(QString DataContent);

    //背板状态返回
    void OtherBoardStateReturn(QString DataContent);

    //报警阀值返回
    void AlarmLimitReturn(QString DataContent);

    //软件版本返回
    void APPVersionReturn(QString DataContent);

    //错误指令返回
    void ErrorOrderReturn(QString DataContent);
public:
    /**********************设置函数*********************/
    /*------------------------------------------------*/
    /*------------------------------------------------*/
    //下发校时
    void Time_Aligned(void);

    //下发自检任务
    void StartSelfInspection(QString WagonName = "all",QString preID = "0");

    //上位机调试部分指令

    //更新指令
    void UpdateSoftware(QString wagon,int devicenum,QString filename,QString version,int devicech = 0);

    //设备重启
    void DeviceReboot(QString wagon);

    //更新轴承信息
    void UpdateBearing(QString wagon, uint8_t order, QString model, float PitchDiameter, float RollingDiameter, int RollerNum, float ContactAngle);

    //更新前置处理器信息
    void UpdatePre(QString wagon, uint8_t order,uint8_t id, uint8_t ch, QString type, float Sensitivity, float ShaftDiameter,
                   QString Bearing1_name, QString Bearing1_model, QString Bearing2_name, QString Bearing2_model,
                   QString Bearing3_name, QString Bearing3_model, QString Bearing4_name, QString Bearing4_model,
                   QString capstanName, int capstanTeethNum, QString DrivenwheelName, int DrivenwheelTeethNum,
                   QString version, bool IsEnable);
    //更新前置处理器信息
    void UpdatePre(QString wagon, uint8_t order, QStringList Preconfig);

    //更新定时任务
    void UpdateTimerTask(QString wagon, uint8_t order, QString task, QString triggertime);

    //更新车辆信息
    void UpdateCarInfo(QString projectname, QString wagon, QString Linenum, QString carnum, float WheelDiameter, QString type, double Coefficient);

    //更新网络信息
    void UpdateNetWorkConfing(QString wagon, QString localip0, QString localmask0, QString gateway0, QString localdns0,
                              QString localip1, QString localmask1, QString gateway1, QString localdns1,
                              QString tcpserverip, int tcpserverport, int LocallistenPort, QString tcpstandbyip, int tcpstandbyport,
                              QString UDPMulticastIP, int UDPListenPort);

    //更新主/从机关联信息
    void UpdateLinkDevice(QString wagon, uint8_t order, QString type, QString ip, int port, QString linkwagon);

    //更新数据保存间隔
    void UpdateSaveInterval(QString wagon, int lowpowerspeedmax, int temspeedmax, bool Isenable, int VIBInterval, int TemInterval);

    //更新报警阈值信息
    void UpdateAlarmLimit(QString wagon,QStringList values);

    //删除指定文件
    void UpdateZIPFile(QString wagon,QString filename);

    //设置报警指示灯(消警)
    void UpdateAlarmLed(QString wagon,qint8 id,qint8 ch);

    //测试新版修改测点功能
    void TestAddPre(QString wagon,QString str);

    /**********************读取函数*********************/
    /*------------------------------------------------*/
    /*------------------------------------------------*/
    //获取前置处理器状态
    void GetPreStateList(QString wagon);

    //获取轴承信息
    void GetBearingInfo(QString wagon,QString bearing_model);

    //获取定时任务信息
    void GetTimerTask(QString wagon);

    //获取车辆信息
    void GetCarConfig(QString wagon);

    //获取网络信息
    void GetNetworkConfig(QString wagon);

    //获取关联主从机信息
    void GetLinkDeviceInfo(QString wagon);

    //获取数据保存间隔
    void GetSaveInterval(QString wagon);

    //获取背板状态
    void GetOtherBoardState(QString wagon);

    //获取报警阀值
    void GetAlarmLimit(QString wagon);

    //获取软件版本信息
    void GetVersion(QString wagon);
signals:
    //设备上线
    void DeviceGoOnline(QString wagon);
    //报警信号
    void DeviceAlarm(QString wagon, int id, int ch, quint8 grade, QString alarmtype);
    //设备自检异常
    void DeviceSelfCheckAbnomal(QString wagon,int id, int ch);
    //更新异常
    void DeviceUpdateAbnomal(QString wagon);
    //前置状态改变
    void PreStateChange(QString wagon, QString prename, PreData::EnumDeviceState state);
    //轴承信息返回
    void BearingReturn(QString wagon,QStringList bearingInfo);
    //定时任务返回
    void taskReturn(QString wagon,QStringList tasklist);
    //车辆信息返回
    void carinforeturn(QString project, int linenum, QString carnum, QString wagon,float WheelDiameter,QString type,double sppedcoefficient);
    //网络信息返回
    void networkconfigreturn(QString wagon,QStringList configlist);
    //关联信息返回
    void linkconfigrereturn(QString wagon,QStringList linklist);
    //保存间隔返回
    void DatasaveIntervalreturn(QString wagon,int lowpowerspeedmax, int temspeedmax, bool IsEnable, int VibInterval, int TemInterval);
    //背板状态返回
    void OtherBoardState(QString wagon,QStringList statelist);
    //报警阀值
    void Alarmlimit(QString wagon,QStringList statelist);
    //指令出错返回
    void ErrorReturn(QString wagon, uint8_t cmd);
    //更新进程
    void UpdateProgress(int currentcount);
    //软件版本返回
    void Versionreturn(QString wagon,QString version);
    //以下为实时显示界面所需信号
    //量纲值返回
    void ReturnEigenvalue(QString Wagon, qint8 id, qint8 ch, quint32 speed, double AmbientTem,double PointTem,QString time,
                          QStringList Dimensional,QStringList Demodulated,QStringList alarmlist);
    //实时转速更新
    void RealtimeSpeedUpdate(quint32 speed);

    //温度巡检返回
    void ReturnTemValue(QString Wagon, qint8 id,QString time,qint8 ChannelCount,QStringList State);
};

#endif // UDPMULTICASTAPI_H
