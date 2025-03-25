#include "m_networkapi.h"
#include "QMessageBox"
#include <zlib.h>
#include "quazip.h"
#include "quazipfile.h"
#include "JlCompress.h"
M_NetworkAPI *M_NetworkAPI::self = 0;
M_NetworkAPI *M_NetworkAPI::Instance()
{
    if(!self){
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if(!self){
            self = new M_NetworkAPI;
        }
    }
    return self;
}

M_NetworkAPI::M_NetworkAPI(QObject *parent) : QObject(parent)
{
    CanDownload = true;
    DownLoadList = QStringList();
    AllDataFileList = QStringList();
    DeleteFileList = QStringList();
}

void M_NetworkAPI::Start()
{
    connect(M_TcpAPI::Instance(),&M_TcpAPI::ConfigReturn,this,&M_NetworkAPI::ConfigReturn);
    connect(M_TcpAPI::Instance(),&M_TcpAPI::DBFileReturn,this,&M_NetworkAPI::DBFileReturn);
    connect(M_TcpAPI::Instance(),&M_TcpAPI::DataFileReturn,this,&M_NetworkAPI::DataFileReturn);
    connect(M_TcpAPI::Instance(),&M_TcpAPI::AllDataFileReturn,this,&M_NetworkAPI::AllDataFileReturn);
    connect(M_TcpAPI::Instance(),&M_TcpAPI::DataDownloadReturn,this,&M_NetworkAPI::DataDownloadReturn);

    connect(FtpClient::Instance(),&FtpClient::ManagerReturnError,this,&M_NetworkAPI::ManagerReturnError);
    connect(FtpClient::Instance(),&FtpClient::ManagerSSLError,this,&M_NetworkAPI::ManagerSSLError);
    connect(FtpClient::Instance(),&FtpClient::OpenLocalFileError,this,&M_NetworkAPI::OpenLocalFileError);
    connect(FtpClient::Instance(),&FtpClient::UploadProcess,this,&M_NetworkAPI::UploadProcess);
    connect(FtpClient::Instance(),&FtpClient::DownloadProcess,this,&M_NetworkAPI::DownloadProcess);
    connect(FtpClient::Instance(),&FtpClient::DownloadProcess,this,&M_NetworkAPI::downloadprogress);
    connect(FtpClient::Instance(),&FtpClient::DownloadFinish,this,&M_NetworkAPI::DownloadFinish);
}

bool M_NetworkAPI::GetRmConfig()
{
    bool Isconnect = M_TcpAPI::Instance()->GetConnectState();
    if(Isconnect){
        M_TcpAPI::Instance()->GetConfigIni();
    }
    return Isconnect;
}

bool M_NetworkAPI::GetRmLog()
{
    bool Isconnect = M_TcpAPI::Instance()->GetConnectState();
    if(Isconnect){
        M_TcpAPI::Instance()->GetDBFile();
    }
    return Isconnect;
}

void M_NetworkAPI::downloadFile()
{
    if(DownLoadList.isEmpty() || !CanDownload) return;
    CanDownload = false;
    downLoadFileName = DownLoadList.at(0);
    DownLoadList.removeFirst();//App::FtpDirName +
    QString ftpDownloadUrl =  "/" + downLoadFileName;
    QStringList listtemp = downLoadFileName.split("/");
    QString filedir = GetDownloadDir(listtemp.last());
//    QString  downloadurl = App::TempDir + "/" + listtemp.last();
    QString  downloadurl = filedir + "/" + listtemp.last();
    FtpClient::Instance()->SetFtpServerFile(ftpDownloadUrl);
    FtpClient::Instance()->SetDownloadFile(downloadurl);
    FtpClient::Instance()->get();
}

QString M_NetworkAPI::GetDownloadDir(QString filename)
{
    QString downloaddir = App::TempDir;
    if(filename.startsWith("config")){//配置文件
        downloaddir = App::DownLoadConfigDir;
    }else if(filename.contains(".db")){//数据库文件
        downloaddir = App::DownLoadDBDir;
    }else if(filename.contains("temporarily")){//临时数据文件
        downloaddir = App::DownLoadTempDataDir;
    }else if(filename.contains("data.zip")){//数据下载文件
        downloaddir = App::DownLoadDataDir;
    }
    return downloaddir;
}

