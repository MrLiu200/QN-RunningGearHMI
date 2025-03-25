#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QTextCodec>
#include <QTranslator>
#include <QDebug>
#include "frmabout.h"
#include "frmhelp/frmhelpdoc.h"
#include "frmuserchange.h"
#include "frmuseradd.h"
#include "frmhistoricaltrends.h"
#include "frmlivepreview.h"
#include "frmdatalog.h"
#include "frmsystemlog.h"
#include "frmsystemconfig.h"
#include "ftpclient.h"
#include "corehelper.h"
#include "m_dbapi.h"
#include "frmrunninggearconfing.h"
#include "quazip.h"
#include "quazipfile.h"
#include "JlCompress.h"
#include "m_dirinit.h"
#include "importofflinefile.h"
#include "frmsetechotime.h"
#include "m_screenshot.h"
#include "datadownload.h"
#include "frmrmconfig.h"
#include "frmrmmeasurepoint.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    App::ConfigFile = CoreHelper::APPPath() + "/HMI.ini";
    App::readConfig();
    if(!M_DirInit::Instance()->dirInit()){
        QMessageBox::warning(this,"日志错误","临时文件夹创建错误，请检查系统权限");
    }
    setStyleSheet("QToolTip { background-color: #c8a6b9; color: black; border: 1px solid gray; }");

    InitCode();
    InitFrom();
    InitView();
    InitOtherTask();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitFrom()
{
    //设置标题
    this->setWindowTitle("走行部管理软件");

    //添加菜单栏
    QMenu *Rmmenu = ui->menubar->addMenu("走行部(&G)");
    QMenu *OfflineDataMenu = ui->menubar->addMenu("离线数据(&I)");
    QAction *mainmenu = ui->menubar->addAction("测试按钮(&T)");
    QAction *debugmenu = ui->menubar->addAction("实时详情(&R)");
    QMenu *configmenu = ui->menubar->addMenu("设置(&C)");
    QMenu *logmenu = ui->menubar->addMenu("日志查询(&L)");
    QMenu *usermenu = ui->menubar->addMenu("用户管理(&U)");
    QAction *rebootmenu = ui->menubar->addAction("重启");
    QMenu *helpmenu = ui->menubar->addMenu("帮助(&H)");

    QAction *connectAction = Rmmenu->addAction(QIcon(":/image/image/Icon/connect.ico"),"连接");
    QAction *getconfigAction = Rmmenu->addAction(QIcon(":/image/image/Icon/config.ico"),"获取配置文件");
    QAction *getDBAction = Rmmenu->addAction(QIcon(":/image/image/Icon/db.ico"),"获取数据库文件");
    QAction *downloadData = Rmmenu->addAction(QIcon(":/image/image/Icon/download.ico"),"数据下载");
    QAction *clearData = Rmmenu->addAction("删除所有数据");

    QAction *importConfigAction = OfflineDataMenu->addAction(QIcon(":/image/image/Icon/newconfig.ico"),"配置导入");
    QAction *importSqlDbAction = OfflineDataMenu->addAction(QIcon(":/image/image/Icon/newdatabase.ico"),"数据库导入");
    QAction *importDataAction = OfflineDataMenu->addAction("原始数据导入");

    QAction *systemAction = configmenu->addAction("本机设置");
    QAction *hostAction = configmenu->addAction("走行部设置");

    QAction *systemlogAction = logmenu->addAction("本机日志");
    QAction *remotelogAction = logmenu->addAction("走行部日志");

    QAction *switchAction = usermenu->addAction("切换");
    QAction *adduserAction = usermenu->addAction(QIcon(QPixmap(":/image/image/Icon/user_add.png")),"新增");
    QAction *LogoutAction = usermenu->addAction(QIcon(QPixmap(":/image/image/Icon/delete.png")),"删除");

    QAction *helpAction = helpmenu->addAction(QIcon(QPixmap(":/image/image/Icon/User_Manual.png")),"帮助文档");
    QAction *aboutAction = helpmenu->addAction(QIcon(QPixmap(":/image/image/Icon/about.png")),"关于");


    //工具栏---未完成
    QToolBar *toolBar = new QToolBar(this);
    addToolBar(toolBar);
    QAction *TrendAction = toolBar->addAction(QIcon(":/image/image/Icon/1.png"),"量纲趋势图");
    QAction *WaveformAction = toolBar->addAction(QIcon(":/image/image/Icon/2.png"),"时域波形、频谱、趋势图");
    QAction *livedemodulatedAction = toolBar->addAction(QIcon(":/image/image/Icon/livedemodulated.png"),"包络波形、频谱、趋势图");
    QAction *demodulatedview = toolBar->addAction(QIcon(":/image/image/Icon/demodulated.png"),"包络量纲趋势图");
    QAction *setTimeIntervalAction = toolBar->addAction(QIcon(":/image/image/Icon/SetTimeInterval.png"),"数据时间区间");
    QAction *ScreenshotAction = toolBar->addAction(QIcon(":/image/image/Icon/screenshot.png"),"截图");
    QAction *PkAction = toolBar->addAction(QIcon(":/image/image/Icon/pk.png"),"对比");

    //填充combox
    QStringList list;
    list << "轴箱" << "电机驱动端" << "电机非驱动端" <<"齿轮箱";
    ui->combox_point->addItems(list);
    QStandardItemModel *model = new QStandardItemModel(this);
    ui->listView->setModel(model);

    //底部状态栏
#if 0
    QLabel *timeLabel = new QLabel(this);
    timeLabel->setText("    欢迎使用");
   ui->statusbar->addWidget(timeLabel);
   ui->statusbar->addWidget(new QLabel("这是一个测试的东西"));
   ui->statusbar->addWidget(new QLabel("第三个label"));
   ui->statusbar->showMessage("欢迎使用",5000);
   QLabel *testlabel = new QLabel(this);
   testlabel->setFrameStyle(QFrame::Box | QFrame::Sunken);
   testlabel->setText("上海诠诺物联网技术有限公司");
   ui->statusbar->addPermanentWidget(testlabel);
    // 创建 QLabel 显示时间
    QLabel *timeLabel = new QLabel(this);
    timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);  // 设置对齐方式

    // 将 timeLabel 添加到 statusBar，设置 stretch 权重
    QWidget *spacer = new QWidget();
    statusBar()->addWidget(spacer, 50); // 空白控件，stretch=10，推开时间标签
    statusBar()->addWidget(timeLabel,1); // stretch=1, 使得时间标签位于右边, 1

    // 设置一个定时器，每秒更新一次
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [timeLabel]() {
        // 获取当前时间并设置到 label
        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        timeLabel->setText(currentTime);
    });

    // 启动定时器，更新时间
    timer->start(1000);  // 每1000ms（即1秒）更新时间
    statusBar()->showMessage("上海诠诺物联网技术有限公司");
