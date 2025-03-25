#include "importofflinefile.h"
#include "m_dbapi.h"
#include <QtConcurrent>
ImportOfflineFile *ImportOfflineFile::self = 0;
ImportOfflineFile *ImportOfflineFile::Instance()
{
    if(!self){
        QMutex mutex;
        QMutexLocker lock(&mutex);
        if(!self){
            self = new ImportOfflineFile;
        }
    }
    return self;
}

ImportOfflineFile::ImportOfflineFile(QObject *parent) : QObject(parent)
{
    connect(this,&ImportOfflineFile::copyResult,this,&ImportOfflineFile::importdataresult);
    connect(this,&ImportOfflineFile::unzipResult,this,&ImportOfflineFile::importdataresult);
}

QStringList ImportOfflineFile::importconfig(QStringList filelist)
{
    QStringList retlist;
    if(filelist.isEmpty()) return retlist;
    for(QString str : filelist){
        QFile file(str);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            continue;
        }
        //加载数据
        QTextStream in(&file);
        //取出车厢号
        QString wagon = "";
        QFileInfo fileinfo(str);
        QString filename = fileinfo.baseName();
        QStringList list = filename.split("_");
        if(list.size() > 3){
            wagon= list.at(3);
        }
        //判断，如果车厢已经存在，则跳过
        if(PreData::wagonsList.contains(wagon)){
            continue;
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
        retlist.append(wagon);
    }
    return retlist;
}

bool ImportOfflineFile::importSql(QString dbfile)
{
    return M_DbAPI::Instance()->ConnectDataDB(dbfile);
}

void ImportOfflineFile::importData(QStringList filelist)
{
    QString suffix = filelist.first().split(".").at(1);
    QString content = "数据导入中……";


    QString targetPath = App::DownLoadTempDataDir;
    //判断是bin文件还是压缩文件
    if(suffix == "bin"){
        QtConcurrent::run(this, &ImportOfflineFile::CopyOrigin, targetPath,filelist);
    }else{
         QtConcurrent::run(this, &ImportOfflineFile::unzipOrigin, targetPath,filelist);
    }
}

void ImportOfflineFile::CopyOrigin(QString targetdir, QStringList files)
{
    for(QString str : files){
        QFileInfo fileinfo(str);
        QString filename = fileinfo.baseName();
        if(!CoreHelper::copyFile(str,targetdir + "/" + filename)){
            Q_EMIT copyResult(false);
        }
    }
    Q_EMIT copyResult(true);
}

void ImportOfflineFile::unzipOrigin(QString targetdir, QStringList files)
{
    for(QString str : files){
        if(!CoreHelper::Zip_extract(targetdir,str)){
            Q_EMIT unzipResult(false);
            return;
        }
    }
    Q_EMIT unzipResult(true);
}


