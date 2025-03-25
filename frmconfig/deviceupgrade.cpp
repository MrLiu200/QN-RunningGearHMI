#include "deviceupgrade.h"
#include "ui_deviceupgrade.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include "corehelper.h"
#include "udpmulticastapi.h"
DeviceUpgrade::DeviceUpgrade(QWidget *parent, QString windowstitle) :
    QDialog(parent),
    ui(new Ui::DeviceUpgrade),
    wagon(windowstitle)
{
    title = "设备升级";
    BlockSize = 40960;
    ui->setupUi(this);
    InitFrom();
    this->setFixedSize(450,350);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::UpdateProgress,this,&DeviceUpgrade::Updateprogress);
}

DeviceUpgrade::~DeviceUpgrade()
{
    delete ui;
}

void DeviceUpgrade::InitFrom()
{
    this->setWindowTitle(title + " 车厢号：" + wagon);

    //初始化前置列表
    QStringList prelist;
    //查找该车厢所包含的前置处理器ID
    for(auto device : PreData::devicelist){
        if(device.Wagon == wagon){
            prelist.append(QString::number(device.id));
        }
    }
    prelist.removeDuplicates();

    ui->cbox_Preid->addItem("全部前置");
    ui->cbox_Preid->addItems(prelist);

    //添加单选列表
    menubtns.append(ui->rbtn_Pre);
    menubtns.append(ui->rbtn_Speed);
    menubtns.append(ui->rbtn_PreIO);
    menubtns.append(ui->rbtn_command);
    menubtns.append(ui->rbtn_Host);
    //设置默认状态
    ui->cbox_Preid->setEnabled(false);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);

}

void DeviceUpgrade::LoadFileInfo()
{
    if(currentfile.isEmpty()) return;
    QFileInfo fileinfo(currentfile);
    QStringList infolist = fileinfo.baseName().split("_");
    if(infolist.size() < 5){
        QMessageBox::warning(nullptr,"选择出错","文件格式不正确,参考(项目名_板卡名_硬件版本号_软件版本号_日期)");
        return;
    }
    currentversion = infolist.at(3);
    ui->txt_filename->setText(fileinfo.fileName());
    currentsize = fileinfo.size();
    ui->txt_filesize->setText(QString::number(currentsize));
    currentcrc = CoreHelper::GetCRC32_STM32H750(currentfile);
    ui->txt_filecrc->setText(QString("0x%1").arg(currentcrc, 0, 16).toUpper());


}

void DeviceUpgrade::on_rbtn_Pre_clicked(bool checked)
{
    ui->cbox_Preid->setEnabled(checked);
}

void DeviceUpgrade::on_btn_start_clicked()
{
    if(currentfile.isEmpty()) return;

    //获取要升级的板卡
    QString currentboard;
    uint8_t boardnum;
    for(int i=0;i<menubtns.size();i++){
        if(menubtns.at(i)->isChecked()){
            boardnum = i+1;
        }
    }
    uint8_t id = 0;
    if(boardnum == 1){
         id = ui->cbox_Preid->currentIndex();
    }

    //计算总共包的数量
    PackSize = std::ceil(static_cast<double>(currentsize) / BlockSize);
//    currentversion = "Host002";
    UDPMulticastAPI::Instance()->UpdateSoftware(wagon, boardnum, currentfile, currentversion,id);
}

void DeviceUpgrade::on_btn_Openfile_clicked()
{
    ui->progressBar->setValue(0);
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select Upgrade File"),
                                 CoreHelper::APPPath(),
                                 tr("files(*.bin);;所有文件(*)"));
    if(fileName.isEmpty()){
        QMessageBox::warning(nullptr,"错误","请选择正确的文件");
        return;
    }else{
        currentfile = fileName;
    }
    LoadFileInfo();
}

void DeviceUpgrade::Updateprogress(int currentcount)
{
    int proportion = std::ceil(static_cast<double>(currentcount*100) / PackSize);
    if(proportion > 100){
        proportion = 100;
    }
    ui->progressBar->setValue(proportion);
}
