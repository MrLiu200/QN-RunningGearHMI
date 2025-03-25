#ifndef M_TCPAPI_H
#define M_TCPAPI_H

#include <QObject>
#include <QtNetwork>

#define PACKET_HEADER   0xB6            //帧头
#define PACKET_END      0x6B            //帧尾

class M_TcpAPI : public QObject
{
    Q_OBJECT
public:
    static M_TcpAPI *Instance();
    explicit M_TcpAPI(QObject *parent = nullptr);
    ~M_TcpAPI();
private:
    static M_TcpAPI *self;
    QTcpSocket *socket;
    bool M_Isconnect;

public:
    bool ConnectServer();
    void DisconnectServer();
    void GetConfigIni();
    void GetDBFile();
    void GetDataFile(QString data);
    void GetAllData(QString data);
    void DeleteFile(QString file);
    void ClearZIPData();
    QByteArray GetPacket(uint8_t order, QString data);
    bool GetConnectState();

private:
    void CheckData(QByteArray data);
    void SendData(QByteArray data);
private slots:
    void ReadData();
    void Disconnect();
    void Error(QAbstractSocket::SocketError);

private:
    void ReturnConfigIni(QString str);
    void ReturnDBFile(QString str);
    void ReturnDataFile(QString str);
    void ReturnAllDataFile(QString str);
    void ReturnDataDownload(QString str);
    void ReturnError(QString str);
signals:
    void ConfigReturn(QStringList listName);
    void DBFileReturn(QString name);
    void DataFileReturn(QString name);
    void AllDataFileReturn(QString name);
    void DataDownloadReturn(bool ShouldDelete,QStringList listName);
};

#endif // M_TCPAPI_H