#endif

    //关联信号槽Rm_Getconfig
    connect(connectAction,&QAction::triggered,this,&MainWindow::connectRm);
    connect(getconfigAction,&QAction::triggered,this,&MainWindow::Rm_Getconfig);
    connect(getDBAction,&QAction::triggered,this,&MainWindow::Rm_GetDBfile);
    connect(downloadData,&QAction::triggered,this,&MainWindow::Rm_downloaddata);
    connect(clearData,&QAction::triggered,this,&MainWindow::Rm_ClearZipData);

    connect(importConfigAction,&QAction::triggered,this,&MainWindow::importConfig);
    connect(importSqlDbAction,&QAction::triggered,this,&MainWindow::importSQL);
    connect(importDataAction,&QAction::triggered,this,&MainWindow::importOriginData);

    connect(mainmenu,&QAction::triggered,this,&MainWindow::TestSLOT);

    connect(debugmenu,&QAction::triggered,this,&MainWindow::OpenRealtimeView);

    connect(rebootmenu,&QAction::triggered,this,&MainWindow::restartApplication);
    connect(aboutAction,&QAction::triggered,this,&MainWindow::AboutWidget);
    connect(helpAction,&QAction::triggered,this,&MainWindow::helpdocWidget);
    connect(switchAction,&QAction::triggered,this,&MainWindow::userchange);
    connect(adduserAction,&QAction::triggered,this,&MainWindow::userAdd);
    connect(LogoutAction,&QAction::triggered,this,&MainWindow::deleteuser);
    connect(TrendAction,&QAction::triggered,this,&MainWindow::OpenTrend);
    connect(WaveformAction,&QAction::triggered,this,&MainWindow::OpenWaveform);
    connect(livedemodulatedAction,&QAction::triggered,this,&MainWindow::OpenLivedemodulatedView);
    connect(demodulatedview,&QAction::triggered,this,&MainWindow::opendemodulatedview);
    connect(setTimeIntervalAction,&QAction::triggered,this,&MainWindow::SetEchotime);
    connect(ScreenshotAction,&QAction::triggered,this,&MainWindow::StartScreen);
    connect(PkAction,&QAction::triggered,this,&MainWindow::OpenPkView);
    connect(systemlogAction,&QAction::triggered,this,&MainWindow::OpenSystemlog);
    connect(remotelogAction,&QAction::triggered,this,&MainWindow::Openremotelog);
    connect(systemAction,&QAction::triggered,this,&MainWindow::OpenSystemConfig);
    connect(hostAction,&QAction::triggered,this,&MainWindow::OpenremoteConfig);


    connect(ui->btn_query,&QPushButton::clicked,this,&MainWindow::QueryDevice);
    connect(ui->listView,&CustomListView::OpenTrend,this,&MainWindow::listviewopentrend);
    connect(ui->listView,&CustomListView::OpenWaveform,this,&MainWindow::listviewopenwaveform);
    connect(ui->listView,&CustomListView::clearAlarm,this,&MainWindow::user_ClearAlarm);


    connect(ImportOfflineFile::Instance(),&ImportOfflineFile::importdataresult,this,&MainWindow::ImportOriginDataResult);
    //默认设置
    ui->combox_carriage->setEnabled(false);
    ui->combox_point->setEnabled(false);
}

void MainWindow::InitCode()
{
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);

    QTranslator *qtTranslator = new QTranslator(qApp);
    qtTranslator->load(":/other/image/other/qt_zh_CN.qm");
    qApp->installTranslator(qtTranslator);

    QTranslator *widgetTranslator = new QTranslator(qApp);
    widgetTranslator->load(":/other/image/other/widgets.qm");
    qApp->installTranslator(widgetTranslator);
}

void MainWindow::InitOtherTask()
{

    connect(M_DbAPI::Instance(),&M_DbAPI::OpenDataDB,this,&MainWindow::OpenRunDB);

    void ConfigFileDownloadfinal();
    //网络通信初始化
    M_NetworkAPI::Instance()->Start();
    M_DbAPI::Instance()->ConnectLocalDB();
    connect(M_NetworkAPI::Instance(),&M_NetworkAPI::DeviceListChanged,this,&MainWindow::RefreshDeviceList);


    if(UDPMulticastAPI::Instance()->startJoin()){
        qDebug()<<"udp Init OK";
        connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::PreStateChange,this,&MainWindow::SetItemState);
        connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::DeviceAlarm,this,&MainWindow::DeviceAlarm);
        connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::DeviceGoOnline,this,&MainWindow::DeviceGoOnline);
    }
}

