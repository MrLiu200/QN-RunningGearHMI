#include "frmrmupgrade.h"
#include "ui_frmrmupgrade.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QButtonGroup>
#include <QCheckBox>
#include "corehelper.h"
FrmRMUpgrade::FrmRMUpgrade(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMUpgrade)
{
    ui->setupUi(this);
    BlockSize = 1024;
    NeedInit = true;
    buttonGroup = new QButtonGroup(this);

}

FrmRMUpgrade::~FrmRMUpgrade()
{
    delete ui;
}

void FrmRMUpgrade::InitFrom()
{
    NeedInit = false;
    buttonGroup->setExclusive(true);
    LoadPre();
    //添加单选列表
    menubtns.append(ui->rbtn_Pre);
    menubtns.append(ui->rbtn_Speed);
    menubtns.append(ui->rbtn_PreIO);
    menubtns.append(ui->rbtn_command);
    menubtns.append(ui->rbtn_Host);
    //设置默认状态
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);

    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::UpdateProgress,this,&FrmRMUpgrade::Updateprogress);
    ui->widget_pre->setVisible(false);
}

void FrmRMUpgrade::LoadPre()
{
    //初始化前置列表
    QStringList prelist;
    //查找该车厢所包含的前置处理器ID
    for(auto device : PreData::devicelist){
        if(device.Wagon == Wagon){
            prelist.append(QString::number(device.id));
        }
    }
    prelist.removeDuplicates();

    if(prelist.isEmpty()) return;
    // 获取 gridLayout
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->widget_pre->layout());
    if (!gridLayout){
        gridLayout = new QGridLayout(ui->widget_pre);
    }
    QCheckBox *cbox_allpre = new QCheckBox(QString("全部前置处理器"));
    buttonGroup->addButton(cbox_allpre);
    gridLayout->addWidget(cbox_allpre, 0, 0);
    for(int i=0;i<prelist.size();i++){
        QCheckBox *checkBox = new QCheckBox(QString("设备id: %1").arg(prelist.at(i)));
        buttonGroup->addButton(checkBox);
        gridLayout->addWidget(checkBox, (i+1) / 2, (i+1) % 2);
    }
}

void FrmRMUpgrade::LoadFileInfo()
{
    if(upgradefileinfo.filename.isEmpty()) return;
    QFileInfo fileinfo(upgradefileinfo.filename);
    QStringList infolist = fileinfo.baseName().split("_");
    if(infolist.size() != 6){
        QMessageBox::warning(nullptr,"选择出错","文件格式不正确,参考(项目名_板卡名_硬件版本号_软件版本号_日期)");
        return;
    }
    upgradefileinfo.ProjectName = infolist.at(0);
    upgradefileinfo.boardName = infolist.at(1);
    upgradefileinfo.Version_hardware = infolist.at(2);
    upgradefileinfo.Version_software = infolist.at(3);
    upgradefileinfo.Date = QDateTime::fromString(infolist.at(4),"yyyyMMdd").toString("yyyy-MM-dd");
    upgradefileinfo.softwareType = infolist.at(5);
    upgradefileinfo.filesize = fileinfo.size();

    if(upgradefileinfo.filesize %4 == 0){
        upgradefileinfo.crc = CoreHelper::GetCRC32_STM32H750(upgradefileinfo.filename);
    }else{
        upgradefileinfo.filename.clear();
        QMessageBox::warning(nullptr,"文件出错","文件大小异常");
        return;
    }

    ui->txt_filename->setText(fileinfo.fileName());
    ui->txt_filesize->setText(QString::number(upgradefileinfo.filesize));
    ui->txt_filecrc->setText(QString("0x%1").arg(upgradefileinfo.crc, 0, 16).toUpper());
    ui->txt_projectname->setText(upgradefileinfo.ProjectName);
    ui->txt_boardname->setText(upgradefileinfo.boardName);
    ui->txt_hardwareversion->setText(upgradefileinfo.Version_hardware);
    ui->txt_appversion->setText(upgradefileinfo.Version_software);
    ui->txt_date->setText(upgradefileinfo.Date);
    ui->txt_Softwaretype->setText(upgradefileinfo.softwareType);
}

void FrmRMUpgrade::clearCboxPre()
{
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->widget_pre->layout());

    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    if(buttons.isEmpty()) return;
    for (QAbstractButton *button : buttons) {
        buttonGroup->removeButton(button);
        gridLayout->removeWidget(button);
        delete button;
    }
}

QString FrmRMUpgrade::GetCurrentWagon() const
{
    return this->Wagon;
}

void FrmRMUpgrade::SetCurrentWagon(const QString &wagon)
{
    //清空界面
    ui->txt_filename->clear();
    ui->txt_filename->clear();
    ui->txt_filesize->clear();
    ui->txt_filecrc->clear();
    ui->txt_projectname->clear();
    ui->txt_boardname->clear();
    ui->txt_hardwareversion->clear();
    ui->txt_appversion->clear();
    ui->txt_date->clear();
    ui->txt_Softwaretype->clear();
    this->Wagon = wagon;
    //初始化前置数量
    clearCboxPre();
    if(NeedInit){
        InitFrom();
    }else{
        LoadPre();
    }
}

void FrmRMUpgrade::RedrawData()
{
    LoadPre();
}

void FrmRMUpgrade::on_btn_save_clicked()
{
    if(upgradefileinfo.filename.isEmpty()) return;

    //获取要升级的板卡
    uint8_t boardnum;
    for(int i=0;i<menubtns.size();i++){
        if(menubtns.at(i)->isChecked()){
            boardnum = i+1;
        }
    }
    uint8_t id = 0;
    if(boardnum == 1){//前置处理器
         QAbstractButton *checkedButton = buttonGroup->checkedButton();
         if(checkedButton){
             QString text = checkedButton->text();
             if(text == ""){
                 id = 0;
             }else{
                 QStringList list1 = text.split(" ");
                 if(list1.size() > 1){
                     id = list1.at(1).toInt();
                 }
             }

         }else{
             QMessageBox::warning(nullptr,"升级","请选择正确的前置处理器");
             return;
         }
    }

    //计算总共包的数量
    PackSize = std::ceil(static_cast<double>(upgradefileinfo.filesize) / BlockSize);

    UDPMulticastAPI::Instance()->UpdateSoftware(Wagon, boardnum, upgradefileinfo.filename, upgradefileinfo.Version_software,id);
}

void FrmRMUpgrade::on_btn_Openfile_clicked()
{
    ui->progressBar->setValue(0);
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select Upgrade File"),
                                 CoreHelper::APPPath(),
                                 tr("files(*.bin);;所有文件(*)"));
    if(fileName.isEmpty()){
        return;
    }else{
        upgradefileinfo.filename = fileName;
    }
    LoadFileInfo();
}

void FrmRMUpgrade::Updateprogress(int currentcount)
{
    int proportion = std::ceil(static_cast<double>(currentcount*100) / PackSize);
    if(proportion > 100){
        proportion = 100;
    }
    ui->progressBar->setValue(proportion);
}

void FrmRMUpgrade::on_rbtn_Pre_toggled(bool checked)
{
    ui->widget_pre->setVisible(checked);
}
