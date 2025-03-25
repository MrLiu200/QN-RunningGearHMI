#include "udpmulticastapi.h"
#include <QMutex>
#include <QNetworkDatagram>
#include <QFile>
#include <QSqlError>
#include <QMessageBox>
#include "corehelper.h"
#include "app.h"

UDPMulticastAPI *UDPMulticastAPI::self = 0;
UDPMulticastAPI *UDPMulticastAPI::Instance()
{
    if(!self){
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if(!self){
            self = new UDPMulticastAPI;
        }
    }
    return self;
}

UDPMulticastAPI::UDPMulticastAPI(QObject *parent) : QThread(parent)
{
    IsOpen = false;
    m_ip = QHostAddress(App::UDPMulticastIP);
    udpServer = new QUdpSocket;
    Stopped = false;
    connect(udpServer,&QUdpSocket::readyRead,this,&UDPMulticastAPI::readData);
}

void UDPMulticastAPI::WriteDataToList(const QByteArray data)
{
    QMutexLocker lock(&m_mutex);
    DataList.append(data);
}

qint64 UDPMulticastAPI::GetDataListSize()
{
    QMutexLocker lock(&m_mutex);
    return DataList.size();
}

QByteArray UDPMulticastAPI::ReadFirstDataFromList()
{
    QMutexLocker lock(&m_mutex);
    QByteArray returnArray;
    if(!DataList.isEmpty()){
        returnArray = DataList.takeFirst();
    }
    return returnArray;
}