void MainWindow::InitView()
{
    frmRunningGearConfing::Instance()->InitFrom();
}

int MainWindow::findTabByName(QString name)
{
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        if (ui->tabWidget->tabText(i) == name) {
            return i; // 返回已存在的索引
        }
    }
    return -1; // 未找到
}

void MainWindow::SetItemState(QString wagon, QString name, PreData::EnumDeviceState state)
{
    UpdateItem(wagon,name,state);
}

void MainWindow::UpdateItem(QString wagon, QString name, PreData::EnumDeviceState state, QString tip)
{
    //拼接
    QString newtext = QString("%1(车厢:%2)").arg(name).arg(wagon);

    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->listView->model());
    // 遍历模型中的所有项
    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem *item = model->item(row);
        QString itemname = item->text();
        if (item && (itemname == newtext)) {
            QPixmap m_pixmap(20,20);
            QString newtate;
            switch (state) {
            case PreData::DeviceState_Online:
            {
                m_pixmap.fill(Qt::green);
                newtate = "状态: 在线";
            }
                break;
            case PreData::DeviceState_Offline:
            {
                m_pixmap.fill(Qt::darkGray);
                newtate = "状态: 离线";
            }
                break;
            case PreData::DeviceState_Waring:
            {
                m_pixmap.fill(Qt::cyan);
                newtate = "状态: 预警";
            }
                break;
            case PreData::DeviceState_FirstAlarm:
            {
                m_pixmap.fill(Qt::yellow);
                newtate = "状态: 一级报警";
            }
                break;
            case PreData::DeviceState_Secondary:
            {
                m_pixmap.fill(Qt::red);
                newtate = "状态: 二级报警";
            }
                break;
            case PreData::DeviceState_Fault:
            {
                m_pixmap.fill(QColor("#3B210A"));
                newtate = "状态：故障";
            }
            default:
                break;
            }
            item->setIcon(m_pixmap);

            //更新tips (id ch wagon enable state info)
            QStringList list = item->toolTip().split("\n");
            if(list.size() > 5){
                //状态字节
                list[4] = newtate;

                if(!tip.isEmpty()){//报警信息
                    QString oldtip = list.at(5);
                    QString newtip;
                    if(oldtip != "无报警"){
                        newtip = oldtip + tip;
                    }else{
                        newtip = tip;
                    }
                    list[5] = newtip;
                }
                item->setToolTip(list.join("\n"));
            }

            // 如果需要，手动更新视图
            model->dataChanged(model->index(row, 0), model->index(row, 0));
            ui->listView->update();
            return;  // 找到后可以跳出循环
        }
    }
}

void MainWindow::user_ClearAlarm(QString text)
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->listView->model());
    // 遍历模型中的所有项
    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem *item = model->item(row);
        QString itemname = item->text();
        if (item && (itemname == text)) {
            //通过tip取出前置信息(id ch wagon enable state info)
            QStringList list = item->toolTip().split("\n");
            if(list.size()!=6) break;
            //ID
            QStringList IDtip = list.at(0).split(":");
            if(IDtip.size() != 2) break;
            int id = IDtip.at(1).trimmed().toInt();
            //ch
            QStringList CHtip = list.at(1).split(":");
            if(CHtip.size() != 2) break;
            int ch = CHtip.at(1).trimmed().toInt();
            //wagon
            QStringList wagontip = list.at(2).split(":");
            if(wagontip.size() != 2) break;
            QString wagon = wagontip.at(1).trimmed();

            //设置状态
            bool Isenable = false;
            for(int i=0;i<PreData::devicelist.size();i++){
                auto device = PreData::devicelist.at(i);
                if((device.Wagon == wagon) && (device.id == id ) && (device.ch == ch)){
                    PreData::EnumDeviceState state = device.IsEnable ? PreData::DeviceState_Online : PreData::DeviceState_Offline;
                    PreData::devicelist[i].state = state;
                    Isenable = device.IsEnable;
                    break;
                }
            }
            UDPMulticastAPI::Instance()->UpdateAlarmLed(wagon,id,ch);
            //设置图标
            QPixmap m_pixmap(20,20);
            m_pixmap.fill(Isenable ? Qt::green : Qt::darkGray);
            item->setIcon(m_pixmap);

            //更新tips
            list.clear();
            list = item->toolTip().split("\n");
            if(list.size() > 5){
                //状态字节
                list[4] = Isenable ? "状态: 在线" : "状态: 离线";
                //报警info
                list[5] = "无报警";
                item->setToolTip(list.join("\n"));
            }

            //手动更新视图
            model->dataChanged(model->index(row, 0), model->index(row, 0));
            ui->listView->update();
            return;  // 找到后可以跳出循环
        }
    }
}

