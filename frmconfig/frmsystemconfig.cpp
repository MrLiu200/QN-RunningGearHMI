#include "frmsystemconfig.h"
#include "ui_frmsystemconfig.h"
#include <QProcess>
#include "app.h"
frmSystemConfig *frmSystemConfig::self = 0;
frmSystemConfig *frmSystemConfig::Instance()
{
    if (!self) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new frmSystemConfig;
        }
    }
    return self;
}

frmSystemConfig::frmSystemConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmSystemConfig)
{
    ui->setupUi(this);
    initfrm();
    setWindowTitle("本地设置");
}

frmSystemConfig::~frmSystemConfig()
{
    delete ui;
}

void frmSystemConfig::initfrm()
{
    baseconfigInit();
    aboutconfigInit();
    netconfigInit();
}

void frmSystemConfig::baseconfigInit()
{
    ui->txtUdpAddr->setText(App::UDPMulticastIP);
    ui->txtUdpPort->setText(QString::number(App::UDPListenPort));
    ui->txtTcpip->setText(App::TcpServerIP);
    ui->txtTcpPort->setText(QString::number(App::TcpServerPort));
    ui->txt_ftpip->setText(App::FtpHostIP);
    ui->txt_ftpport->setText(QString::number(App::FtpPort));
    ui->txt_ftpuser->setText(App::FtpUser);
    ui->txt_ftppassword->setText(App::FtpPasswd);
    ui->txt_udplistenip->setText(App::UdpListenIP);


    QStringList list;
    list << "1次" << "2次" << "3次" << "4次" << "5次" << "6次" <<"7次" << "8次" << "9次" <<"10次" << "15次" << "20次";
    ui->cboxtimeoutCount->addItems(list);
    QString str = QString("%1次").arg(App::TimeoutCount);
    ui->cboxtimeoutCount->setCurrentIndex(ui->cboxtimeoutCount->findText(str));

    list.clear();
    list << "10秒钟" << "20秒钟" << "30秒钟"<< "40秒钟" << "50秒钟" << "60秒钟"<< "70秒钟" << "80秒钟" << "90秒钟"<< "100秒钟" << "110秒钟" << "120秒钟";
    ui->cboxtimeoutTime->addItems(list);
    str = QString("%1秒钟").arg(App::Timeouttimer);
    ui->cboxtimeoutTime->setCurrentIndex(ui->cboxtimeoutTime->findText(str));

    connect(ui->btnUpdate,&QPushButton::clicked,this,&frmSystemConfig::baseconfigupdate);

}

void frmSystemConfig::aboutconfigInit()
{
    ui->labAuthor->setText("作者: Mr.Liu");
    ui->labTel->setText("Tel: 152xxxx4418");
    ui->labWebsite->setText("官网: http://www.qnode.cn/");
    ui->labAppVersion->setText("软件版本: v20241385513");
    ui->lab_copyright->setText("版权所有:上海诠诺物联网技术有限公司");
    ui->lab_email->setText("Email: 1207997878@qq.com");

}

void frmSystemConfig::netconfigInit()
{
    ui->txtIP->setText(App::LocalIP);
    ui->txtMask->setText(App::LocalMask);
    ui->txtGateway->setText(App::LocalGateway);
}

void frmSystemConfig::on_btnGetNetConfig_clicked()
{
    QList<QNetworkInterface> network = QNetworkInterface::allInterfaces();    // 获取所有的接口
    foreach(QNetworkInterface net, network)
    {
        QString netName = net.humanReadableName();  // 获取网络名称

        QList<QNetworkAddressEntry> list = net.addressEntries();  // 获取IP地址与子网掩码等
        foreach(QNetworkAddressEntry address,list)
        {
            if(address.ip().protocol() == QAbstractSocket::IPv4Protocol)  // 获取IPv4的地址
            {
                if(netName == "以太网"){
                    ui->txtIP->setText(address.ip().toString());//ip
                    ui->txtMask->setText(address.netmask().toString());//掩码
                }
            }
        }
    }

}

void frmSystemConfig::on_btnSetNetConfig_clicked()
{
    QString ip = ui->txtIP->text().trimmed();
    QString mask = ui->txtMask->text().trimmed();
    QString gateway = ui->txtGateway->text().trimmed();
    if(ip.isEmpty() || mask.isEmpty() || gateway.isEmpty()){
        QMessageBox::warning(this,"错误","网卡信息不完整!!!");
        return;
    }

    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    if(!RegExp.exactMatch(ip)){
        QMessageBox::warning(this,"错误","IP地址不合法!!!");
        return;
    }
    if(!RegExp.exactMatch(gateway)){
        QMessageBox::warning(this,"错误","网关地址不合法!!!");
        return;
    }

    QProcess *process = new QProcess();
    QString cmd = "netsh interface ip set address ";
    cmd += "\"";     //网卡名称要加双引号
    cmd += "以太网";
    cmd += "\" ";

    cmd += "static ";

    cmd += ui->txtIP->text(); //IP
    cmd += " ";

    cmd += ui->txtMask->text(); //子网掩码
    cmd += " ";

    cmd += ui->txtGateway->text(); //网关

    qDebug()<< "cmd = " << cmd;

    process->start(cmd);   //执行dos命令
    process->waitForFinished(); //等待执行完成
    delete process;

    App::LocalIP = ui->txtIP->text();
    App::LocalMask = ui->txtMask->text();
    App::LocalGateway = ui->txtGateway->text();
    App::writeConfig();
    QMessageBox::information(this,"提示","网卡配置修改成功~");

}

void frmSystemConfig::baseconfigupdate()
{
//    APPConfig::Title = ui->txtAppTitle->text();
    App::UDPMulticastIP = ui->txtUdpAddr->text();
    App::UDPListenPort = ui->txtUdpPort->text().toInt();
    App::TcpServerIP = ui->txtTcpip->text();
    App::TcpServerPort = ui->txtTcpPort->text().toInt();

    App::FtpHostIP = ui->txt_ftpip->text();
    App::FtpPort = ui->txt_ftpport->text().toInt();
    App::FtpUser = ui->txt_ftpuser->text();
    App::FtpPasswd = ui->txt_ftppassword->text();
    App::UdpListenIP = ui->txt_udplistenip->text();

    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    if(!RegExp.exactMatch(App::UDPMulticastIP)){
        QMessageBox::warning(this,"错误","UDP组播地址不合法!!!");
        return;
    }
    if(!RegExp.exactMatch(App::TcpServerIP)){
        QMessageBox::warning(this,"错误","Tcp服务器地址不合法!!!");
        return;
    }
    if(!RegExp.exactMatch(App::FtpHostIP)){
        QMessageBox::warning(this,"错误","FTP服务器地址不合法!!!");
        return;
    }


    QString str = ui->cboxtimeoutCount->currentText();
    int count = str.mid(0,str.size()-1).toInt();
    App::TimeoutCount = count;

    str = ui->cboxtimeoutTime->currentText();
    count = str.mid(0,str.size()-2).toInt();
    App::Timeouttimer = count;

    App::writeConfig();

    QMessageBox::information(this,"提示","更新配置成功");
}