bool UDPMulticastAPI::startJoin()
{
#if 1
    QHostAddress specificInterface(App::UdpListenIP);
    bool ret = udpServer->bind(specificInterface,App::UDPListenPort,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
#else
    bool ret = udpServer->bind(QHostAddress::AnyIPv4,App::UDPListenPort,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
#endif

    if(ret){
        ret = udpServer->joinMulticastGroup(m_ip);
        if(ret){
            udpServer->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);
            IsOpen = true;
            QThread::start();
        }
//        QByteArray date;
//        date.append("Welcom to HMI******");
//        senddata(date);
//        GetCarConfig("QN-t0021");

        qDebug() << "Bound to address:" << udpServer->localAddress().toString();
            qDebug() << "Bound to port:" << udpServer->localPort();
    }
    return ret;
}

bool UDPMulticastAPI::Stop()
{
    bool ret = udpServer->leaveMulticastGroup(m_ip);
    if(ret){
        udpServer->abort();
        IsOpen = false;
    }
    return ret;

}

void UDPMulticastAPI::run()
{
    while (!Stopped) {
        if(GetDataListSize() > 0){
//            qDebug()<<"run******************";
            QByteArray buffer = ReadFirstDataFromList();
            CheckData(buffer);
        }
    }
}

void UDPMulticastAPI::senddata(const QByteArray data)
{
    if(IsOpen){
        udpServer->writeDatagram(data,m_ip,App::UDPListenPort);
//        qDebug()<< "udp send " << CoreHelper::byteArrayToHexStr(data);
    }
}

void UDPMulticastAPI::readData()
{
    QNetworkDatagram datagrm = udpServer->receiveDatagram();
//    CheckData(datagrm.data());
    QByteArray data = datagrm.data();
    WriteDataToList(data);
}

void UDPMulticastAPI::CheckData(const QByteArray data)
{
    int len = data.length();
    if(len == 0){
        return;
    }
//    qDebug()<< DATETIMES << ":udp Receive " << CoreHelper::byteArrayToHexStr(data);
    uint8_t frame[len];

    for(int i=0; i<len; i++){
        frame[i] = data[i];
    }

    //判断帧头帧尾若非主从机通信数据则不处理
    if((frame[0] == UDP_PACKET_HEAD1) && (frame[1] == UDP_PACKET_HEAD2) && (frame[len-1] == UDP_PACKET_END1) &&(frame[len-2] == UDP_PACKET_END2)){
        //校验和判断
        uint8_t oldcrc = frame[len-3];
        uint8_t newcrc = CoreHelper::CheckSum(frame,2,len-3);
        if(oldcrc == newcrc){
            uint8_t order = frame[2];
            uint16_t datalen = frame[3]<<8 | frame[4];
            QString DataContent = QString::fromUtf8(data.mid(5,datalen));
            switch (order) {
            case 0x03://接收到自检结果
                ReceiveSelfInspection(DataContent);
                break;
            case 0x04://收到温度巡检结果
                TemInspection(DataContent);
                break;
            case 0x05://收到综合巡检结果
                ComprehensiveInspection(DataContent);
                break;
            case 0x06://收到心跳数据包
                HeartbeatEvent(DataContent);
                break;
            case 0x07://收到更新转速信息
                UpdateSynchronous(DataContent);
                break;
            case 0x08://设备状态发生改变
                BoardStatusEvent(DataContent);
                break;
            case 0x11://更新返回
                UpdateReturn(DataContent);
                break;
            case 0x80://前置设备状态返回
                DeviceStateReturn(DataContent);
                break;
            case 0x81://轴承信息返回
                BearingInfoReturn(DataContent);
                break;
            case 0x82://定时任务
                TimerTaskReturn(DataContent);
                break;
            case 0x83://车辆信息
                CarInfoReturn(DataContent);
                break;
            case 0x84://网络信息
                netInfoReturn(DataContent);
                break;
            case 0x85://关联设备
                LinkInfoReturn(DataContent);
                break;
            case 0x86://数据保存间隔
                SaveIntervalReturn(DataContent);
                break;
            case 0x87://背板状态
                OtherBoardStateReturn(DataContent);
                break;
            case 0x88://报警阈值
                AlarmLimitReturn(DataContent);
                break;
            case 0x89://软件版本
                APPVersionReturn(DataContent);
                break;
            case 0xFF://错误指令
                ErrorOrderReturn(DataContent);
                break;
            default:
                break;
            }
        }
    }
}

QByteArray UDPMulticastAPI::GetPackage(uint8_t order, QString data)
{

    QByteArray messageData = data.toUtf8();
    uint16_t len = messageData.size();
    QByteArray array;
    array.append(UDP_PACKET_HEAD1);
    array.append(UDP_PACKET_HEAD2);
    array.append(order);
    array.append((len >> 8) & 0xff);
    array.append(len & 0xff);
    array.append(messageData);
    len = array.size();
    array.append(CoreHelper::CheckSum(array,2,len));
    array.append(UDP_PACKET_END1);
    array.append(UDP_PACKET_END2);
    return array;
}

void UDPMulticastAPI::ReceiveSelfInspection(QString DataContent)
{
    QStringList list = DataContent.split(";");
    QString WagonNum = list.at(0);
    QString time = list.at(1);
    uint8_t pre_id = list.at(2).toInt();
    uint8_t pre_state = list.at(3).toInt();
    uint8_t ChannelCount = list.at(4).toInt();
    if(ChannelCount <= 0) return;
    uint8_t ChannelState[ChannelCount];
    for(int i=5;i<list.size();i++){
        ChannelState[i-5] = list.at(i).toInt();
    }

    //判断前置主体状态
    if(pre_state == 1){
       Q_EMIT DeviceSelfCheckAbnomal(WagonNum,0,0);
    }
    //判断各通道状态
    for(int i=0;i<ChannelCount;i++){
        if(ChannelState[i] == 1){
            Q_EMIT DeviceSelfCheckAbnomal(WagonNum,pre_id,i+1);
        }
    }
}

void UDPMulticastAPI::TemInspection(QString DataContent)
{
    //取出对应数据
    QStringList list = DataContent.split(";");
    QString WagonNum = list.at(0);
    QString time = list.at(1);
    uint8_t pre_id = list.at(2).toInt();
    uint8_t ChannelCount = list.at(3).toInt();
    if(ChannelCount <= 0) return;
    uint8_t ChannelState[ChannelCount];
    for(int i=4;i<list.size();i++){
        ChannelState[i-4] = list.at(i).toInt();
    }
    QStringList Statelist;
    //这里应该更新TRDP或MVB等标志位---LMG
    //判断各通道状态
    for(int i=0;i<ChannelCount;i++){
        Statelist.append(QString::number(ChannelState[i]));
        if(ChannelState[i] > 0){
            Q_EMIT DeviceAlarm(WagonNum,pre_id,i+1,ChannelState[i]-1,"温度");
        }
    }
    Q_EMIT ReturnTemValue(WagonNum,pre_id,time,ChannelCount,Statelist);
}

void UDPMulticastAPI::ComprehensiveInspection(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.size() < 11) return;
    QString WagonNum = list.at(0);
    QString time = list.at(1);
    uint8_t pre_id = list.at(2).toInt();
    uint8_t ChannelNum = list.at(3).toInt();
    uint32_t Speed = list.at(4).toInt();
    double AmbientTem = list.at(5).toDouble();
    double pointTem = list.at(6).toDouble();
    QStringList alarmlist = list.at(8).split("|");
    QStringList DimensionList = list.at(9).split("|");
    QStringList DemodulatedList = list.at(10).split("|");
    uint8_t state[9];
    state[0] = list.at(7).toInt();
    for(int i=0;i<alarmlist.size();i++){
        state[i+1] = alarmlist.at(i).toInt();
    }

    //判断状态
    QStringList Contents;
    Contents << "温度" << "保外" << "保内" << "外环" << "内环" << "滚单" << "大齿轮" << "小齿轮" << "踏面";
    for(int i=0;i<9;i++){
        if(state[i] != 0xff){//0:预警，1:一级报00警 2:二级报警
            QString content = Contents.at(i);
//            qDebug()<<"**********-------------" << content << "-------**********";
            Q_EMIT DeviceAlarm(WagonNum,pre_id,ChannelNum,state[i],content);
        }
    }
    QStringList alarminfo;//仅传给实时界面使用
    for(auto baojing : state){
        alarminfo.append(QString::number(baojing));
    }
    Q_EMIT ReturnEigenvalue(WagonNum,pre_id,ChannelNum,Speed,AmbientTem,pointTem,time,DimensionList,DemodulatedList,alarminfo);
}

