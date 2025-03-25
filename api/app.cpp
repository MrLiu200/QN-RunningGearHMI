#include "app.h"
QString App::ConfigFile = "";
QString App::TempDir = "tempdir";
QString App::DownLoadDataDir = "tempdir/Data";
QString App::DownLoadConfigDir = "tempdir/Config";
QString App::DownLoadDBDir = "tempdir/DB";
QString App::DownLoadTempDataDir = "tempdir/tempData";

QString App::LocalDataBaseFile = ":/HMI.db";

QString App::LocalIP = "192.168.3.123";
QString App::LocalMask = "255.255.255.0";
QString App::LocalGateway = "192.168.3.1";

QString App::FtpHostIP = "192.168.3.233";
int App::FtpPort = 21;
QString App::FtpUser = "forlinx";
QString App::FtpPasswd = "forlinx";
QString App::FtpDirName = "//udisk/ftptemp";

QString App::UdpListenIP = "192.168.3.121";
QString App::UDPMulticastIP = "239.255.43.21";
int App::UDPListenPort = 6500;

QString App::TcpServerIP = "192.168.3.233";
int App::TcpServerPort = 5800;

QString App::CurrentUser = "QN";
QString App::CurrentPassword = "quannuo";

int App::TimeoutCount = 2;
int App::Timeouttimer = 10;

QString App::LiveDataStartTime = "2025-01-01 00:00:00";
QString App::LiveDataEndTime = "2026-12-12 00:00:00";
QString App::HistoryDataStartTime = "2025-01-01 00:00:00";
QString App::HistoryDataEndTime = "2026-12-12 00:00:00";

void App::readConfig()
{
    if (!checkConfig()) {
        return;
    }

    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    App::TempDir = set.value("TempDir").toString();
    App::LocalDataBaseFile = set.value("LocalDataBaseFile").toString();
    set.endGroup();

    set.beginGroup("NetworkConfig");
    App::LocalIP = set.value("LocalIP").toString();
    App::LocalMask = set.value("LocalMask").toString();
    App::LocalGateway = set.value("LocalGateway").toString();
    set.endGroup();

    set.beginGroup("FtpConfig");
    App::FtpHostIP = set.value("FtpHostIP").toString();
    App::FtpPort = set.value("FtpPort").toInt();
    App::FtpUser = set.value("FtpUser").toString();
    App::FtpPasswd = set.value("FtpPasswd").toString();
    App::FtpDirName = set.value("FtpDirName").toString();
    set.endGroup();

    set.beginGroup("UdpConfig");
    App::UDPMulticastIP = set.value("UDPMulticastIP").toString();
    App::UDPListenPort = set.value("UDPListenPort").toInt();
    set.endGroup();

    set.beginGroup("TcpConfig");
    App::TcpServerIP = set.value("TcpServerIP").toString();
    App::TcpServerPort = set.value("TcpServerPort").toInt();
    set.endGroup();

    set.beginGroup("UserConfig");
    App::CurrentUser = set.value("CurrentUser").toString();
    App::CurrentPassword = set.value("CurrentPassword").toInt();
    set.endGroup();

    set.beginGroup("TimeoutConfig");
    App::TimeoutCount = set.value("TimeoutCount").toInt();
    App::Timeouttimer = set.value("Timeouttimer").toInt();
    set.endGroup();

    set.beginGroup("DataTimeConfig");
    App::LiveDataStartTime = set.value("LiveDataStartTime").toString();
    App::LiveDataEndTime = set.value("LiveDataEndTime").toString();
    App::HistoryDataStartTime = set.value("HistoryDataStartTime").toString();
    App::HistoryDataEndTime = set.value("HistoryDataEndTime").toString();
    set.endGroup();
}

void App::writeConfig()
{
    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    set.setValue("TempDir", App::TempDir);
    set.setValue("LocalDataBaseFile",App::LocalDataBaseFile);
    set.endGroup();

    set.beginGroup("NetworkConfig");
    set.setValue("LocalIP", App::LocalIP);
    set.setValue("LocalMask",App::LocalMask);
    set.setValue("LocalGateway", App::LocalGateway);
    set.endGroup();

    set.beginGroup("FtpConfig");
    set.setValue("FtpHostIP", App::FtpHostIP);
    set.setValue("FtpPort",App::FtpPort);
    set.setValue("FtpUser", App::FtpUser);
    set.setValue("FtpPasswd",App::FtpPasswd);
    set.setValue("FtpDirName", App::FtpDirName);
    set.endGroup();

    set.beginGroup("UdpConfig");
    set.setValue("UDPMulticastIP", App::UDPMulticastIP);
    set.setValue("UDPListenPort",App::UDPListenPort);
    set.endGroup();

    set.beginGroup("TcpConfig");
    set.setValue("TcpServerIP", App::TcpServerIP);
    set.setValue("TcpServerPort",App::TcpServerPort);
    set.endGroup();

    set.beginGroup("UserConfig");
    set.setValue("CurrentUser", App::CurrentUser);
    set.setValue("CurrentPassword",App::CurrentPassword);
    set.endGroup();

    set.beginGroup("TimeoutConfig");
    set.setValue("TimeoutCount", App::TimeoutCount);
    set.setValue("Timeouttimer",App::Timeouttimer);
    set.endGroup();

    set.beginGroup("DataTimeConfig");
    set.setValue("LiveDataStartTime", App::LiveDataStartTime);
    set.setValue("LiveDataEndTime",App::LiveDataEndTime);
    set.setValue("HistoryDataStartTime", App::HistoryDataStartTime);
    set.setValue("HistoryDataEndTime",App::HistoryDataEndTime);
    set.endGroup();
}

void App::newConfig()
{
    writeConfig();
}

bool App::checkConfig()
{
    //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
    QFile file(App::ConfigFile);

    if (file.size() == 0) {
        newConfig();
        return false;
    }

    //如果配置文件不完整,则以初始值继续运行,并生成配置文件
    if (file.open(QFile::ReadOnly)) {
        bool ok = true;

        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            QStringList list = line.split("=");

            if (list.count() == 2) {
                if (list.at(1) == "") {
                    ok = false;
                    break;
                }
            }
        }

        if (!ok) {
            newConfig();
            return false;
        }
    } else {
        newConfig();
        return false;
    }

    return true;
}