void MainWindow::DeviceAlarm(QString wagon, int id, int ch, quint8 grade, QString alarmtype)
{
    //    qDebug()<<"MainWindow::DeviceAlarm";
    if(grade > 2) return;
    PreData::EnumDeviceState state = PreData::DeviceState_Online;
    QString alarmstr;
    if(grade == 0){
        state = PreData::DeviceState_Waring;
        alarmstr = "预警";
    }else if(grade == 1){
        state = PreData::DeviceState_FirstAlarm;
        alarmstr = "一级报警";
    }else if(grade == 2){
        state = PreData::DeviceState_Secondary;
        alarmstr = "二级报警";
    }
    QString devicename,tips;
    for(int i=0;i<PreData::devicelist.size();i++){
        auto device = PreData::devicelist.at(i);
        if((device.Wagon == wagon) && (device.id == id ) && (device.ch == ch)){
            devicename = device.name;
            //如果设备掉线，则不处理，对吧？
            if(device.state != PreData::DeviceState_Offline){
                PreData::devicelist[i].state = state;
                tips = QString("%1%2").arg(alarmtype).arg(alarmstr);
            }else{
                return;
            }
            break;
        }
    }

    //添加至滚动信息框，报警信息
    QString text = QString("%1(%2): %3,等级：%4").arg(devicename).arg(wagon).arg(alarmtype).arg(alarmstr);
    qDebug()<< text;

    UpdateItem(wagon,devicename,state,tips);
}

void MainWindow::DeviceGoOnline(QString wagon)
{
    int len = PreData::devicelist.size();
    for(int i=0;i<len;i++){
        auto &device = PreData::devicelist[i];
        if((device.Wagon == wagon) && (device.state == PreData::DeviceState_Offline)){
            device.state = PreData::DeviceState_Online;
        }
    }
}

void MainWindow::DeviceGoOffline(QString wagon)
{
    int len = PreData::devicelist.size();
    for(int i=0;i<len;i++){
        auto &device = PreData::devicelist[i];
        if((device.Wagon == wagon) && (device.state != PreData::DeviceState_Offline)){
            device.state = PreData::DeviceState_Offline;
        }
    }
}

void MainWindow::on_check_carriage_clicked(bool checked)
{
    if(checked){
        ui->check_point->setChecked(false);
    }
    ui->combox_carriage->setEnabled(checked);
}

void MainWindow::on_check_point_clicked(bool checked)
{
    if(checked){
        ui->check_carriage->setChecked(false);
    }
    ui->combox_point->setEnabled(checked);
}

void MainWindow::restartApplication()
{
    qApp->exit();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}

void MainWindow::QueryDevice()
{
    ui->listView->onGetSelectedItems();
}

void MainWindow::AboutWidget()
{
    frmabout::Instance()->show();
}