void UDPMulticastAPI::HeartbeatEvent(QString DataContent)
{//心跳只负责在线与否，不管是否报警
    for(int i=0;i<PreData::DeviceState.size();i++){
        //数据格式：车厢号|当前状态|最后一个心跳时间
        QStringList list  = PreData::DeviceState.at(i).split("|");
        QString OldWagonNumber = list.at(0);
        int OldState = list.at(1).toInt();
        if(DataContent == OldWagonNumber){
            QString str = QString("%1|0|%2").arg(OldWagonNumber).arg(DATETIME);
            PreData::DeviceState[i] = str;
            if(OldState == 1){//设备上线
                Q_EMIT DeviceGoOnline(OldWagonNumber);
            }
            break;
        }
    }
}

void UDPMulticastAPI::UpdateSynchronous(QString DataContent)
{
    quint32 runspeed = DataContent.toUInt();
    Q_EMIT RealtimeSpeedUpdate(runspeed);
}

void UDPMulticastAPI::BoardStatusEvent(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.size() < 6) return;
    QString wagon = list.at(0);
    QString type = list.at(1);
    QString strid = list.at(2);
//    QString oldContent = list.at(3);
    int state = list.at(4).toInt();
    QString Time = list.at(5);
    int id =0,ch =0;
    if(type == "前置"){
        id = strid.split("|").at(0).toInt();
        ch = strid.split("|").at(1).toInt();
        //查找通道名称
        QString name;
        for(auto device : PreData::devicelist){
            if(device.id == id && device.ch == ch){
                name = device.name;
                break;
            }
        }
        if(!name.isEmpty()){
            Q_EMIT PreStateChange(wagon,name,static_cast<PreData::EnumDeviceState>(state));
        }
    }
}

void UDPMulticastAPI::UpdateReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    QString wagon = list.at(0);
    int state = list.at(1).toInt();
    if(state == 1){
        Q_EMIT DeviceUpdateAbnomal(wagon);
    }
}

void UDPMulticastAPI::DeviceStateReturn(QString DataContent)
{
    //创建一个QHash存储新的状态,键值为wagon_id_ch
    QHash<QString, PreData::EnumDeviceState> updatedDeviceStates;

    //取出对应的列表
    QStringList list = DataContent.split(";");
    QString wagon = list.at(0);
    for(int i=1;i<list.size();i++){
        QStringList state = list.at(i).split("|");
        //取出最大的状态
        int alarm = state.at(3).toInt() > state.at(4).toInt() ? state.at(3).toInt() : state.at(4).toInt();
        PreData::EnumDeviceState newstate = static_cast<PreData::EnumDeviceState>(alarm);
        QString key = state.at(0) + "_" + state.at(1) + "_" + state.at(2);
        updatedDeviceStates.insert(key,newstate);
    }
    //更新列表
    int len = PreData::devicelist.size();
    for(int i=0;i<len;i++){
        auto &device = PreData::devicelist.at(i);
        QString key = QString("%1_%2_%3").arg(device.Wagon).arg(device.id).arg(device.ch);
        if(updatedDeviceStates.contains(key)){
            PreData::devicelist[i].state = updatedDeviceStates.value(key);
            Q_EMIT PreStateChange(device.Wagon,device.name,updatedDeviceStates.value(key));
        }
    }
}