void M_NetworkAPI::ConfigReturn(QStringList listName)
{
    configfilecount = listName.size();
    DownLoadList.append(listName);
    downloadFile();
}

void M_NetworkAPI::DBFileReturn(QString name)
{
    DownLoadList.append(name);
    downloadFile();
}

void M_NetworkAPI::DataFileReturn(QString name)
{
//    DownLoadList.append(App::FtpDirName + "/" + name);
    DownLoadList.append(name);
    downloadFile();
}

void M_NetworkAPI::AllDataFileReturn(QString name)
{
    Q_EMIT StartDownLoadAllData(1);
    DownLoadList.append(name);
    downloadFile();

}

void M_NetworkAPI::DataDownloadReturn(bool ShouldDelete, QStringList listName)
{
    if(ShouldDelete){
        DeleteFileList.append(listName);
    }
    AllDataFileList.append(listName);
    Q_EMIT StartDownLoadAllData(listName.size());
    DownLoadList.append(listName);
    downloadFile();
}

void M_NetworkAPI::ManagerReturnError()
{
    //下载文件出错
    QMessageBox::warning(nullptr,"Ftp错误","下载文件出错:" + downLoadFileName);
    CanDownload = true;
    downloadFile();
}

void M_NetworkAPI::ManagerSSLError()
{
    //下载文件出错
    QMessageBox::warning(nullptr,"Ftp错误","SSL证书 出错");
    CanDownload = true;
    downloadFile();
}

void M_NetworkAPI::OpenLocalFileError(QString filename)
{
    QMessageBox::warning(nullptr,"文件错误","文件打开出错" + filename);
}

void M_NetworkAPI::UploadProcess(uint64_t current, uint64_t Total)
{
    qDebug()<<"当前：" << current << " 剩余:" << Total-current;
}

void M_NetworkAPI::DownloadProcess(uint64_t current, uint64_t Total)
{
    qDebug()<<"下载进度:  当前：" << current << " 剩余:" << Total-current;
}