void MainWindow::helpdocWidget()
{
    QString tabtitle = "帮助文档";
    int index = findTabByName(tabtitle);
    if(index == -1){
        frmhelpdoc *doc = new frmhelpdoc;
        index = ui->tabWidget->addTab(doc,"帮助文档");
    }
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::userchange()
{
    frmUserChange *changewidget = new frmUserChange(this);
    changewidget->setAttribute(Qt::WA_DeleteOnClose);  // 设置关闭时删除
    changewidget->exec();
}

void MainWindow::userAdd()
{
    frmUserAdd *addwidget = new frmUserAdd;
    addwidget->setAttribute(Qt::WA_DeleteOnClose);  // 设置关闭时删除
    addwidget->exec();
}

void MainWindow::deleteuser()
{
    if(QMessageBox::question(this,"提示",QString("确认删除用户:%1 ? \n删除后将重启软件").arg(App::CurrentUser)) == QMessageBox::Yes){
        QString deletestr = "DELETE FROM UserInfo WHERE User = :user";
        QSqlQuery query(M_DbAPI::Instance()->GetLocalDBHandle());
        query.prepare(deletestr);
        query.bindValue(":user",App::CurrentUser);
        if(query.exec()){
            CoreHelper::Reboot();
        }else{
            QMessageBox::warning(this,"删除用户","用户删除失败，请稍后重试");
        }
    }
}

void MainWindow::OpenTrend()
{
    QStringList selectlist = ui->listView->onGetSelectedItems();
    //先获取选中的项
    //    QStringList selectlist
    if(selectlist.isEmpty()) return;
    QString name = selectlist.join(",");
    if(selectlist.size() != 1){
        name += "对比趋势图";
    }else{
        name += "趋势图";
    }

    //判断是否存在窗口，如存在则跳转
    int index = findTabByName(name);
    if(index == -1){//如果没有，需要创建
        frmHistoricalTrends *history = new frmHistoricalTrends(selectlist);
        history->SetWidgettitle(name);
        index = ui->tabWidget->addTab(history,name);
    }
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::listviewopentrend(QString info,bool IsEnvelope)
{
    if(info.isEmpty()) return;
    QStringList selectlist;
    selectlist << info;
    QString suffix = "趋势图";
    if(IsEnvelope){
        suffix = "包络趋势图";
    }
    QString name = info + suffix;
    //判断是否存在窗口，如存在则跳转
    int index = findTabByName(name);
    if(index == -1){//如果没有，需要创建
        frmHistoricalTrends *history = new frmHistoricalTrends(selectlist,IsEnvelope);
        history->SetWidgettitle(name);
        index = ui->tabWidget->addTab(history,name);
    }
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::OpenWaveform()
{
    //先获取选中的项
    QStringList selectlist = ui->listView->onGetSelectedItems();
    if(selectlist.isEmpty()) return;
    QString name = selectlist.join(",");
    if(selectlist.size() != 1){
        return;
    }else{
        name += "波形图";
    }
    //判断是否存在窗口，如存在则跳转
    int index = findTabByName(name);
    if(index == -1){//如果没有，需要创建
        frmLivePreview *liveview = new frmLivePreview(selectlist);
        //        history->SetWidgettitle(name);
        index = ui->tabWidget->addTab(liveview,name);
    }

    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::listviewopenwaveform(QString info,bool IsEnvelope)
{
    if(info.isEmpty()) return;
    QStringList selectlist;
    selectlist << info;
    QString suffix = "波形图";
    if(IsEnvelope){
        suffix = "包络图";
    }
    QString name = info + suffix;
    //判断是否存在窗口，如存在则跳转
    int index = findTabByName(name);
    if(index == -1){//如果没有，需要创建
        frmLivePreview *liveview = new frmLivePreview(selectlist,IsEnvelope);
        index = ui->tabWidget->addTab(liveview,name);
    }
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::OpenLivedemodulatedView()
{
    QStringList selectlist = ui->listView->onGetSelectedItems();
    if(selectlist.isEmpty()) return;
    QString name = selectlist.join(",");
    if(selectlist.size() != 1){
        return;
    }else{
        name += "包络图";
    }
    //判断是否存在窗口，如存在则跳转
    int index = findTabByName(name);
    if(index == -1){//如果没有，需要创建
        frmLivePreview *liveview = new frmLivePreview(selectlist,true);
        index = ui->tabWidget->addTab(liveview,name);
    }

    ui->tabWidget->setCurrentIndex(index);
}


void MainWindow::opendemodulatedview()
{
    //先获取选中的项
    QStringList selectlist = ui->listView->onGetSelectedItems();
    if(selectlist.isEmpty()) return;
    QString name = selectlist.join(",");
    if(selectlist.size() != 1){
        name += "对比包络趋势图";
    }else{
        name += "包络趋势图";
    }

    //判断是否存在窗口，如存在则跳转
    int index = findTabByName(name);
    if(index == -1){//如果没有，需要创建
        frmHistoricalTrends *history = new frmHistoricalTrends(selectlist,true);
        history->SetWidgettitle(name);
        index = ui->tabWidget->addTab(history,name);
    }
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::OpenSystemlog()
{
    //判断窗口是否存在
    QString str = "本机日志";
    int index = findTabByName(str);
    if(index == -1){
        frmSystemLog *systemlogview = new frmSystemLog;
        index = ui->tabWidget->addTab(systemlogview,str);
    }

    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::Openremotelog()
{
    //判断窗口是否存在
    QString str = "走行部日志";
    int index = findTabByName(str);
    if(index == -1){
        frmDataLog *remoteview = new frmDataLog;
        index = ui->tabWidget->addTab(remoteview,str);
    }
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::OpenSystemConfig()
{
    frmSystemConfig::Instance()->show();
}

void MainWindow::OpenremoteConfig()
{
#if 0
    frmRunningGearConfing::Instance()->show();
#else
    QString str = "走行部配置";
    int index = findTabByName(str);
    if(index == -1){
         FrmRMConfig *RmConfigView = new FrmRMConfig;
        index = ui->tabWidget->addTab(RmConfigView,str);

    }
    ui->tabWidget->setCurrentIndex(index);
#endif
}

void MainWindow::TestSLOT()
{
#if 0
    //FTP 测试
    connect(FtpClient::Instance(),&FtpClient::DownloadProcess,this,[=](uint64_t current, uint64_t Total){
        qDebug()<< "已下载字节数" << current << "总字节数:" << Total;
    });
    connect(FtpClient::Instance(),&FtpClient::UploadProcess,this,[=](uint64_t current, uint64_t Total){
        qDebug()<< "已上传字节数" << current << "总字节数:" << Total;
    });
    // 下载

    QString ftpDownloadUrl = "/ftptest/RunningGear.db";
    QString downloadurl = FtpClient::Instance()->GetDownloadFile();
    downloadurl = downloadurl + "/" + "RunningGear.db";
    FtpClient::Instance()->SetFtpServerFile(ftpDownloadUrl);
    FtpClient::Instance()->SetDownloadFile(downloadurl);
    FtpClient::Instance()->get();
    //上传

    QString ftpDownloadUrl = "/ftptest/history.mp4";
    FtpClient::Instance()->SetFtpServerFile(ftpDownloadUrl);
    QString uploadurl = FtpClient::Instance()->GetUploadFile();
    uploadurl = uploadurl + "/" + "history.mp4";
    FtpClient::Instance()->SetUploadFile(uploadurl);
    FtpClient::Instance()->put();
#endif
#if 0
    // TCP 测试
    M_DbAPI::Instance()->ReceiveDbFile(CoreHelper::APPPath() + "/" + "RunningGear.db");
    bool ok = M_TcpAPI::Instance()->ConnectServer();
    if(ok){
        M_TcpAPI::Instance()->GetConfigIni();
        // 延迟 50ms 后发送下一个包
        QTimer::singleShot(50, M_TcpAPI::Instance(), &M_TcpAPI::GetDBFile);
    }else{
        qDebug()<<" tcp connect server fail";
    }
#endif
#if 0
    //文件过滤测试+压缩
    QString datadir = CoreHelper::APPPath() + "/"+ App::TempDir;
    QDir dir(datadir);

    // 查找符合条件的文件（数据类型_车厢号_前置ID_通道号_时间.bin）
    QStringList filters;
    filters << "*.bin"; // 查找所有 .bin 文件
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files);
    QFileInfoList files = dir.entryInfoList();
    QStringList filenames;
    for(QFileInfo info : files){
        filenames.append(info.absoluteFilePath());
    }
    QString newname = CoreHelper::APPPath() +  "/"+ App::TempDir + "/newname.zip";
    //    if(JlCompress::compressFiles(newname,filenames)){
    //        qDebug()<<"压缩成功";
    //    }else{
    //        qDebug()<<"压缩失败";
    //    }

    // 确保目标目录存在
    //    QDir zipDir(QFileInfo(newname).absolutePath());
    //    if (!zipDir.exists()) {
    //        zipDir.mkpath(".");  // 创建目录
    //    }

    // 创建QuaZip对象，表示目标压缩包
    QuaZip zip(newname);

    // 尝试打开压缩包，如果打开失败返回false
    if (!zip.open(QuaZip::mdCreate)) {
        qDebug() << "无法打开压缩包：" << newname;
    }

    // 遍历文件列表，将每个文件添加到压缩包
    for (const QString &file : filenames) {
        QFile inputFile(file);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开文件：" << file;
            continue;  // 继续下一个文件
        }

        // 获取文件名而不是路径
        QString fileName = QFileInfo(file).fileName();
        qDebug() << "正在压缩文件：" << fileName;

        // 创建一个新的压缩包文件
        QuaZipFile zipFile(&zip);
        //        zipFile.setFileName(fileName);  // 只传入文件名，不包含路径

        // 打开压缩包文件进行写入
        if (!zipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName, file)))
        {
            qDebug() << "Failed to add file to ZIP:" << file;
            continue;
        }

        // 读取文件数据并写入到压缩包
        QByteArray fileData = inputFile.readAll();
        zipFile.write(fileData);

        // 关闭压缩包文件
        zipFile.close();
        inputFile.close();
    }

    // 关闭压缩包
    zip.close();

    qDebug() << "文件已成功压缩到：" << newname;
#endif

#if 0
    //解压文件
    QString newname = CoreHelper::APPPath() + "/" + App::TempDir + "/newname.zip";
    QString newname1 = CoreHelper::APPPath() + "/" + App::TempDir + "/newname";
    //    // 创建 QuaZip 对象，指向目标 zip 文件
    QuaZip zip(newname);

    // 打开 zip 文件
    if (!zip.open(QuaZip::mdUnzip)) {
        qDebug() << "无法打开 zip 文件:" << newname;
        return;
    }

    // 创建目标文件夹（如果不存在）
    QDir dir;
    if (!dir.exists(newname1)) {
        dir.mkpath(newname1);
    }

    // 获取 zip 文件中的文件列表
    QuaZipFile zipFile(&zip);
    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
        // 获取当前文件的文件名
        QString fileName = zip.getCurrentFileName();

        // 设置解压文件的路径
        QString fullPath = newname1 + "/" + fileName;

        // 打开 zip 中的当前文件
        if (!zipFile.open(QIODevice::ReadOnly)) {
            qDebug() << "无法打开 zip 中的文件:" << fileName;
            continue;
        }

        // 创建解压目标文件
        QFile outFile(fullPath);
        if (!outFile.open(QIODevice::WriteOnly)) {
            qDebug() << "无法创建文件:" << fullPath;
            continue;
        }

        // 将 zip 文件内容写入目标文件
        outFile.write(zipFile.readAll());

        // 关闭文件
        outFile.close();
        zipFile.close();
    }

    // 关闭 zip 文件
    zip.close();
    qDebug() << "解压完成！";
#endif
#if 0
    //测试CRC
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select Upgrade File"),
                                                    CoreHelper::APPPath(),
                                                    tr("files(*.bin);;所有文件(*)"));
    if(fileName.isEmpty()) return;

    quint32 Crc = CoreHelper::GetCRC32_STM32H750(fileName);
    quint32 newCRC = CoreHelper::GetrCRC32_MPEG_2(fileName);
    QMessageBox::information(this,"CRC校验(H750)",QString("该文件\nH750 CRC值为:0x%1\n标准CRC为0x%2")
                             .arg(QString::number(Crc,16).toUpper())
                             .arg(QString::number(newCRC,16).toUpper()));
#endif
#if 0
    //临时，读取csv文件打包成bin文件
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select Upgrade File"),
                                                    CoreHelper::APPPath(),
                                                    tr("files(*.csv);;所有文件(*)"));
    if(fileName.isEmpty()) return;
    QFile M_File(fileName);
    if(!M_File.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"打开文件失败";
        return;
    }

    QTextStream in(&M_File);

    QByteArray data1;  // 用来存储第一列数据
    //    QByteArray data2;  // 用来存储第二列数据
    int index = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        index +=1;
        QStringList fields = line.split(',');  // 以逗号为分隔符分割每行数据

        if (fields.size() >= 2) {  // 确保每行至少有两列
            bool ok1;
            double value1 = fields[0].toDouble(&ok1);  // 尝试将第一列转换为 double
            //            double value2 = fields[1].toDouble(&ok2);  // 尝试将第二列转换为 double

            // 如果转换成功，将值添加到 QByteArray 中
            if (ok1) {
                // 将 double 转换为 QByteArray 并追加
                data1.append(reinterpret_cast<const char*>(&value1), sizeof(value1));
                //                data2.append(reinterpret_cast<const char*>(&value2), sizeof(value2));
            }
        }
    }

    M_File.close();
    qDebug()<<"index = " << index;
    qDebug()<<data1.size();
    //    qDebug()<<data2.size();
    QByteArray VibdataArr;
    VibdataArr.append(data1);
    VibdataArr.append(data1);
    qDebug()<<VibdataArr.size();
    uint32_t speedAve = 219;

    QVector<uint32_t> SpeedVector;
    for(int i=0;i<1000;i++){
        uint32_t randomValue = QRandomGenerator::global()->bounded(501);  // 生成范围 [0, 500]
        SpeedVector.append(randomValue);
    }

    QStringList ret;
    ret.append("255|255|255|255|255|2|2|255");
    ret.append("大齿轮2级报警");
    ret.append("小齿轮2级报警");

    QDataStream stream(&VibdataArr, QIODevice::WriteOnly | QIODevice::Append);
    QString titlestr = "speedAve:";
    stream << titlestr << speedAve;
    titlestr = "SpeedVector:";
    stream << titlestr << SpeedVector.size();
    for (uint32_t value : SpeedVector) {
        stream << value;
    }
    titlestr = "result:";
    stream << titlestr;
    for(QString str : ret){
        stream << str;
    }
    qDebug()<<"VibdataArr.size == " << VibdataArr.size();
    //写入bin文件
    QString binname = CoreHelper::APPPath() + "/vib_QN-t0021_1_1_20250205194305.bin";
    M_File.setFileName(binname);
    if(M_File.open(QFile::WriteOnly)){
        QDataStream in(&M_File);
        in.writeRawData(VibdataArr.data(),VibdataArr.size());
        M_File.flush();
        M_File.close();
        QMessageBox::information(this,"csv转bin","导出bin完事儿");
    }
#endif
#if 0
    //创建临时波形图
    //先获取选中的项
    QString select = "ID1CH1(车厢QN-t0021)";
    QStringList selectlist;
    selectlist.append(select);
    QString name = selectlist.join(",") + " 波形图";
    //判断是否存在窗口，如存在则跳转

    frmLivePreview *liveview = new frmLivePreview(selectlist);
    QString dir = App::DownLoadTempDataDir;
    liveview->Test20250206(dir);
    int index = ui->tabWidget->addTab(liveview,name);

    ui->tabWidget->setCurrentIndex(index);
#endif
#if 0
    //设备模拟报警
    DeviceAlarm("QN-t0021", 2, 1, 2, "保外");
#endif
//    ui->tabWidget->setAutoFillBackground(true);
    qDebug() << ui->tabWidget->sizeHint();
    //走行部配置界面
    QString str = "走行部配置";
    int index = findTabByName(str);
    if(index == -1){
         FrmRMConfig *RmConfigView = new FrmRMConfig;
        index = ui->tabWidget->addTab(RmConfigView,str);

    }

    ui->tabWidget->setCurrentIndex(index);

}

