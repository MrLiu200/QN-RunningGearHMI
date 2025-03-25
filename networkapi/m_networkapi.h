#ifndef M_NETWORKAPI_H
#define M_NETWORKAPI_H

#include <QObject>
#include "m_tcpapi.h"
#include "ftpclient.h"
#include "udpmulticastapi.h"
#include "corehelper.h"
#include "predata.h"

class M_NetworkAPI : public QObject
{
    Q_OBJECT
public:
    static M_NetworkAPI *Instance();
    explicit M_NetworkAPI(QObject *parent = nullptr);
    void Start();
    bool GetRmConfig();
    bool GetRmLog();


private:
    static M_NetworkAPI *self;
    QStringList DownLoadList;
    bool CanDownload;
    QString downLoadFileName;
    QStringList DeleteFileList;
    QStringList AllDataFileList;
    qint16 configfilecount;

    void downloadFile();
    QString GetDownloadDir(QString filename);
private slots:

    void ConfigReturn(QStringList listName);
    void DBFileReturn(QString name);
    void DataFileReturn(QString name);
    void AllDataFileReturn(QString name);
    void DataDownloadReturn(bool ShouldDelete,QStringList listName);
    void ManagerReturnError();
    void ManagerSSLError();
    void OpenLocalFileError(QString filename);
    void UploadProcess(uint64_t current, uint64_t Total);
    void DownloadProcess(uint64_t current, uint64_t Total);
    void DownloadFinish();
signals:
    void DeviceListChanged(QString WagonNum,bool Isend = false);
    void ReturnDBFile(QString filename);
    void ReturnTemData();
    void ReturnAllDataFile(QString filename);
    void ReturnDownLoadData();
    void ReturnCurrentDownLoadFinish();
    void downloadprogress(uint64_t current, uint64_t Total);
    void StartDownLoadAllData(int totalcount);
};

#endif // M_NETWORKAPI_H
