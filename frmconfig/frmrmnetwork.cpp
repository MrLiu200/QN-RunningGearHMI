#include "frmrmnetwork.h"
#include "ui_frmrmnetwork.h"
#include <QMessageBox>
#include "corehelper.h"
FrmRMNetwork::FrmRMNetwork(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMNetwork)
{
    ui->setupUi(this);
    frmInit();
    NeedSave = false;
}

FrmRMNetwork::~FrmRMNetwork()
{
    delete ui;
}

void FrmRMNetwork::frmInit()
{
    QList<QLabel *> labels = findChildren<QLabel *>();

    for(auto lab : labels){
        lab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    txtNetInfo.append(ui->txt_eth0_ip);
    txtNetInfo.append(ui->txt_eth0_mask);
    txtNetInfo.append(ui->txt_eth0_gateway);
    txtNetInfo.append(ui->txt_eth0_dns);
    txtNetInfo.append(ui->txt_eth1_ip);
    txtNetInfo.append(ui->txt_eth1_mask);
    txtNetInfo.append(ui->txt_eth1_gateway);
    txtNetInfo.append(ui->txt_eth1_dns);
    txtNetInfo.append(ui->txt_serverip);
    txtNetInfo.append(ui->txt_serverport);
    txtNetInfo.append(ui->txt_listenport);
    txtNetInfo.append(ui->txt_standbyip);
    txtNetInfo.append(ui->txt_standbyport);
    txtNetInfo.append(ui->txt_udpip);
    txtNetInfo.append(ui->txt_udpport);

    for(auto txt : txtNetInfo){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setProperty("lastText","Null");
        connect(txt,&QLineEdit::editingFinished,this,&FrmRMNetwork::DataChange);
    }

    ui->txt_eth0_ip->setProperty("CheckIp",true);
    ui->txt_eth0_mask->setProperty("CheckIp",true);
    ui->txt_eth0_gateway->setProperty("CheckIp",true);
    ui->txt_eth0_dns->setProperty("CheckIp",true);
    ui->txt_eth1_ip->setProperty("CheckIp",true);
    ui->txt_eth1_mask->setProperty("CheckIp",true);
    ui->txt_eth1_gateway->setProperty("CheckIp",true);
    ui->txt_eth1_dns->setProperty("CheckIp",true);
    ui->txt_serverip->setProperty("CheckIp",true);
    ui->txt_standbyip->setProperty("CheckIp",true);
    ui->txt_udpip->setProperty("CheckIp",true);

    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::networkconfigreturn,this,&FrmRMNetwork::UDPDataReturn);
}

void FrmRMNetwork::RefreshData()
{
    if(networkinfo.isEmpty()) return;

    for(int i=0;i<networkinfo.size();i++){
        txtNetInfo.at(i)->setText(networkinfo.at(i));
        txtNetInfo.at(i)->setProperty("lastText",networkinfo.at(i));
    }
}

QString FrmRMNetwork::GetCurrentWagon() const
{
    return this->Wagon;
}

void FrmRMNetwork::SetCurrentWagon(const QString &wagon)
{
    for(auto txt : txtNetInfo){
        txt->clear();
    }
    networkinfo.clear();
    for(int i=0;i<txtNetInfo.size();i++){
        txtNetInfo.at(i)->setProperty("lastText","Null");
    }
    this->Wagon = wagon;
    UDPMulticastAPI::Instance()->GetNetworkConfig(Wagon);
}

void FrmRMNetwork::SaveData()
{
    if(!NeedSave) return;

    if(QMessageBox::question(this,"保存","是否将改变内容提交") == QMessageBox::No){
        NeedSave = false;
    }else{
        on_btn_save_clicked();
    }
}

void FrmRMNetwork::RedrawData()
{
    RefreshData();
}

void FrmRMNetwork::on_btn_save_clicked()
{
    if(Wagon.isEmpty()) return;
    for(auto txt : txtNetInfo){
        if(txt->text().isEmpty()){
            QMessageBox::warning(this,"提示","未填写完整");
            return;
        }else if(txt->property("CheckIp") == true){
            if(!CoreHelper::isIP(txt->text())){
                QMessageBox::warning(this,"提示","填写信息不合法");
                return;
            }
        }
    }

    QString ip = ui->txt_eth0_ip->text();
    QString mask = ui->txt_eth0_mask->text();
    QString gateway = ui->txt_eth0_gateway->text();
    QString dns = ui->txt_eth0_dns->text();
    QString ip1 = ui->txt_eth1_ip->text();
    QString mask1 = ui->txt_eth1_mask->text();
    QString gateway1 = ui->txt_eth1_gateway->text();
    QString dns1 = ui->txt_eth1_dns->text();
    QString serverip = ui->txt_serverip->text();
    int serverport = ui->txt_serverport->text().toInt();
    int listenport = ui->txt_listenport->text().toInt();
    QString standbyip = ui->txt_standbyip->text();
    int standbyport = ui->txt_standbyport->text().toInt();
    QString udpip = ui->txt_udpip->text();
    int udpport = ui->txt_udpport->text().toInt();

    //检查IP是否填写规范


    UDPMulticastAPI::Instance()->UpdateNetWorkConfing(Wagon,ip,mask,gateway,dns,
                                                      ip1,mask1,gateway1,dns1,
                                                      serverip,serverport,listenport,standbyip,
                                                      standbyport,udpip,udpport);
    NeedSave = false;
    CoreHelper::showFadingMessage(this,"已发送提交");
}

void FrmRMNetwork::DataChange()
{
     QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
     if (!lineEdit) return;

     QString LastText = lineEdit->property("lastText").toString();
     QString newText = lineEdit->text();
     if(LastText != newText){
         lineEdit->setProperty("lastText",newText);
         NeedSave = true;
     }
     lineEdit->clearFocus();
}

void FrmRMNetwork::UDPDataReturn(QString wagon, QStringList linklist)
{
    if(wagon != this->Wagon){
        return;
    }
    networkinfo.clear();
    networkinfo = linklist;
    RefreshData();
}