void MainWindow::OpenRealtimeView()
{
    //判断窗口是否存在
    QString str = "实况详情";
    int index = findTabByName(str);
    if(index == -1){
        frmRealTimeDebug *realtimedebug = new frmRealTimeDebug;
        index = ui->tabWidget->addTab(realtimedebug,str);
    }
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::RefreshDeviceList(QString wagon,bool Isend)
{
    //添加车厢列表
    ui->combox_carriage->addItem(wagon);
    frmRunningGearConfing::Instance()->AddWagon(wagon);
    //状态列表添加车厢
    QString str = QString("%1|0|%2").arg(wagon).arg(DATETIME);
    PreData::DeviceState.append(str);
    //添加全局变量
    PreData::wagonsList.append(wagon);

    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->listView->model());
    //从设备列表中读取对应车型号下属所有测点
    for(const auto device : PreData::devicelist){
        if(device.Wagon == wagon){
            QStandardItem *item = new QStandardItem();
            QString str = QString("%1(车厢:%2)").arg(device.name).arg(device.Wagon);
            // 设置图标  = QPixmap(":/image/image/other/status_green.png")
            QPixmap pixmap(20,20);
            pixmap.fill(Qt::green);
            if(!device.IsEnable){
                pixmap.fill(Qt::black);
            }
            item->setIcon(pixmap); // 设置图标大小
            item->setText(str);
            item->setSizeHint(QSize(0,30));
            QString tip = QString("id: %1\n"
                                  "ch: %2\n"
                                  "车厢: %3\n"
                                  "使能: %4\n"
                                  "状态: %5\n"
                                  "无报警").arg(device.id).arg(device.ch).arg(wagon).arg(device.IsEnable? "是" : "否")
                    .arg(device.IsEnable? "在线" : "未知");
            item->setToolTip(tip);
            model->appendRow(item);
        }
    }
    if(Isend){
        frmBlockingInterface::Instance()->Stop();
    }
}


