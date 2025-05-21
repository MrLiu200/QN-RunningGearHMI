#include "frmrmconfig.h"
#include "ui_frmrmconfig.h"
#include <QFile>
#include <QLabel>
#include <QGridLayout>
#include <QFont>
#include "predata.h"
#include <QMessageBox>
#include "frmrmmeasurepoint.h"
#include "frmrmbearing.h"
#include "frmrmassociation.h"
#include "frmrmtimertask.h"
#include "frmrmnetwork.h"
#include "frmrmcarinfo.h"
#include "frmrmalarmlimit.h"
#include "frmrmdatasave.h"
#include "frmrmupgrade.h"
#include "frmboardstate.h"
FrmRMConfig::FrmRMConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMConfig)
{
    ui->setupUi(this);
    NeedSave = false;

    InitFrom();
    InitWagon();
    InitWidget();

}

FrmRMConfig::~FrmRMConfig()
{
    delete ui;
}

void FrmRMConfig::InitFrom()
{
    QFile file("D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/image/other/style1.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

    FunctionbtnGroup = new QButtonGroup(this);
    FunctionbtnGroup->setExclusive(true);

    WagonbtnGroup = new QButtonGroup(this);
    WagonbtnGroup->setExclusive(true);

    functionbtns = ui->widget_function->findChildren<QToolButton*>();
    for(auto btn : functionbtns){
        btn->setCheckable(true);
        FunctionbtnGroup->addButton(btn);
//        btn->setEnabled(false);
    }


    wagonbtns = ui->widget_wagon->findChildren<QToolButton*>();
    for(auto btn : wagonbtns){
        btn->setCheckable(true);
        WagonbtnGroup->addButton(btn);
    }
    ui->tbtn_measurePoint->setChecked(true);
    connect(FunctionbtnGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &FrmRMConfig::FunctionViewSwitch);
    connect(WagonbtnGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &FrmRMConfig::WagonSwitch);

}

void FrmRMConfig::InitWagon()
{
    int wagoncount = PreData::wagonsList.size();
    QList <QToolButton*> wagonbtns;
    wagonbtns.append(ui->toolButton_11);
    wagonbtns.append(ui->toolButton_12);
    wagonbtns.append(ui->toolButton_13);
    wagonbtns.append(ui->toolButton_14);
    wagonbtns.append(ui->toolButton_15);
    wagonbtns.append(ui->toolButton_16);

    for(int i=0;i<wagoncount;i++){
        wagonbtns.at(i)->setText(PreData::wagonsList.at(i));
    }

    if(wagoncount < wagonbtns.size()){
        for(int i=wagoncount;i<wagonbtns.size();i++){
            wagonbtns.at(i)->setVisible(false);
        }
    }
}

void FrmRMConfig::InitWidget()
{
    //测点配置
    FrmRMMeasurePoint *rmmeasurepoint = new FrmRMMeasurePoint;
    ui->stackedWidget->addWidget(rmmeasurepoint);
    //轴承配置
    FrmRMBearing *bearingconfig = new FrmRMBearing;
    ui->stackedWidget->addWidget(bearingconfig);
    //关联设备
    FrmRMAssociation *associationconfig = new FrmRMAssociation;
    ui->stackedWidget->addWidget(associationconfig);
    //定时任务
    FrmRMTimertask *timertaskconfig = new FrmRMTimertask;
    ui->stackedWidget->addWidget(timertaskconfig);
    //网络信息
    FrmRMNetwork *rmnetwork = new FrmRMNetwork;
    ui->stackedWidget->addWidget(rmnetwork);
    //车辆信息
    FrmRMCarInfo *rmcarinfo = new FrmRMCarInfo;
    ui->stackedWidget->addWidget(rmcarinfo);
    //报警阈值
    FrmRMAlarmLimit *rmalarmlimit = new FrmRMAlarmLimit;
    ui->stackedWidget->addWidget(rmalarmlimit);
    //数据保存
    FrmRMDataSave *rmdatasave = new FrmRMDataSave;
    ui->stackedWidget->addWidget(rmdatasave);
    //设备升级
    FrmRMUpgrade *rmupgrade = new FrmRMUpgrade;
    ui->stackedWidget->addWidget(rmupgrade);
    //板卡状态
    FrmBoardState *boardState = new FrmBoardState;
    ui->stackedWidget->addWidget(boardState);

    ui->stackedWidget->setCurrentIndex(0);


}

void FrmRMConfig::SubmitCurrentChanges()
{
    //找出当前的widget，并触发提交
    QWidget *currentwidget = ui->stackedWidget->currentWidget();
    if(FrmRMMeasurePoint *widget = qobject_cast<FrmRMMeasurePoint*>(currentwidget)){
            widget->SaveData();
            qDebug()<< "测点配置 保存";
    }else if(FrmRMBearing *widget = qobject_cast<FrmRMBearing*>(currentwidget)){
        widget->SaveData();
        qDebug()<< "轴承配置 保存";
    }else if(FrmRMAssociation *widget = qobject_cast<FrmRMAssociation*>(currentwidget)){
        widget->SaveData();
        qDebug()<< "关联设备 保存";
    }else if(FrmRMTimertask *widget = qobject_cast<FrmRMTimertask*>(currentwidget)){
        widget->SaveData();
        qDebug()<< "定时任务 保存";
    }else if(FrmRMNetwork *widget = qobject_cast<FrmRMNetwork*>(currentwidget)){
        widget->SaveData();
        qDebug()<< "网络配置 保存";
    }else if(FrmRMCarInfo *widget = qobject_cast<FrmRMCarInfo*>(currentwidget)){
        widget->SaveData();
        qDebug()<< "车辆信息 保存";
    }else if(FrmRMAlarmLimit *widget = qobject_cast<FrmRMAlarmLimit*>(currentwidget)){
        widget->SaveData();
        qDebug()<< "报警阈值 保存";
    }else if(FrmRMDataSave *widget = qobject_cast<FrmRMDataSave*>(currentwidget)){
        widget->SaveData();
        qDebug()<< "保存数据间隔 保存";
    }

}

void FrmRMConfig::CheckWagon()
{
    if(selectedwagon.isEmpty()) return;
    QWidget *currentwidget = ui->stackedWidget->currentWidget();
    if(FrmRMMeasurePoint *widget = qobject_cast<FrmRMMeasurePoint*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }else if(FrmRMBearing *widget = qobject_cast<FrmRMBearing*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }else if(FrmRMAssociation *widget = qobject_cast<FrmRMAssociation*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }else if(FrmRMTimertask *widget = qobject_cast<FrmRMTimertask*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }else if(FrmRMNetwork *widget = qobject_cast<FrmRMNetwork*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }else if(FrmRMCarInfo *widget = qobject_cast<FrmRMCarInfo*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }else if(FrmRMAlarmLimit *widget = qobject_cast<FrmRMAlarmLimit*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }else if(FrmRMDataSave *widget = qobject_cast<FrmRMDataSave*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }else if(FrmRMUpgrade *widget = qobject_cast<FrmRMUpgrade*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }else if(FrmBoardState *widget = qobject_cast<FrmBoardState*>(currentwidget)){
        if(widget->GetCurrentWagon() != selectedwagon){
            widget->SetCurrentWagon(selectedwagon);
        }else{
            widget->RedrawData();
        }
    }
}

void FrmRMConfig::RMReboot()
{
    if(selectedwagon.isEmpty()) return;
    if(QMessageBox::question(this,"二次确认",QString("确认重启设备 %1 吗").arg(selectedwagon)) == QMessageBox::Yes){
//        UDPMulticastAPI::Instance()->DeviceReboot(selectedwagon);
        qDebug()<<"设备重启已经确认,车厢号：" << selectedwagon;
    }

    FunctionbtnGroup->setExclusive(false);
    FunctionbtnGroup->checkedButton()->setChecked(false);
    FunctionbtnGroup->setExclusive(true);

}

void FrmRMConfig::FunctionViewSwitch(QAbstractButton *btn)
{

    QString str = btn->text();
    if(str == currentFunction){
        return;
    }
    SubmitCurrentChanges();
    if(str == "测点配置"){
        ui->stackedWidget->setCurrentIndex(0);
    }else if(str == "轴承配置"){
        ui->stackedWidget->setCurrentIndex(1);
    }else if(str == "关联设备"){
        ui->stackedWidget->setCurrentIndex(2);
    }else if(str == "定时任务"){
        ui->stackedWidget->setCurrentIndex(3);
    }else if(str == "网络信息"){
        ui->stackedWidget->setCurrentIndex(4);
    }else if(str == "车辆信息"){
        ui->stackedWidget->setCurrentIndex(5);
    }else if(str == "报警阈值"){
        ui->stackedWidget->setCurrentIndex(6);
    }else if(str == "数据保存"){
        ui->stackedWidget->setCurrentIndex(7);
    }else if(str == "设备升级"){
        ui->stackedWidget->setCurrentIndex(8);
    }else if(str == "板卡状态"){
        ui->stackedWidget->setCurrentIndex(9);
    }else if(str == "设备重启"){
        RMReboot();
    }


    if(str != "设备重启"){
        currentFunction = str;
    }

    //检查是否需要设置车厢
    CheckWagon();
}

void FrmRMConfig::WagonSwitch(QAbstractButton *btn)
{
    if(!selectedwagon.isEmpty()){
        SubmitCurrentChanges();
    }
    selectedwagon = btn->text();

    //查找当前的widget,并把车厢更新进去
    QWidget *currentwidget = ui->stackedWidget->currentWidget();
    if(FrmRMMeasurePoint *widget = qobject_cast<FrmRMMeasurePoint*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为测点配置";
    }else if(FrmRMBearing *widget = qobject_cast<FrmRMBearing*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为轴承配置";
    }else if(FrmRMAssociation *widget = qobject_cast<FrmRMAssociation*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为关联设备配置";
    }else if(FrmRMTimertask *widget = qobject_cast<FrmRMTimertask*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为定时任务配置";
    }else if(FrmRMNetwork *widget = qobject_cast<FrmRMNetwork*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为网络信息配置";
    }else if(FrmRMCarInfo *widget = qobject_cast<FrmRMCarInfo*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为车辆信息配置";
    }else if(FrmRMAlarmLimit *widget = qobject_cast<FrmRMAlarmLimit*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为报警阈值配置";
    }else if(FrmRMDataSave *widget = qobject_cast<FrmRMDataSave*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为数据保存间隔配置";
    }else if(FrmRMUpgrade *widget = qobject_cast<FrmRMUpgrade*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为设备升级配置";
    }else if(FrmBoardState *widget = qobject_cast<FrmBoardState*>(currentwidget)){
        widget->SetCurrentWagon(selectedwagon);
        qDebug()<< "当前为板卡状态界面";
    }
}
