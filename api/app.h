#ifndef APP_H
#define APP_H

#include <QString>
#include <QtCore>
class App
{
public:
    static QString ConfigFile;          //配置文件文件路径及名称
    static QString TempDir;             //临时文件存储目录
    static QString DownLoadDataDir;         //数据下载目录
    static QString DownLoadConfigDir;       //配置文件下载目录
    static QString DownLoadDBDir;           //数据库文件下载目录
    static QString DownLoadTempDataDir;     //临时数据下载目录
    static QString LocalDataBaseFile;       //本地数据库名称

    static QString LocalIP;                 //本机IP
    static QString LocalMask;               //本机掩码
    static QString LocalGateway;            //本机网关

    static QString FtpHostIP;               //ftp主机地址
    static int FtpPort;                     //ftp 端口
    static QString FtpUser;                 //ftp 用户
    static QString FtpPasswd;               //ftp 密码
    static QString FtpDirName;              //ftp 操作的目录

    //udp 网络配置
    static QString UdpListenIP;                     //UDP监听端口
    static QString UDPMulticastIP;                  //UDP组播IP
    static int UDPListenPort;                       //UDP监听端口

    //Tcp 网络配置
    static QString TcpServerIP;             //Tcp服务器ip
    static int TcpServerPort;               //Tcp服务器端口

    //用户设置
    static QString CurrentUser;
    static QString CurrentPassword;

    //设备超时检测
    static int TimeoutCount;                //超时次数
    static int Timeouttimer;                //超时时间

    //查看数据时间区间
    static QString LiveDataStartTime;               //波形开始时间
    static QString LiveDataEndTime;                 //波形结束时间
    static QString HistoryDataStartTime;               //趋势开始时间
    static QString HistoryDataEndTime;                 //趋势结束时间


    static void readConfig();               //读取配置文件,在main函数最开始加载程序载入
    static void writeConfig();              //写入配置文件,在更改配置文件程序关闭时调用
    static void newConfig();                //以初始值新建配置文件
    static bool checkConfig();              //校验配置文件
};

#endif // APP_H
