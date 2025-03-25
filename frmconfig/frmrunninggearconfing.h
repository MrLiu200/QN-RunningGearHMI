#ifndef FRMRUNNINGGEARCONFING_H
#define FRMRUNNINGGEARCONFING_H
/*
 * 走行部设置界面
 * 功能：
 * 1、可以选择设备，也就是主从机列表,按车厢号划分
 * 2、应实现通信协议所包含的所有分类
 * 3、每个分类都应该有读取和设置按钮，对应通信协议
*/
#include <QDialog>
#include <QtCore>
#include <QToolButton>
#include <QLineEdit>
#include "predata.h"
#include "udpmulticastapi.h"
namespace Ui {
class frmRunningGearConfing;
}

class frmRunningGearConfing : public QDialog
{
    Q_OBJECT

public:
    static frmRunningGearConfing *Instance();
    explicit frmRunningGearConfing(QWidget *parent = nullptr);
    ~frmRunningGearConfing();

private:
    Ui::frmRunningGearConfing *ui;
    static frmRunningGearConfing *self;
    QString currentwagon;
    QString currentfile;
    QList<QLineEdit *> txtAlarmLimit;
    QList<QLineEdit *> txtCarInfo;
    QList<QLineEdit *> txtSaveInterval;
    QList<QLineEdit *> txtBearingInfo;
    QList<QLineEdit *> txtNetInfo;

    QList<QPushButton *> btnList;
    QList<QPushButton *> btnPre;
    QList<QPushButton *> btnLink;
    QList<QPushButton *> btnTask;
    QList<QPushButton *> btnBearing;


public:
    void InitFrom();
    void AddWagon(QString wagon);

private:
    void clearFrom();
    void SetbtnState(bool enable);
private slots:
    //车厢切换
    void WagonSwitch();
    //更新联动设备视图
    void UpdateLinkView(QString wagon,QStringList linklist);
    //背板状态更新视图
    void UpdateOtherBoardView(QString wagon,QStringList statelist);
    //定时任务更新视图
    void UpdatetaskView(QString wagon,QStringList tasklist);
    //车辆信息更新视图
    void UpdatecarView(QString project, int linenum, QString carnum, QString wagon,float WheelDiameter, QString type,double sppedcoefficient);
    //保存间隔更新视图
    void UpdatesaveIntervalView(QString wagon,int lowpowerspeedmax, int temspeedmax, bool IsEnable, int VibInterval, int TemInterval);
    //轴承信息更新视图
    void UpdateBearingView(QString wagon,QStringList bearingInfo);
    //网络信息更新视图
    void UpdatenetworkView(QString wagon,QStringList configlist);
    //报警阈值更新视图
    void UpdateAlarmlimitView(QString wagon, QStringList values);
    //软件版本更新视图
    void UpdateAppVersion(QString wagon, QString version);
    //获取该车厢轴承名称列表
    QStringList GetBearingModelList(QString wagon);


    void on_btn_preadd_clicked();
    void on_btn_premodify_clicked();
    void on_btn_predelete_clicked();
    void on_btn_SetSametype_clicked();
    void on_btn_setAll_clicked();
    void on_btn_getlinkinfo_clicked();
    void on_btn_addlinkdevice_clicked();
    void on_btn_modifylinkdevice_clicked();
    void on_btn_deletelinkdevice_clicked();
    void on_btn_getotherboard_clicked();
    void on_btn_gettask_clicked();
    void on_btn_addtask_clicked();
    void on_btn_modifytask_clicked();
    void on_btn_deletetask_clicked();
    void on_btn_getcarinfo_clicked();
    void on_btn_setcarinfo_clicked();
    void on_btn_getInterval_clicked();
    void on_btn_setInterval_clicked();
    void on_btn_getbearing_clicked();
    void on_btn_addbearing_clicked();
    void on_btn_modifybearing_clicked();
    void on_btn_deletebearing_clicked();
    void on_btn_getnetinfo_clicked();
    void on_btn_setnetinfo_clicked();
    void on_btn_getalarmlimit_clicked();
    void on_btn_setalarmlimit_clicked();
    void on_btn_reboot_clicked();
    void on_btn_DeviceUpgrade_clicked();
    void on_btn_getversion_clicked();
    void on_btn_Datadownload_clicked();
    void on_btn_clear_clicked();
    void on_cbox_bearing_currentIndexChanged(const QString &arg1);
    void on_btn_deleteData_clicked();
};

#endif // FRMRUNNINGGEARCONFING_H