void MainWindow::on_btn_query_clicked()
{
    QList<PreData::DEVICEINFO> querylist;
    QString wagon = "",point = "";
    if(ui->check_carriage->isChecked()){
        wagon = ui->combox_carriage->currentText();
    }
    if(ui->check_point->isChecked()){
        point = ui->combox_point->currentText();
    }
    if(wagon.isEmpty() && point.isEmpty()) return;
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->listView->model());
    model->removeRows(0,model->rowCount());
    for(const auto device : PreData::devicelist){
        if((device.Wagon == wagon) || (wagon.isEmpty())){
            if((point.isEmpty()) || (device.type == point)){
                QStandardItem *item = new QStandardItem();
                QString str = QString("%1(车厢:%2)").arg(device.name).arg(device.Wagon);
                // 设置图标
                QPixmap pixmap = QPixmap(":/image/image/other/status_green.png");
                item->setIcon(pixmap.scaled(20, 20)); // 设置图标大小
                item->setText(str);
                item->setSizeHint(QSize(0,30));
                model->appendRow(item);
            }
        }
    }
}

void MainWindow::on_btn_all_clicked()
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->listView->model());
    model->removeRows(0,model->rowCount());
    for(const auto device : PreData::devicelist){
        QStandardItem *item = new QStandardItem();
        QString str = QString("%1(车厢:%2)").arg(device.name).arg(device.Wagon);
        // 设置图标
        QPixmap pixmap = QPixmap(":/image/image/other/status_green.png");
        item->setIcon(pixmap.scaled(20, 20)); // 设置图标大小
        item->setText(str);
        item->setSizeHint(QSize(0,30));
        model->appendRow(item);
    }
}