void M_NetworkAPI::DownloadFinish()
{
    QString sss = downLoadFileName.split("/").last();
    QString downloadurl = App::TempDir + "/" + sss;
    //解析当前下载的文件
    if(sss.startsWith("config")){//配置文件 config_项目号_车号_车厢号.csv
        downloadurl = App::DownLoadConfigDir + "/" + sss;
        //加载内容
        QFile file(downloadurl);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);
            //取出车厢号
            QString wagon = "";
            QFileInfo fileinfo(downloadurl);
            QString filename = fileinfo.baseName();
            QStringList list = filename.split("_");
            if(list.size() > 3){
                wagon= list.at(3);
            }
            //判断，如果车厢已经存在，则跳过
            if(PreData::wagonsList.contains(wagon)){
                return;
            }
            QString infotype;
            int infolen = 0;
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                //判断这一行是什么内容
                if(line == "BearingInfo"|| line == "PreInfo"){
                    infotype = line;
                    if(!in.atEnd()){
                        line = in.readLine().trimmed();
                        infolen = line.toInt();
                    }
                }else if(infolen > 0){
                    QStringList dataList = line.split(",");
                    if(infotype == "BearingInfo"){
                        if(dataList.size() < 5) continue;
                        PreData::BEARINGINFO bearing;
                        bearing.Wagon = wagon;
                        bearing.Model = dataList.at(0).trimmed();
                        bearing.PitchDiameter = dataList.at(1).trimmed().toFloat();
                        bearing.RollingDiameter = dataList.at(2).trimmed().toFloat();
                        bearing.RollerNum = dataList.at(3).trimmed().toInt();
                        bearing.ContactAngle = dataList.at(4).trimmed().toFloat();
                        PreData::bearinglist.append(bearing);
                    }else if(infotype == "PreInfo"){
                        if(dataList.size() < 20) continue;
                        PreData::DEVICEINFO device;
                        device.Wagon = wagon;
                        device.state = PreData::DeviceState_Online;
                        device.id = dataList.at(0).trimmed().toInt();
                        device.ch = dataList.at(1).trimmed().toInt();
                        device.name = dataList.at(2).trimmed();
                        device.type = dataList.at(3).trimmed();
                        device.sensitivity = dataList.at(4).trimmed().toFloat();
                        device.shaftDiameter = dataList.at(5).trimmed().toFloat();
                        device.bearing1_name = dataList.at(6).trimmed();
                        device.bearing1_model = dataList.at(7).trimmed();
                        device.bearing2_name = dataList.at(8).trimmed();
                        device.bearing2_model = dataList.at(9).trimmed();
                        device.bearing3_name = dataList.at(10).trimmed();
                        device.bearing3_model = dataList.at(11).trimmed();
                        device.bearing4_name = dataList.at(12).trimmed();
                        device.bearing4_model = dataList.at(13).trimmed();
                        device.capstanname = dataList.at(14).trimmed();
                        device.capstanTeethNum = dataList.at(15).trimmed().toInt();
                        device.drivenwheelName = dataList.at(16).trimmed();
                        device.drivenwheelTeethNum = dataList.at(17).trimmed().toInt();
                        device.version = dataList.at(18).trimmed();
                        device.IsEnable = dataList.at(19).trimmed().toInt();
                        PreData::devicelist.append(device);
                    }
                    infolen--;
                }
            }
            file.close();
            configfilecount--;
            if(configfilecount == 0){
                Q_EMIT DeviceListChanged(wagon,true);
            }else{
                Q_EMIT DeviceListChanged(wagon);
            }
        }
    }else if(sss.contains(".db")){//db文件
        downloadurl = App::DownLoadDBDir + "/" + sss;
        Q_EMIT ReturnDBFile(downloadurl);
    }else if (sss.contains("temporarily")){//数据文件,触发信号
        // 获取文件信息
        downloadurl = App::DownLoadTempDataDir + "/" + sss;
        QFileInfo fileInfo(downloadurl);
        QString filename = fileInfo.baseName();
        //解析出车厢号并下发删除命令
        QString wagon = filename.split("_").at(0);
//        UDPMulticastAPI::Instance()->UpdateZIPFile(wagon,fileInfo.fileName());
        M_TcpAPI::Instance()->DeleteFile(downLoadFileName);
//        QString basedir = App::DownLoadTempDataDir + "/" + filename;
        QString basedir = App::DownLoadTempDataDir;
        if(CoreHelper::Zip_extract(App::DownLoadTempDataDir,downloadurl)){
            qDebug() << "解压成功：\n";
            Q_EMIT ReturnTemData();
        }else{
            qDebug() << "解压失败：\n";
        }
    }else if(sss.contains("AllData")){//数据下载
        QFileInfo fileInfo(downloadurl);
        QString filename = fileInfo.baseName();
        //解析出车厢号并下发删除命令
        QString wagon = filename.split("_").at(0);
        UDPMulticastAPI::Instance()->UpdateZIPFile(wagon,fileInfo.fileName());
        Q_EMIT ReturnAllDataFile(downloadurl);
    }else if(sss.contains("data.zip")){//数据下载
        downloadurl = App::DownLoadDataDir + "/" + sss;
//        QFileInfo fileInfo(downloadurl);
//        QString filename = fileInfo.baseName();
        for(int i=0;i<DeleteFileList.size();i++){
            QString str = DeleteFileList.at(i);
            if(str.contains(sss)){
                M_TcpAPI::Instance()->DeleteFile(str);
                DeleteFileList.removeAt(i);
                break;
            }
        }
        if(AllDataFileList.contains(downLoadFileName)){
            AllDataFileList.removeOne(downLoadFileName);
        }
        if(AllDataFileList.isEmpty()){
            Q_EMIT ReturnDownLoadData();
        }else{
            Q_EMIT ReturnCurrentDownLoadFinish();
        }
        qDebug()<<"*******数据下载成功*******" <<sss << "*******";
    }

    CanDownload = true;
    downloadFile();
}