void UDPMulticastAPI::BearingInfoReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.size() != 6) return;

    QString wagon = list.at(0);
    list.removeFirst();

    Q_EMIT BearingReturn(wagon,list);
}

void UDPMulticastAPI::TimerTaskReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    QString wagon = list.at(0);
    list.removeFirst();
    Q_EMIT taskReturn(wagon,list);

}

void UDPMulticastAPI::CarInfoReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.size() != 8) return;
    QString wagon = list.at(0);
    QString project = list.at(1);
    int line = list.at(2).toInt();
    QString car = list.at(3);
    float wheel = list.at(5).toFloat();
    QString wagontype = list.at(6);
    double coefficient = list.at(7).toDouble();
    Q_EMIT carinforeturn(project,line,car,wagon,wheel,wagontype,coefficient);
}

void UDPMulticastAPI::netInfoReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.isEmpty())return;
    QString wagon = list.at(0);
    list.removeFirst();
    Q_EMIT networkconfigreturn(wagon,list);
}

void UDPMulticastAPI::LinkInfoReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.isEmpty())return;
    QString wagon = list.at(0);
    list.removeFirst();
    Q_EMIT linkconfigrereturn(wagon,list);
}

void UDPMulticastAPI::SaveIntervalReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.size() != 6)return;
    QString wagon = list.at(0);
    int lowpowertaskspeed = list.at(1).toInt();
    int tmptaskspeed = list.at(2).toInt();
    bool enable = list.at(3).toInt();
    int vib = list.at(4).toInt();
    int tmp = list.at(5).toInt();
    Q_EMIT DatasaveIntervalreturn(wagon,lowpowertaskspeed,tmptaskspeed,enable,vib,tmp);
}

void UDPMulticastAPI::OtherBoardStateReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.isEmpty())return;
    QString wagon = list.at(0);
    list.removeFirst();
    Q_EMIT OtherBoardState(wagon,list);
}

void UDPMulticastAPI::AlarmLimitReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.isEmpty())return;
    QString wagon = list.at(0);
    list.removeFirst();
    Q_EMIT Alarmlimit(wagon,list);
}

void UDPMulticastAPI::APPVersionReturn(QString DataContent)
{
    //只有软件版本
    QStringList list = DataContent.split(";");
    if(list.isEmpty()|| (list.size()!=2))return;
    QString wagon = list.at(0);
    QString version = list.at(1);
    Q_EMIT Versionreturn(wagon,version);
}

void UDPMulticastAPI::ErrorOrderReturn(QString DataContent)
{
    QStringList list = DataContent.split(";");
    if(list.isEmpty())return;
    QString wagon = list.at(0);
    uint8_t order = list.at(1).toInt();
    Q_EMIT ErrorReturn(wagon,order);
}

void UDPMulticastAPI::Time_Aligned()
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss");
    QByteArray array = GetPackage(0x01,time);
    senddata(array);
}

void UDPMulticastAPI::StartSelfInspection(QString WagonName, QString preID)
{
    QString data = WagonName + ";" + preID;
    QByteArray array = GetPackage(0x02,data);
    senddata(array);
}

void UDPMulticastAPI::UpdateSoftware(QString wagon, int devicenum, QString filename, QString version, int devicech)
{
    quint32 crc = CoreHelper::GetCRC32_STM32H750(filename);
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(nullptr,"文件错误","文件打开失败");
        return;
    }
    //前置处理器需要带通道号
    QString device = QString::number(devicenum);
    if(devicenum == 1){
        device = QString("%1|%2").arg(devicenum).arg(devicech);
    }

    //求出总包数
    const int packetSize = 1024;
    qint64 totalBytes = file.size();
    int totalPackets = (totalBytes + packetSize - 1) / packetSize; // 总包数，向上取整

    QString currentpage = "1";

    QStringList descriptionlist;
    descriptionlist.append(wagon);
    descriptionlist.append(device);
    descriptionlist.append(currentpage);
    descriptionlist.append(QString::number(totalPackets));
    descriptionlist.append(version);
//    descriptionlist.append("2852050767");
    descriptionlist.append(QString::number(crc));
    QString description = descriptionlist.join(";");
