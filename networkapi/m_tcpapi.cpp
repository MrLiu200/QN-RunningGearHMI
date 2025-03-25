#include "m_tcpapi.h"
#include "app.h"
#include <QMessageBox>
M_TcpAPI *M_TcpAPI::self = 0;
M_TcpAPI *M_TcpAPI::Instance()
{
    if(!self){
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if(!self){
            self = new M_TcpAPI;
        }
    }
    return self;
}

M_TcpAPI::M_TcpAPI(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket,&QTcpSocket::readyRead,this,&M_TcpAPI::ReadData);
    connect(socket,&QTcpSocket::disconnected,this,&M_TcpAPI::Disconnect);
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(Error(QAbstractSocket::SocketError)));

    M_Isconnect = false;
}

M_TcpAPI::~M_TcpAPI()
{
    DisconnectServer();
}

bool M_TcpAPI::ConnectServer()
{
    bool ret = false;
    if(!M_Isconnect){
        socket->connectToHost(App::TcpServerIP,App::TcpServerPort);
        ret = socket->waitForConnected(3000);
        if(ret){
            M_Isconnect = true;
        }else{
            qDebug()<< "connect server fail" << socket->errorString();
        }
    }
    return ret;
}

void M_TcpAPI::DisconnectServer()
{
    if(M_Isconnect){
        socket->disconnectFromHost();
    }
}

void M_TcpAPI::GetConfigIni()
{
    QByteArray arr = GetPacket(0xA0,QString());
    SendData(arr);
}

void M_TcpAPI::GetDBFile()
{
    QByteArray arr = GetPacket(0xA1,QString());
    SendData(arr);
}

void M_TcpAPI::GetDataFile(QString data)
{
    QByteArray arr = GetPacket(0xA2,data);
    SendData(arr);
}

void M_TcpAPI::GetAllData(QString data)
{
    QByteArray arr = GetPacket(0xA4,data);
    SendData(arr);
}

void M_TcpAPI::DeleteFile(QString file)
{
    QByteArray arr = GetPacket(0xA5,file);
    SendData(arr);
}

void M_TcpAPI::ClearZIPData()
{
    QByteArray arr = GetPacket(0xA6,QString());
    SendData(arr);
}

QByteArray M_TcpAPI::GetPacket(uint8_t order, QString data)
{
    QByteArray messageData = data.toUtf8();
    uint16_t len = messageData.size();
    QByteArray array;
    array.append(0xB6);
    array.append(0xB6);
    array.append(order);
    array.append((len >> 8) & 0xff);
    array.append(len & 0xff);
    array.append(messageData);
    array.append(0x6B);
    array.append(0x6B);
    return array;
}

bool M_TcpAPI::GetConnectState()
{
    return this->M_Isconnect;
}

void M_TcpAPI::CheckData(QByteArray data)
{
    int len = data.size();
    if(len == 0 ) return;

    uint8_t frame[len];

    for(int i=0; i<len; i++){
        frame[i] = data[i];
    }
    if((frame[0] == PACKET_HEADER) && (frame[1] == PACKET_HEADER) && (frame[len-1] == PACKET_END) &&(frame[len-2] == PACKET_END)){
        uint8_t orderByte = frame[2];
        uint16_t datalen = (frame[3] << 8) | frame[4];
        //数据内容
//        QByteArray arr = ;
        QString info = QString::fromUtf8(data.mid(5,datalen));

        switch (orderByte) {
        case 0xA0://获取配置文件
            ReturnConfigIni(info);
            break;
        case 0xA1://获取数据库文件
            ReturnDBFile(info);
            break;
        case 0xA2://获取原始数据(范围u)
            ReturnDataFile(info);
            break;
        case 0xA3://下载数据
            ReturnAllDataFile(info);
            break;
        case 0xA4://2025-02-12测试
            ReturnDataDownload(info);
            break;
        case 0xFF://错误命令
            ReturnError(info);
            break;
        default:
            break;
        }
    }
}

void M_TcpAPI::SendData(QByteArray data)
{
#if 0
    if((socket->state() == QAbstractSocket::ConnectedState) && M_Isconnect) {
        socket->write(data);
    }
#else
    if (socket->bytesToWrite() == 0) {
               socket->write(data);
           } else {
               qDebug() << "Socket is busy, cannot send data right now.";
           }
#endif
}

void M_TcpAPI::ReadData()
{
    QByteArray array = socket->readAll();
    CheckData(array);
}

void M_TcpAPI::Disconnect()
{
    M_Isconnect = false;
}

void M_TcpAPI::Error(QAbstractSocket::SocketError)
{
    qDebug() << "Socket error:" << socket->errorString();
    M_Isconnect = false;
}

void M_TcpAPI::ReturnConfigIni(QString str)
{
    if(str.isEmpty()){
        QMessageBox::warning(nullptr,"错误","配置文件不存在");
        return;
    }
    QStringList list = str.split(";");
    Q_EMIT ConfigReturn(list);
}

void M_TcpAPI::ReturnDBFile(QString str)
{
    if(str.isEmpty()){
        QMessageBox::warning(nullptr,"错误","板载数据库文件不存在");
        return;
    }
    QString name = str;
    Q_EMIT DBFileReturn(name);
}

void M_TcpAPI::ReturnDataFile(QString str)
{
    if(str.isEmpty()){
        QMessageBox::warning(nullptr,"错误","数据为空");
        return;
    }
    QString Datafile = str;
    Q_EMIT DataFileReturn(Datafile);
}

void M_TcpAPI::ReturnAllDataFile(QString str)
{
    if(str.isEmpty()){
        QMessageBox::warning(nullptr,"错误","数据为空");
        return;
    }
    QString Datafile = str;
    Q_EMIT AllDataFileReturn(Datafile);
}

void M_TcpAPI::ReturnDataDownload(QString str)
{
    if(str.isEmpty()){
        QMessageBox::warning(nullptr,"错误","数据为空");
        return;
    }
    //解析
    QStringList list = str.split(";");
    if(list.size() < 2) return;
    QStringList filename = list.at(1).split("|");
    bool shoulddelete = list.at(0).toInt();
    Q_EMIT DataDownloadReturn(shoulddelete,filename);
}

void M_TcpAPI::ReturnError(QString str)
{
    if(str.isEmpty()) return;
    //解析
    QStringList list = str.split(";");
    if(list.size() < 2) return;
    int errororder = list.at(0).toInt();
    QString content = list.at(1);

    //生成命令描述
    QString commandstr;
    switch(errororder){
    case 0xA0:
        commandstr = "获取配置文件";
        break;
    case 0xA1:
        commandstr = "下载数据库文件";
        break;
    case 0xA2:
        commandstr = "下载原始数据(视图显示)";
        break;
    case 0xA3:
    case 0xA4:
        commandstr = "原始数据下载(数据保存)";
        break;
    case 0xA5:
        commandstr = "删除单文件";
        break;
    case 0xA6:
        commandstr = "清空ZIP数据";
        break;
    }

    QMessageBox::warning(nullptr,commandstr,content);
}