void MainWindow::OpenRunDB(bool ok)
{
    if(!ok){
        frmBlockingInterface::Instance()->SetFalse("#ff0000","数据库打开失败");
    }else{
        frmBlockingInterface::Instance()->Stop();
    }
}

void MainWindow::connectRm()
{
    bool ok = M_TcpAPI::Instance()->ConnectServer();
    if(ok){
        if(QMessageBox::question(nullptr,"连接","连接走行部成功，是否获取配置文件") == QMessageBox::Yes){
            if(M_NetworkAPI::Instance()->GetRmConfig()){
                frmBlockingInterface::Instance()->SetLabelText("配置获取中,请稍后……");
                frmBlockingInterface::Instance()->StartShow();
            }
        }
    }else{
        QMessageBox::warning(nullptr,"连接设备","连接走行部失败");
    }
}

void MainWindow::Rm_Getconfig()
{
    if(M_NetworkAPI::Instance()->GetRmConfig()){
        frmBlockingInterface::Instance()->SetLabelText("配置获取中,请稍后……");
        frmBlockingInterface::Instance()->StartShow();
    }else{
        QMessageBox::warning(nullptr,"获取配置","走行部未连接");
    }
}

void MainWindow::Rm_GetDBfile()
{
    if(M_NetworkAPI::Instance()->GetRmLog()){
        frmBlockingInterface::Instance()->SetLabelText("配置获取中,请稍后……");
        frmBlockingInterface::Instance()->StartShow();
    }else{
        QMessageBox::warning(nullptr,"获取配置","走行部未连接");
    }
}

void MainWindow::Rm_downloaddata()
{
    if(!M_TcpAPI::Instance()->GetConnectState()){
        QMessageBox::warning(nullptr,"失败","走行部未连接");
        return;
    }
    DataDownload downloadview(this);
    downloadview.exec();
}

void MainWindow::Rm_ClearZipData()
{
    if(!M_TcpAPI::Instance()->GetConnectState()){
        QMessageBox::warning(nullptr,"失败","走行部未连接");
        return;
    }
    if(QMessageBox::question(nullptr,"清空数据","确认清空走行部所有原始数据?") == QMessageBox::Yes){
        M_TcpAPI::Instance()->ClearZIPData();
    }
}

void MainWindow::importConfig()
{
    QStringList list = QFileDialog::getOpenFileNames(this,"选择导入的配置文件",App::DownLoadConfigDir,
                                                     tr("配置文件(*.csv)"));
    if(list.isEmpty()) return;
    frmBlockingInterface::Instance()->SetLabelText("导入中,请稍后……");
    frmBlockingInterface::Instance()->StartShow();
    QStringList wagonlist = ImportOfflineFile::Instance()->importconfig(list);
    if(wagonlist.isEmpty()){
        frmBlockingInterface::Instance()->SetFalse("#ff0000","导入失败");
        return;
    }else{
        frmBlockingInterface::Instance()->Stop();
    }
    for(QString str : wagonlist){
        RefreshDeviceList(str);
    }
}

void MainWindow::importSQL()
{
    QString filename = QFileDialog::getOpenFileName(this,"导入数据库文件",App::DownLoadDBDir,
                                 tr("db文件(*.db)"));
    if(filename.isEmpty()) return;
    if(ImportOfflineFile::Instance()->importSql(filename)){
        QMessageBox::information(nullptr,"导入数据库","导入成功");
    }else{
        QMessageBox::information(nullptr,"导入数据库","导入失败");
    }
}

void MainWindow::importOriginData()
{
    //从本地文件夹内选择一个或多个zip压缩包，或bin文件
    QStringList list = QFileDialog::getOpenFileNames(this,"选择导入的文件",App::DownLoadDataDir,
                                                     tr("压缩文件(*.zip);;数据文件(vib*.bin)"));
    if(list.isEmpty()) return;
    frmBlockingInterface::Instance()->SetLabelText("导入中,请稍后……");
    frmBlockingInterface::Instance()->StartShow();
    ImportOfflineFile::Instance()->importData(list);
}

void MainWindow::ImportOriginDataResult(bool ok)
{
    if(!ok){
        frmBlockingInterface::Instance()->SetFalse("#ff0000","导入失败");
    }else{
        frmBlockingInterface::Instance()->Stop();
    }
}

void MainWindow::on_btn_flushstate_clicked()
{
    for(QString str: PreData::wagonsList){
        UDPMulticastAPI::Instance()->GetPreStateList(str);
    }
}

void MainWindow::SetEchotime()
{
    FrmSetEchoTime setdialog;
    setdialog.exec();
}

void MainWindow::StartScreen()
{
    ScreenshotApp *screenshotApp = new ScreenshotApp(this);
    screenshotApp->startCapture();  // 触发截图
}

void MainWindow::OpenPkView()
{
    //判断对比队列是否存在
    if(PreData::pkdatalist.isEmpty()){
        QMessageBox::warning(this,"对比图","对比数据为空，打开失败");
        return;
    }


    //判断是否存在窗口，如存在则跳转
    QString name = "对比图";
    int index = findTabByName(name);
    if(index == -1){//如果没有，需要创建
        frmDemodulatedChart *pkview = new frmDemodulatedChart;
        index = ui->tabWidget->addTab(pkview,name);
    }
    ui->tabWidget->setCurrentIndex(index);

}
