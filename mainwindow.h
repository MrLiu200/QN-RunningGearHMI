#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include "myhelper.h"
#include "predata.h"
#include "frmrealtimedebug.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_check_carriage_clicked(bool checked);

    void on_check_point_clicked(bool checked);
    //重启
    void restartApplication();
    //查询排列测点
    void QueryDevice();
    //about
    void AboutWidget();
    //help
    void helpdocWidget();
    //用户切换
    void userchange();
    //用户添加
    void userAdd();
    //删除用户
    void deleteuser();
    //打开趋势图
    void OpenTrend();
    //右键打开趋势图
    void listviewopentrend(QString info,bool IsEnvelope);
    //打开波形图
    void OpenWaveform();
    //右键打开波形/包络图
    void listviewopenwaveform(QString info, bool IsEnvelope);
    //打开包络图
    void OpenLivedemodulatedView();
    //打开包络量纲趋势图
    void opendemodulatedview();
    //打开本地日志
    void OpenSystemlog();
    //打开走行部日志
    void Openremotelog();
    //打开本地配置
    void OpenSystemConfig();
    //打开走行部配置
    void OpenremoteConfig();
    //测试专用槽
    void TestSLOT();
    //打开实时界面
    void OpenRealtimeView();
    //刷新整个设备列表(获取到设备列表后,用于初始化阶段)
    void RefreshDeviceList(QString wagon, bool Isend = false);
    //设置前置状态
    void SetItemState(QString wagon, QString name,PreData::EnumDeviceState state);
    void UpdateItem(QString wagon, QString name,PreData::EnumDeviceState state,QString tip = QString());
    //右键清除报警
    void user_ClearAlarm(QString text);
    //设备报警
    void DeviceAlarm(QString wagon, int id, int ch, quint8 grade, QString alarmtype);
    //嵌入式设备上线
    void DeviceGoOnline(QString wagon);
    //嵌入式设备离线
    void DeviceGoOffline(QString wagon);
    //按条件筛选内容
    void on_btn_query_clicked();
    //显示全部内容
    void on_btn_all_clicked();
    //走行部数据库打开情况
    void OpenRunDB(bool ok);
    //连接走行部
    void connectRm();
    //获取配置文件
    void Rm_Getconfig();
    //获取数据库文件
    void Rm_GetDBfile();
    //数据下载
    void Rm_downloaddata();
    //数据删除
    void Rm_ClearZipData();
    //离线配置导入
    void importConfig();
    //离线数据库导入
    void importSQL();
    //离线数据导入
    void importOriginData();
    //离线数据导入结果返回
    void ImportOriginDataResult(bool ok);
    //获取测点最新状态
    void on_btn_flushstate_clicked();
    //设置全局时间区间
    void SetEchotime();
    //截图
    void StartScreen();
    //对比图
    void OpenPkView();

private:
    Ui::MainWindow *ui;
    void InitFrom();
    void InitCode();
    void InitOtherTask();
    void InitView();
    int findTabByName(QString name);

};

#endif // MAINWINDOW_H