//    QString description = QString("%1;%2;%3;%4;%5;%6;")
//            .arg(wagon).arg(device).arg(currentpage).arg(QString::number(totalPackets))
//            .arg(version).arg(QString::number(crc));

    QByteArray fileData;
    while(!file.atEnd()){
        fileData = file.read(packetSize);

        QByteArray base64Data = fileData.toBase64();
        QByteArray package = GetPackage(0x11, description +";"+ QString(base64Data));
        senddata(package);
        qDebug()<<"currentpage = " << currentpage;
        // 更新当前计数
        currentpage = QString::number(currentpage.toInt() + 1);
        descriptionlist.replace(2,currentpage);
//        description = description.replace(4,1,currentpage); // 更新包描述
        description = descriptionlist.join(";");
//        QThread::sleep(1);
        QThread::msleep(1);
        Q_EMIT UpdateProgress(currentpage.toInt());
    }
    file.close();
}

void UDPMulticastAPI::DeviceReboot(QString wagon)
{
    if(wagon.isEmpty()) return;

    QByteArray array = GetPackage(0x12,wagon);
    senddata(array);
}

void UDPMulticastAPI::UpdateBearing(QString wagon, uint8_t order, QString model, float PitchDiameter, float RollingDiameter, int RollerNum, float ContactAngle)
{
    if(order > 2) return;
    QStringList list;
    list.append(wagon);
    list.append(QString::number(order));
    list.append(model);
    list.append(QString::number(PitchDiameter));
    list.append(QString::number(RollingDiameter));
    list.append(QString::number(RollerNum));
    list.append(QString::number(ContactAngle));
    QByteArray array = GetPackage(0x13,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::UpdatePre(QString wagon, uint8_t order, uint8_t id, uint8_t ch, QString type, float Sensitivity, float ShaftDiameter,
                                QString Bearing1_name, QString Bearing1_model, QString Bearing2_name, QString Bearing2_model, QString Bearing3_name,
                                QString Bearing3_model, QString Bearing4_name, QString Bearing4_model, QString capstanName, int capstanTeethNum,
                                QString DrivenwheelName, int DrivenwheelTeethNum, QString version, bool IsEnable)
{
    if(order > 2) return;
    QStringList list;
    list.append(wagon);
    list.append(QString::number(order));
    list.append(QString::number(id));
    list.append(QString::number(ch));
    list.append(type);
    list.append(QString::number(Sensitivity));
    list.append(QString::number(ShaftDiameter));
    list.append(Bearing1_name);
    list.append(Bearing1_model);
    list.append(Bearing2_name);
    list.append(Bearing2_model);
    list.append(Bearing3_name);
    list.append(Bearing3_model);
    list.append(Bearing4_name);
    list.append(Bearing4_model);
    list.append(capstanName);
    list.append(QString::number(capstanTeethNum));
    list.append(DrivenwheelName);
    list.append(QString::number(DrivenwheelTeethNum));
    list.append(version);
    list.append(QString::number(IsEnable));

    QByteArray array = GetPackage(0x14,list.join(";"));
    senddata(array);

}

void UDPMulticastAPI::UpdatePre(QString wagon, uint8_t order, QStringList Preconfig)
{
    if(order > 2) return;
    QStringList list;
    list.append(wagon);
    list.append(QString::number(order));
    list.append(Preconfig);

    QByteArray array = GetPackage(0x14,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::UpdateTimerTask(QString wagon, uint8_t order, QString task, QString triggertime)
{
    if(order > 2) return;
    QStringList list;
    list.append(wagon);
    list.append(QString::number(order));
    list.append(task);
    list.append(triggertime);

    QByteArray array = GetPackage(0x15,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::UpdateCarInfo(QString projectname, QString wagon, QString Linenum, QString carnum, float WheelDiameter, QString type, double Coefficient)
{
    QStringList list;
    list.append(wagon);
    list.append(projectname);
    list.append(Linenum);
    list.append(carnum);
    list.append(wagon);
    list.append(QString::number(WheelDiameter));
    list.append(type);
    list.append(QString::number(Coefficient));
    QByteArray array = GetPackage(0x16,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::UpdateNetWorkConfing(QString wagon, QString localip0, QString localmask0, QString gateway0, QString localdns0, QString localip1, QString localmask1, QString gateway1, QString localdns1,
                                           QString tcpserverip, int tcpserverport, int LocallistenPort, QString tcpstandbyip,
                                           int tcpstandbyport, QString UDPMulticastIP, int UDPListenPort)
{
    QStringList list;
    list.append(wagon);
    list.append(localip0);
    list.append(localmask0);
    list.append(gateway0);
    list.append(localdns0);
    list.append(localip1);
    list.append(localmask1);
    list.append(gateway1);
    list.append(localdns1);
    list.append(tcpserverip);
    list.append(QString::number(tcpserverport));
    list.append(QString::number(LocallistenPort));
    list.append(tcpstandbyip);
    list.append(QString::number(tcpstandbyport));
    list.append(UDPMulticastIP);
    list.append(QString::number(UDPListenPort));

    QByteArray array = GetPackage(0x17,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::UpdateLinkDevice(QString wagon, uint8_t order, QString type, QString ip, int port, QString linkwagon)
{
    if(order > 2) return;
    QStringList list;
    list.append(wagon);
    list.append(QString::number(order));
    list.append(type);
    list.append(ip);
    list.append(QString::number(port));
    list.append(linkwagon);

    QByteArray array = GetPackage(0x18,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::UpdateSaveInterval(QString wagon, int lowpowerspeedmax, int temspeedmax, bool Isenable, int VIBInterval, int TemInterval)
{
    QStringList list;
    list.append(wagon);
    list.append(QString::number(lowpowerspeedmax));
    list.append(QString::number(temspeedmax));
    list.append(QString::number(Isenable));
    list.append(QString::number(VIBInterval));
    list.append(QString::number(TemInterval));

    QByteArray array = GetPackage(0x19,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::UpdateAlarmLimit(QString wagon, QStringList values)
{
    if(values.size() != 49) return;
    QStringList list;
    list.append(wagon);
    list.append(values);

    QByteArray array = GetPackage(0x1A,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::UpdateZIPFile(QString wagon, QString filename)
{
    QStringList list;
    list.append(wagon);
    list.append(filename);
    QByteArray array = GetPackage(0x1B,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::UpdateAlarmLed(QString wagon, qint8 id, qint8 ch)
{
    QStringList list;
    list.append(wagon);
    list.append(QString::number(id));
    list.append(QString::number(ch));
    QByteArray array = GetPackage(0x1E,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::TestAddPre(QString wagon, QString str)
{
    QStringList list;
    list.append(wagon);
    list.append(str);
    QByteArray array = GetPackage(0x1F,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::GetPreStateList(QString wagon)
{
    if(wagon.isEmpty()) return;
    QByteArray array = GetPackage(0x80,wagon);
    senddata(array);
}

void UDPMulticastAPI::GetBearingInfo(QString wagon, QString bearing_model)
{
    if(wagon.isEmpty() || bearing_model.isEmpty()) return;
    QStringList list;
    list.append(wagon);
    list.append(bearing_model);

    QByteArray array = GetPackage(0x81,list.join(";"));
    senddata(array);
}

void UDPMulticastAPI::GetTimerTask(QString wagon)
{
    if(wagon.isEmpty()) return;
    QByteArray array = GetPackage(0x82,wagon);
    senddata(array);
}

void UDPMulticastAPI::GetCarConfig(QString wagon)
{
    if(wagon.isEmpty()) return;
    QByteArray array = GetPackage(0x83,wagon);
    senddata(array);
}

void UDPMulticastAPI::GetNetworkConfig(QString wagon)
{
    if(wagon.isEmpty()) return;
    QByteArray array = GetPackage(0x84,wagon);
    senddata(array);
}

void UDPMulticastAPI::GetLinkDeviceInfo(QString wagon)
{
    if(wagon.isEmpty()) return;
    QByteArray array = GetPackage(0x85,wagon);
    senddata(array);
}

void UDPMulticastAPI::GetSaveInterval(QString wagon)
{
    if(wagon.isEmpty()) return;
    QByteArray array = GetPackage(0x86,wagon);
    senddata(array);
}

void UDPMulticastAPI::GetOtherBoardState(QString wagon)
{
    if(wagon.isEmpty()) return;
    QByteArray array = GetPackage(0x87,wagon);
    senddata(array);
}

void UDPMulticastAPI::GetAlarmLimit(QString wagon)
{
    if(wagon.isEmpty()) return;
    QByteArray array = GetPackage(0x88,wagon);
    senddata(array);

}

void UDPMulticastAPI::GetVersion(QString wagon)
{
    if(wagon.isEmpty()) return;
    QByteArray array = GetPackage(0x89,wagon);
    senddata(array);
}

