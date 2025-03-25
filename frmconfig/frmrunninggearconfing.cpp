#include "frmrunninggearconfing.h"
#include "ui_frmrunninggearconfing.h"
#include <QHBoxLayout>
#include "app.h"
#include "preconfig.h"
#include "linkdeviceconfig.h"
#include "timertaskconfig.h"
#include "deviceupgrade.h"
#include "datadownload.h"
#include "m_networkapi.h"
frmRunningGearConfing *frmRunningGearConfing::self =0;
frmRunningGearConfing *frmRunningGearConfing::Instance()
{
    if (!self) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new frmRunningGearConfing;
        }
    }
    return self;
}

frmRunningGearConfing::frmRunningGearConfing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmRunningGearConfing)
{
    ui->setupUi(this);
    //    InitFrom();
    this->setWindowTitle("走行部配置");
    this->setWindowFlag(Qt::WindowMinMaxButtonsHint);
    this->resize(1200,800);
}

frmRunningGearConfing::~frmRunningGearConfing()
{
    delete ui;
}

void frmRunningGearConfing::InitFrom()
{
    //wagons button widget Init
    QLayout *oldLayout = ui->widget_wagons->layout();
    if (oldLayout) {
        delete oldLayout;  // 删除旧的布局
    }
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(6);
    layout->setGeometry(QRect(5,5,5,5));
    ui->widget_wagons->setLayout(layout);

    //tablewidget 不可编辑
    ui->tableWidget_Pre->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget__linkdevice->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_timertask->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //选中方式为整行且只能选择一行
    ui->tableWidget_Pre->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_Pre->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget__linkdevice->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget__linkdevice->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_timertask->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_timertask->setSelectionMode(QAbstractItemView::SingleSelection);

    //禁止编辑及选中
    ui->listWidget_otherboard->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listWidget_otherboard->setSelectionMode(QAbstractItemView::NoSelection);

    //设置默认列数
    ui->tableWidget__linkdevice->setColumnCount(4);
    ui->tableWidget_Pre->setColumnCount(20);
    ui->tableWidget_timertask->setColumnCount(2);

    //LineList Init
    txtAlarmLimit.append(ui->txt_baowaiwarning);
    txtAlarmLimit.append(ui->txt_baoneiwaring);
    txtAlarmLimit.append(ui->txt_waihuanwarning);
    txtAlarmLimit.append(ui->txt_neihuanwarning);
    txtAlarmLimit.append(ui->txt_gundanwarning);
    txtAlarmLimit.append(ui->txt_gunshuangwarning);
    txtAlarmLimit.append(ui->txt_tamianwarning);
    txtAlarmLimit.append(ui->txt_benzhouwarning);
    txtAlarmLimit.append(ui->txt_linzhouwarning);
    txtAlarmLimit.append(ui->txt_baowaifirst);
    txtAlarmLimit.append(ui->txt_baoneifirst);
    txtAlarmLimit.append(ui->txt_waihuanfirst);
    txtAlarmLimit.append(ui->txt_neihuanfirst);
    txtAlarmLimit.append(ui->txt_gundanfirst);
    txtAlarmLimit.append(ui->txt_gunshuangfirst);
    txtAlarmLimit.append(ui->txt_tamianfirst);
    txtAlarmLimit.append(ui->txt_benzhoufirst);
    txtAlarmLimit.append(ui->txt_linzhoufirst);
    txtAlarmLimit.append(ui->txt_baowaisecond);
    txtAlarmLimit.append(ui->txt_baoneisecond);
    txtAlarmLimit.append(ui->txt_waihuansecond);
    txtAlarmLimit.append(ui->txt_neihuansecond);
    txtAlarmLimit.append(ui->txt_gundansecond);
    txtAlarmLimit.append(ui->txt_gunshuangsecond);
    txtAlarmLimit.append(ui->txt_tamiansecond);
    txtAlarmLimit.append(ui->txt_benzhousecond);
    txtAlarmLimit.append(ui->txt_linzhousecond);
    txtAlarmLimit.append(ui->txt_absolutetem);
    txtAlarmLimit.append(ui->txt_relativetem);
    txtAlarmLimit.append(ui->txt_temwarning);

    txtCarInfo.append(ui->txt_projectname);
    txtCarInfo.append(ui->txt_linenum);
    txtCarInfo.append(ui->txt_carnum);
    txtCarInfo.append(ui->txt_wagonnum);
    txtCarInfo.append(ui->txt_wheeldiameter);
    txtCarInfo.append(ui->txt_speedCoefficient);

    //    txtSaveInterval.append(ui->txt_IsEnable);
    txtSaveInterval.append(ui->txt_lowpowerTaskSpeed);
    txtSaveInterval.append(ui->txt_temTaskSpeed);
    txtSaveInterval.append(ui->txt_VIBInterval);
    txtSaveInterval.append(ui->txt_temInterval);

    txtBearingInfo.append(ui->txt_model);
    txtBearingInfo.append(ui->txt_pitchDiameter);
    txtBearingInfo.append(ui->txt_rollingDiameter);
    txtBearingInfo.append(ui->txt_rollerNum);
    txtBearingInfo.append(ui->txt_contactAngle);

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

    for(auto txt : txtAlarmLimit){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    for(auto txt : txtCarInfo){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    for(auto txt : txtSaveInterval){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    for(auto txt : txtBearingInfo){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    for(auto txt : txtNetInfo){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    //pushbutton List Init
    btnList.append(ui->widget_right->findChildren<QPushButton *>());
    btnList.append(ui->widget_left->findChildren<QPushButton *>());
    btnList.append(ui->gbox_Pre->findChildren<QPushButton *>());
    btnList.append(ui->btn_reboot);
    btnList.append(ui->btn_DeviceUpgrade);

    QStringList list;
    list << "Host" << "StandbyHost" << "Slave";
    ui->cbox_wagontype->addItems(list);

    //set default state
    SetbtnState(false);


    //bind signal
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::linkconfigrereturn,this,&frmRunningGearConfing::UpdateLinkView);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::OtherBoardState,this,&frmRunningGearConfing::UpdateOtherBoardView);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::taskReturn,this,&frmRunningGearConfing::UpdatetaskView);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::carinforeturn,this,&frmRunningGearConfing::UpdatecarView);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::DatasaveIntervalreturn,this,&frmRunningGearConfing::UpdatesaveIntervalView);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::BearingReturn,this,&frmRunningGearConfing::UpdateBearingView);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::networkconfigreturn,this,&frmRunningGearConfing::UpdatenetworkView);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::Alarmlimit,this,&frmRunningGearConfing::UpdateAlarmlimitView);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::Versionreturn,this,&frmRunningGearConfing::UpdateAppVersion);



    //Test
    //    AddWagon("QN-t0021");
    //    AddWagon("QN-t0998");
    //    AddWagon("QN-Test0003");
    //    AddWagon("QN-Test0004");
}

void frmRunningGearConfing::AddWagon(QString wagon)
{
    QToolButton *toolButton = new QToolButton(ui->widget_wagons);
    toolButton->setText(wagon);
    toolButton->setCheckable(true);
    toolButton->setAutoExclusive(true);
    QLayout *oldlayout = ui->widget_wagons->layout();
    oldlayout->addWidget(toolButton);
    connect(toolButton,&QToolButton::clicked,this,&frmRunningGearConfing::WagonSwitch);
}

void frmRunningGearConfing::clearFrom()
{
    ui->tableWidget_Pre->clear();
    ui->tableWidget__linkdevice->clear();
    ui->tableWidget_timertask->clear();
    ui->listWidget_otherboard->clear();
    ui->tableWidget_Pre->setRowCount(0);
    ui->tableWidget_Pre->setColumnCount(0);
    ui->tableWidget__linkdevice->setRowCount(0);
    ui->tableWidget__linkdevice->setColumnCount(0);
    ui->tableWidget_timertask->setRowCount(0);
    ui->tableWidget_timertask->setColumnCount(0);
    ui->cbox_bearing->clear();
    ui->checkbox_Isenable->setChecked(false);

    //清空界面lineedit
    for(auto txt : txtAlarmLimit){
        txt->clear();
    }

    for(auto txt : txtCarInfo){
        txt->clear();
    }

    for(auto txt : txtSaveInterval){
        txt->clear();
    }

    for(auto txt : txtBearingInfo){
        txt->clear();
    }

    for(auto txt : txtNetInfo){
        txt->clear();
    }
    ui->txt_version->clear();
}

void frmRunningGearConfing::SetbtnState(bool enable)
{
    for(auto btn : btnList){
        btn->setEnabled(enable);
    }
    if(enable){
        ui->btn_addtask->setEnabled(false);
        ui->btn_modifytask->setEnabled(false);
        ui->btn_deletetask->setEnabled(false);
        ui->btn_addlinkdevice->setEnabled(false);
        ui->btn_modifylinkdevice->setEnabled(false);
        ui->btn_deletelinkdevice->setEnabled(false);
        //        ui->btn_addbearing->setEnabled(false);
        //        ui->btn_modifybearing->setEnabled(false);
        //        ui->btn_deletebearing->setEnabled(false);
        ui->btn_setcarinfo->setEnabled(false);
        ui->btn_setInterval->setEnabled(false);
        ui->btn_setnetinfo->setEnabled(false);
        ui->btn_setalarmlimit->setEnabled(false);
    }
}

void frmRunningGearConfing::WagonSwitch()
{
    //更新选中的车厢号码
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    QString clickwagon = button->text();
    if(clickwagon != currentwagon){
        currentwagon = clickwagon;
    }

    //加载对应车厢前置处理器信息
#if 0
    //1、找出对应的csv文件
    QString filename;
    QDir dir(App::DownLoadConfigDir);
    if (!dir.exists()) {
        qDebug() << "Directory does not exist!";
        return;
    }

    // 只查找文件
    dir.setFilter(QDir::Files);

    QFileInfoList files = dir.entryInfoList();
    foreach (const QFileInfo &fileInfo, files) {
        if (fileInfo.fileName().contains(QString("%1.csv").arg(currentwagon), Qt::CaseInsensitive)) {
            filename = fileInfo.absoluteFilePath();
        }
    }

    //2、读出文件内容
    if(filename.isEmpty()) return;
    QFile file(filename);
    QStringList datalist;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString str = in.readLine();
        if(!str.isEmpty()){
            datalist.append(str);
        }
    }
    file.close();

    currentfile = filename;
    int rows = datalist.size();
    int columns = datalist.first().split(",").size();

    clearFrom();
    //初始化tablewidget
    ui->tableWidget_Pre->setColumnCount(columns);
    ui->tableWidget_Pre->setRowCount(rows);

    QStringList labels;
    labels << "id" << "ch" << "名称" << "类型" << "灵敏度" << "轴径" << "轴承1名称"
           << "轴承1型号" << "轴承2名称" << "轴承2型号" << "轴承3名称" << "轴承3型号"
           << "轴承4名称" << "轴承4型号" << "主动轮名称" << "主动轮齿数" << "从动轮名称"
           << "从动轮齿数" << "版本" << "是否使能";
    ui->tableWidget_Pre->setHorizontalHeaderLabels(labels);
    //将内容导入pre视图
    int row = 0;

    for(const QString str : datalist){
        QStringList prelist = str.split(",");
        if(prelist.size()!=columns) continue;
        for(int i=0;i<columns;i++){
            QTableWidgetItem *item = new QTableWidgetItem(prelist.at(i));
            ui->tableWidget_Pre->setItem(row,i,item);
            if(i == 7 || i == 9 || i == 11 || i == 13){
                BearingList.append(prelist.at(i));
            }
        }
        row += 1;
    }

    //更新轴承选项卡
    //剔除相同元素
    QStringList BearingList;
    BearingList.removeDuplicates();
    ui->cbox_bearing->addItems(BearingList);
#else
    //获取前置处理器索引
    QList<int> preindexlist;
    for(int i=0;i<PreData::devicelist.size();i++){
        if(currentwagon == PreData::devicelist.at(i).Wagon){
            preindexlist.append(i);
        }
    }

    int rows = preindexlist.size();
    int columns = 20;//固定写死的，如果要该数据库这里也要改
    clearFrom();
    //初始化tablewidget
    ui->tableWidget_Pre->setColumnCount(columns);
    ui->tableWidget_Pre->setRowCount(rows);

    QStringList labels;
    labels << "id" << "ch" << "名称" << "类型" << "灵敏度" << "轴径" << "轴承1名称"
           << "轴承1型号" << "轴承2名称" << "轴承2型号" << "轴承3名称" << "轴承3型号"
           << "轴承4名称" << "轴承4型号" << "从动轮名称" << "从动轮齿数" << "主动轮名称"
           << "主动轮齿数" << "版本" << "是否使能";
    ui->tableWidget_Pre->setHorizontalHeaderLabels(labels);


    //将前置内容导入pre视图
    for(int i=0;i<preindexlist.size();i++){
        auto device = PreData::devicelist.at(preindexlist.at(i));
        ui->tableWidget_Pre->setItem(i, 0, new QTableWidgetItem(QString::number(device.id)));
        ui->tableWidget_Pre->setItem(i, 1, new QTableWidgetItem(QString::number(device.ch)));
        ui->tableWidget_Pre->setItem(i, 2, new QTableWidgetItem(device.name));
        ui->tableWidget_Pre->setItem(i, 3, new QTableWidgetItem(device.type));
        ui->tableWidget_Pre->setItem(i, 4, new QTableWidgetItem(QString::number(device.sensitivity)));
        ui->tableWidget_Pre->setItem(i, 5, new QTableWidgetItem(QString::number(device.shaftDiameter)));
        ui->tableWidget_Pre->setItem(i, 6, new QTableWidgetItem(device.bearing1_name));
        ui->tableWidget_Pre->setItem(i, 7, new QTableWidgetItem(device.bearing1_model));
        ui->tableWidget_Pre->setItem(i, 8, new QTableWidgetItem(device.bearing2_name));
        ui->tableWidget_Pre->setItem(i, 9, new QTableWidgetItem(device.bearing2_model));
        ui->tableWidget_Pre->setItem(i, 10, new QTableWidgetItem(device.bearing3_name));
        ui->tableWidget_Pre->setItem(i, 11, new QTableWidgetItem(device.bearing3_model));
        ui->tableWidget_Pre->setItem(i, 12, new QTableWidgetItem(device.bearing4_name));
        ui->tableWidget_Pre->setItem(i, 13, new QTableWidgetItem(device.bearing4_model));
        ui->tableWidget_Pre->setItem(i, 14, new QTableWidgetItem(device.capstanname));
        ui->tableWidget_Pre->setItem(i, 15, new QTableWidgetItem(QString::number(device.capstanTeethNum)));
        ui->tableWidget_Pre->setItem(i, 16, new QTableWidgetItem(device.drivenwheelName));
        ui->tableWidget_Pre->setItem(i, 17, new QTableWidgetItem(QString::number(device.drivenwheelTeethNum)));
        ui->tableWidget_Pre->setItem(i, 18, new QTableWidgetItem(device.version));
        ui->tableWidget_Pre->setItem(i, 19, new QTableWidgetItem(device.IsEnable ? "Enabled" : "Disabled"));
    }

    //设置Pre视图的宽度
    QList<int> columnWidths;
    columnWidths << 50 << 50 << 80 << 80 << 50 << 50 << 120 << 120
                 << 120 << 120 << 120 << 120 << 120 << 120 << 120 << 80
                 << 120 << 80 << 80 << 50;
    for(int i=0;i<columnWidths.size();i++){
        ui->tableWidget_Pre->setColumnWidth(i, columnWidths.at(i));
    }
    //    ui->tableWidget_Pre->resizeColumnsToContents();
    ui->tableWidget_Pre->horizontalHeader()->setStretchLastSection(true);

    //轴承内容导入bearing视图
    QStringList BearingList = GetBearingModelList(currentwagon);
    ui->cbox_bearing->addItems(BearingList);

    if(!BearingList.isEmpty()){
        //加载默认轴承信息
        ui->cbox_bearing->setCurrentIndex(0);
        ui->txt_model->setText(PreData::bearinglist.at(0).Model);
        ui->txt_pitchDiameter->setText(QString::number(PreData::bearinglist.at(0).PitchDiameter));
        ui->txt_rollingDiameter->setText(QString::number(PreData::bearinglist.at(0).RollingDiameter));
        ui->txt_rollerNum->setText(QString::number(PreData::bearinglist.at(0).RollerNum));
        ui->txt_contactAngle->setText(QString::number(PreData::bearinglist.at(0).ContactAngle));
    }


#endif

    SetbtnState(true);
}

void frmRunningGearConfing::UpdateLinkView(QString wagon, QStringList linklist)
{
    if(wagon != currentwagon) return;
    ui->tableWidget__linkdevice->clear();
    //格式：类型|ip|端口|车厢

    int rows = linklist.size();
    int columns = 4;

    ui->tableWidget__linkdevice->setColumnCount(columns);
    ui->tableWidget__linkdevice->setRowCount(rows);

    QStringList labels;
    labels << "类型" << "IP地址" << "端口" << "车厢号";
    ui->tableWidget__linkdevice->setHorizontalHeaderLabels(labels);

    //导入视图
    int row = 0;
    for(const QString str :linklist){
        QStringList temp = str.split("|");
        for(int i=0;i<columns;i++){
            QTableWidgetItem *item = new QTableWidgetItem(temp.at(i));
            ui->tableWidget__linkdevice->setItem(row,i,item);
        }
        row += 1;
    }
    ui->tableWidget__linkdevice->horizontalHeader()->setStretchLastSection(true);//最后一列自适应宽度
    ui->btn_addlinkdevice->setEnabled(true);
    ui->btn_modifylinkdevice->setEnabled(true);
    ui->btn_deletelinkdevice->setEnabled(true);
}

void frmRunningGearConfing::UpdateOtherBoardView(QString wagon, QStringList statelist)
{
    if(wagon != currentwagon) return;
    ui->listWidget_otherboard->clear();
    //转速板卡状态|版本|自检状态;通信板卡状态|版本自检状态;前置IO板卡状态|版本自检状态
    QStringList boardname;
    boardname << "转速" << "通信" << "前置IO";
    for(int i=0;i<statelist.size();i++){
        QStringList list = statelist.at(i).split("|");
        int state = list.at(0).toInt();
        QString itemtext = QString("%1板卡: %2, 软件版本: %3,自检状态: %4").arg(boardname.at(i))
                .arg(state > 0 ? "离线" : "在线").arg(list.at(1)).arg(list.at(2));
        ui->listWidget_otherboard->addItem(itemtext);
    }
}

void frmRunningGearConfing::UpdatetaskView(QString wagon, QStringList tasklist)
{
    if(wagon != currentwagon) return;
    ui->tableWidget_timertask->clear();
    //格式：任务名称|执行时间
    int rows = tasklist.size();
    int columns = 2;

    ui->tableWidget_timertask->setColumnCount(columns);
    ui->tableWidget_timertask->setRowCount(rows);

    QStringList labels;
    labels << "任务名称" << "执行时间";
    ui->tableWidget_timertask->setHorizontalHeaderLabels(labels);

    //导入视图
    int row = 0;
    for(const QString str :tasklist){
        QStringList temp = str.split("|");
        for(int i=0;i<columns;i++){
            QTableWidgetItem *item = new QTableWidgetItem(temp.at(i));
            ui->tableWidget_timertask->setItem(row,i,item);
        }
        row += 1;
    }
    ui->tableWidget_timertask->horizontalHeader()->setStretchLastSection(true);//最后一列自适应宽度
    ui->btn_addtask->setEnabled(true);
    ui->btn_modifytask->setEnabled(true);
    ui->btn_deletetask->setEnabled(true);
}

void frmRunningGearConfing::UpdatecarView(QString project, int linenum, QString carnum, QString wagon, float WheelDiameter, QString type, double sppedcoefficient)
{
    if(wagon != currentwagon) return;
    for(auto txt : txtCarInfo){
        txt->clear();
    }
    ui->txt_projectname->setText(project);
    ui->txt_linenum->setText(QString::number(linenum));
    ui->txt_carnum->setText(carnum);
    ui->txt_wagonnum->setText(wagon);
    ui->txt_wheeldiameter->setText(QString::number(WheelDiameter));
    ui->btn_setcarinfo->setEnabled(true);
    ui->txt_speedCoefficient->setText(QString::number(sppedcoefficient));
    int index = ui->cbox_wagontype->findText(type, Qt::MatchExactly);
    if(index != -1){
        ui->cbox_wagontype->setCurrentIndex(index);
    }
}

void frmRunningGearConfing::UpdatesaveIntervalView(QString wagon, int lowpowerspeedmax, int temspeedmax, bool IsEnable, int VibInterval, int TemInterval)
{
    if(wagon != currentwagon) return;
    ui->txt_lowpowerTaskSpeed->setText(QString::number(lowpowerspeedmax));
    ui->txt_temTaskSpeed->setText(QString::number(temspeedmax));
    ui->checkbox_Isenable->setChecked(IsEnable);
    ui->txt_VIBInterval->setText(QString::number(VibInterval));
    ui->txt_temInterval->setText(QString::number(TemInterval));
    ui->btn_setInterval->setEnabled(true);
}

void frmRunningGearConfing::UpdateBearingView(QString wagon, QStringList bearingInfo)
{
    if(wagon != currentwagon) return;
    //轴承型号;中径;滚径;滚子数;接触角
    if(bearingInfo.at(0) != ui->cbox_bearing->currentText()) return;
    for(int i=0;i<bearingInfo.size();i++){
        txtBearingInfo.at(i)->setText(bearingInfo.at(i));
    }

    //    ui->btn_addbearing->setEnabled(true);
    //    ui->btn_modifybearing->setEnabled(true);
    //    ui->btn_deletebearing->setEnabled(true);
}

void frmRunningGearConfing::UpdatenetworkView(QString wagon, QStringList configlist)
{
    if(wagon != currentwagon) return;
    for(int i=0;i<configlist.size();i++){
        txtNetInfo.at(i)->setText(configlist.at(i));
    }
    ui->btn_setnetinfo->setEnabled(true);
}

void frmRunningGearConfing::UpdateAlarmlimitView(QString wagon, QStringList values)
{
    if(wagon != currentwagon) return;
    for(int i=0;i<values.size();i++){
        txtAlarmLimit.at(i)->setText(values.at(i));
    }
    ui->btn_setalarmlimit->setEnabled(true);
}

void frmRunningGearConfing::UpdateAppVersion(QString wagon, QString version)
{
    if(wagon != currentwagon) return;
    ui->txt_version->setText(version);
}

QStringList frmRunningGearConfing::GetBearingModelList(QString wagon)
{
    QStringList BearingList;
    for(int i=0;i<PreData::bearinglist.size();i++){
        if(wagon == PreData::bearinglist.at(i).Wagon){
            BearingList.append(PreData::bearinglist.at(i).Model);
        }
    }
    return BearingList;
}

void frmRunningGearConfing::on_btn_preadd_clicked()
{
    PreConfig preconfig(this,"新增");
    preconfig.SetBearingDefault(GetBearingModelList(currentwagon));
    int result = preconfig.exec();

    if(result ==  PreConfig::Accepted){
        QStringList inputValues = preconfig.GetPreInfo();
        //将内容添加到显示框中
        int rows = ui->tableWidget_Pre->rowCount();
        ui->tableWidget_Pre->insertRow(rows);
        for (int i = 0; i < inputValues.size(); ++i) {
            ui->tableWidget_Pre->setItem(rows, i, new QTableWidgetItem(inputValues.at(i)));
        }
        //将内容添加至文件中
        if(!currentfile.isEmpty()){
            QFile file(currentfile);
            if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
                QTextStream out(&file);
                for(int i=0;i<inputValues.size();i++){
                    out << inputValues.at(i);
                    if(i == inputValues.size()-1){
                        out << Qt::endl;
                    }else{
                        out << ",";
                    }
                }
                file.close();
            }
        }
        //发送指令给走行部设备
        UDPMulticastAPI::Instance()->UpdatePre(currentwagon,0,inputValues);
        QMessageBox::information(this,"提示","已添加");
    }
}

void frmRunningGearConfing::on_btn_premodify_clicked()
{
    //读取当前选择的项
    int currentrow = ui->tableWidget_Pre->currentRow();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","当前未选中任何行");
        return;
    }

    //读取出当前选择行的内容
    QStringList currentvalues;
    for(int i=0;i<ui->tableWidget_Pre->columnCount();i++){
        currentvalues.append(ui->tableWidget_Pre->item(currentrow,i)->text());
    }


    PreConfig preconfig(this,"修改");
    preconfig.SetBearingDefault(GetBearingModelList(currentwagon));
    preconfig.SetPreDefault(currentvalues);

    int result = preconfig.exec();

    if(result ==  PreConfig::Accepted){
        QStringList inputValues = preconfig.GetPreInfo();
        //更新视图
        for(int i=0;i<inputValues.size();i++){
            if(i == inputValues.size()-1){
                if(inputValues.at(i).toInt()){
                    ui->tableWidget_Pre->item(currentrow, i)->setText("Enable");
                }else{
                    ui->tableWidget_Pre->item(currentrow, i)->setText("Disable");
                }
                continue;
            }
            ui->tableWidget_Pre->item(currentrow, i)->setText(inputValues.at(i));
        }

        //更改文件内容
        if(!currentfile.isEmpty()){
            QFile file(currentfile);
            QStringList lines;
            // 读取文件内容到列表
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    lines.append(in.readLine());
                }
                file.close();
            }
            lines[currentrow] = inputValues.join(",");
            // 写回文件
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                for (const QString& line : lines) {
                    out << line << "\n";
                }
                file.close();
            }
        }
        //发送指令给走行部设备
        UDPMulticastAPI::Instance()->UpdatePre(currentwagon,1,inputValues);
        QMessageBox::information(this,"提示","修改成功");
    }
}

void frmRunningGearConfing::on_btn_predelete_clicked()
{
    //读取当前选择的项
    int currentrow = ui->tableWidget_Pre->currentRow();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","当前未选中任何行");
        return;
    }

    //读取出当前选择行的内容
    QStringList list;
    for (int i = 0; i < ui->tableWidget_Pre->columnCount(); ++i) {
        auto item = ui->tableWidget_Pre->item(currentrow, i);
        list << (item ? item->text() : QString());
    }

    //更新列表内容
    ui->tableWidget_Pre->removeRow(currentrow);

    //更改文件内容
    if(!currentfile.isEmpty()){
        QFile file(currentfile);
        QStringList lines;
        // 读取文件内容到列表
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                lines.append(in.readLine());
            }
            file.close();
        }
        lines.removeAt(currentrow);
        // 写回文件
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const QString& line : lines) {
                out << line << "\n";
            }
            file.close();
        }
    }
    //发送指令给走行部设备
    UDPMulticastAPI::Instance()->UpdatePre(currentwagon,2,list);
    QMessageBox::information(this,"提示","删除成功");
}

void frmRunningGearConfing::on_btn_SetSametype_clicked()
{
    int currentrow = ui->tableWidget_Pre->currentRow();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","当前未选中任何行");
        return;
    }

    //读取选中行的类型
    QString type = ui->tableWidget_Pre->item(currentrow,3)->text();
    if(QMessageBox::question(this,"二次确认",QString("确认将选中行参数更新至所有 %1 测点？").arg(type)) == QMessageBox::Yes){
        // 创建提示框
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("更新中，请稍后...");
        msgBox.setStandardButtons(QMessageBox::NoButton); // 不显示按钮
        msgBox.show();

        //打开本地配置文件
        QStringList lines;
        QFile file(currentfile);
        if(!currentfile.isEmpty()){
            // 读取文件内容到列表
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    lines.append(in.readLine());
                }
                file.close();
            }
        }
        //读取当前测点内容，除了id和ch，名称，类型，版本、是否使能
        QStringList baseconfig;
        for(int i=4;i<ui->tableWidget_Pre->columnCount()-2;i++){
            baseconfig.append(ui->tableWidget_Pre->item(currentrow,i)->text());
        }
        //读取所有相同测点的行号
        QList<int> SameTypeRows;
        for(int row=0;row<ui->tableWidget_Pre->rowCount();row++){
            if(ui->tableWidget_Pre->item(row,3)->text() == type){
                if(row == currentrow) continue;
                SameTypeRows.append(row);
                //更新视图内容
                for(int column=0;column<baseconfig.size();column++){
                    ui->tableWidget_Pre->item(row,column+4)->setText(baseconfig.at(column));
                }
                //获取该行的新内容
                QStringList newconfig;
                for(int i=0;i<ui->tableWidget_Pre->columnCount();i++){
                    newconfig.append(ui->tableWidget_Pre->item(row,i)->text());
                }
                //更新文件内容
                lines[row] = newconfig.join(",");
                //更新前置处理器信息
                UDPMulticastAPI::Instance()->UpdatePre(currentwagon,1,newconfig);
                QThread::usleep(5);
            }
        }
        // 写回文件
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const QString& line : lines) {
                out << line << "\n";
            }
            file.close();
        }
        msgBox.close();
    }
}

void frmRunningGearConfing::on_btn_setAll_clicked()
{
    int currentrow = ui->tableWidget_Pre->currentRow();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","当前未选中任何行");
        return;
    }

    //读取选中行的类型
    QString type = ui->tableWidget_Pre->item(currentrow,3)->text();
    if(QMessageBox::question(this,"二次确认",QString("确认将选中行参数更新至所有测点？")) == QMessageBox::Yes){
        // 创建提示框
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("更新中，请稍后...");
        msgBox.setStandardButtons(QMessageBox::NoButton); // 不显示按钮
        msgBox.show();

        //打开本地配置文件
        QStringList lines;
        QFile file(currentfile);
        if(!currentfile.isEmpty()){
            // 读取文件内容到列表
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    lines.append(in.readLine());
                }
                file.close();
            }
        }
        //读取当前测点内容，除了id和ch，名称，类型，版本、是否使能
        QStringList baseconfig;
        for(int i=4;i<ui->tableWidget_Pre->columnCount()-2;i++){
            baseconfig.append(ui->tableWidget_Pre->item(currentrow,i)->text());
        }
        //读取所有相同测点的行号
        //        QList<int> SameTypeRows;
        for(int row=0;row<ui->tableWidget_Pre->rowCount();row++){
            if(row == currentrow) continue;
            //            SameTypeRows.append(row);
            //更新视图内容
            for(int column=0;column<baseconfig.size();column++){
                ui->tableWidget_Pre->item(row,column+4)->setText(baseconfig.at(column));
            }
            //获取该行的新内容
            QStringList newconfig;
            for(int i=0;i<ui->tableWidget_Pre->columnCount();i++){
                newconfig.append(ui->tableWidget_Pre->item(row,i)->text());
            }
            //更新文件内容
            lines[row] = newconfig.join(",");
            //更新前置处理器信息
            UDPMulticastAPI::Instance()->UpdatePre(currentwagon,1,newconfig);
            QThread::usleep(5);
        }
        // 写回文件
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const QString& line : lines) {
                out << line << "\n";
            }
            file.close();
        }
        msgBox.close();
    }
}

void frmRunningGearConfing::on_btn_getlinkinfo_clicked()
{
    if(currentwagon.isEmpty()) return;
    UDPMulticastAPI::Instance()->GetLinkDeviceInfo(currentwagon);
}

void frmRunningGearConfing::on_btn_addlinkdevice_clicked()
{
    if(currentwagon.isEmpty()) return;
    LinkDeviceConfig linkconfig(this,"新增");
    int result = linkconfig.exec();
    if(result == LinkDeviceConfig::Accepted){
        //添加视图
        QStringList inputValues = linkconfig.GetLinkInfo();
        int rows = ui->tableWidget__linkdevice->rowCount();
        if(rows == 0){
            //添加列名
            ui->tableWidget__linkdevice->setColumnCount(4);
            QStringList labels;
            labels << "类型" << "IP地址" << "端口" << "车厢号";
            ui->tableWidget__linkdevice->setHorizontalHeaderLabels(labels);
            ui->tableWidget__linkdevice->horizontalHeader()->setStretchLastSection(true);//最后一列自适应宽度
        }
        ui->tableWidget__linkdevice->insertRow(rows);
        for (int i = 0; i < inputValues.size(); ++i) {
            ui->tableWidget__linkdevice->setItem(rows, i, new QTableWidgetItem(inputValues.at(i)));
        }

        //发送指令给走行部设备
        UDPMulticastAPI::Instance()->UpdateLinkDevice(currentwagon,0,inputValues.at(0),inputValues.at(1),inputValues.at(2).toInt(),inputValues.at(3));
        QMessageBox::information(this,"提示","已添加");
    }
}

void frmRunningGearConfing::on_btn_modifylinkdevice_clicked()
{
    //读取当前选择的项
    int currentrow = ui->tableWidget__linkdevice->currentRow();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","当前未选中任何行");
        return;
    }
    //读取选中项的内容
    QStringList values;
    for(int i=0;i<ui->tableWidget__linkdevice->columnCount();i++){
        values.append(ui->tableWidget__linkdevice->item(currentrow,i)->text());
    }

    LinkDeviceConfig linkconfig(this,"修改");
    linkconfig.SetDefaultInfo(values);
    int result = linkconfig.exec();

    if(result == LinkDeviceConfig::Accepted){
        //添加视图
        QStringList inputValues = linkconfig.GetLinkInfo();
        for(int i=0;i<inputValues.size();i++){
            ui->tableWidget__linkdevice->item(currentrow, i)->setText(inputValues.at(i));
        }

        //发送指令给走行部设备
        UDPMulticastAPI::Instance()->UpdateLinkDevice(currentwagon,1,inputValues.at(0),inputValues.at(1),inputValues.at(2).toInt(),inputValues.at(3));
        QMessageBox::information(this,"提示","修改成功");
    }
}

void frmRunningGearConfing::on_btn_deletelinkdevice_clicked()
{
    //读取当前选择的项
    int currentrow = ui->tableWidget__linkdevice->currentRow();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","当前未选中任何行");
        return;
    }

    //读取出当前选择行的内容
    QStringList list;
    for (int i = 0; i < ui->tableWidget__linkdevice->columnCount(); ++i) {
        auto item = ui->tableWidget__linkdevice->item(currentrow, i);
        list << (item ? item->text() : QString());
    }

    //更新视图
    ui->tableWidget__linkdevice->removeRow(currentrow);

    //发送指令
    UDPMulticastAPI::Instance()->UpdateLinkDevice(currentwagon,2,list.at(0),list.at(1),list.at(2).toInt(),list.at(3));
    QMessageBox::information(this,"提示","已删除");
}

void frmRunningGearConfing::on_btn_getotherboard_clicked()
{
    if(currentwagon.isEmpty()) return;
    UDPMulticastAPI::Instance()->GetOtherBoardState(currentwagon);
}

void frmRunningGearConfing::on_btn_gettask_clicked()
{
    if(currentwagon.isEmpty()) return;
    UDPMulticastAPI::Instance()->GetTimerTask(currentwagon);
}

void frmRunningGearConfing::on_btn_addtask_clicked()
{
    if(ui->tableWidget_timertask->rowCount() == 2){
        QMessageBox::warning(this,"提示","任务已满,不可添加");
        return;
    }
    if(currentwagon.isEmpty()) return;
    TimerTaskConfig taskconfig(this,"新增");
    int result = taskconfig.exec();

    if(result == TimerTaskConfig::Accepted){
        QStringList list = taskconfig.GetTaskInfo();
        //更新视图
        int rows = ui->tableWidget_timertask->rowCount();
        if(rows == 0){
            //添加列名
            ui->tableWidget_timertask->setColumnCount(2);

            QStringList labels;
            labels << "任务名称" << "执行时间";
            ui->tableWidget_timertask->setHorizontalHeaderLabels(labels);
            ui->tableWidget_timertask->horizontalHeader()->setStretchLastSection(true);//最后一列自适应宽度
        }
        ui->tableWidget_timertask->insertRow(rows);
        for(int i=0;i<list.size();i++){
            ui->tableWidget_timertask->setItem(rows,i,new QTableWidgetItem(list.at(i)));
        }

        //发送指令
        UDPMulticastAPI::Instance()->UpdateTimerTask(currentwagon,0,list.at(0),list.at(1));
        QMessageBox::information(this,"提示","已添加");
    }
}

void frmRunningGearConfing::on_btn_modifytask_clicked()
{
    //读取当前选择的项
    int currentrow = ui->tableWidget_timertask->currentRow();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","当前未选中任何行");
        return;
    }
    //读取选中项的任务
    QString currentAction = ui->tableWidget_timertask->item(currentrow,0)->text();

    TimerTaskConfig taskconfig(this,"修改");
    taskconfig.SetDefaultInfo(currentAction);
    int result = taskconfig.exec();

    if(result == LinkDeviceConfig::Accepted){
        //添加视图
        QStringList inputValues = taskconfig.GetTaskInfo();
        for(int i=0;i<inputValues.size();i++){
            ui->tableWidget_timertask->item(currentrow, i)->setText(inputValues.at(i));
        }

        //发送指令给走行部设备
        UDPMulticastAPI::Instance()->UpdateTimerTask(currentwagon,1,inputValues.at(0),inputValues.at(1));
        QMessageBox::information(this,"提示","修改成功");
    }
}

void frmRunningGearConfing::on_btn_deletetask_clicked()
{
    //读取当前选择的项
    int currentrow = ui->tableWidget_timertask->currentRow();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","当前未选中任何行");
        return;
    }

    //读取出当前选择行的内容
    QStringList list;
    for (int i = 0; i < ui->tableWidget_timertask->columnCount(); ++i) {
        auto item = ui->tableWidget_timertask->item(currentrow, i);
        list << (item ? item->text() : QString());
    }

    //更新视图
    ui->tableWidget_timertask->removeRow(currentrow);

    //发送指令
    UDPMulticastAPI::Instance()->UpdateTimerTask(currentwagon,2,list.at(0),list.at(1));
    QMessageBox::information(this,"提示","已删除");
}

void frmRunningGearConfing::on_btn_getcarinfo_clicked()
{
    if(currentwagon.isEmpty()) return;
    UDPMulticastAPI::Instance()->GetCarConfig(currentwagon);
}

void frmRunningGearConfing::on_btn_setcarinfo_clicked()
{
    if (ui->txt_linenum->text().isEmpty() || ui->txt_carnum->text().isEmpty() ||
            ui->txt_wagonnum->text().isEmpty() || ui->txt_wheeldiameter->text().isEmpty() ||
            ui->txt_speedCoefficient->text().isEmpty() || ui->txt_projectname->text().isEmpty()){
        QMessageBox::warning(this,"提示","未填写完整");
        return;
    }
    QString project = ui->txt_projectname->text();
    QString line = ui->txt_linenum->text();
    QString carname = ui->txt_carnum->text();
    QString wagonname = ui->txt_wagonnum->text();
    float wheeldiameter = ui->txt_wheeldiameter->text().toFloat();
    QString type = ui->cbox_wagontype->currentText();
    double Coefficient = ui->txt_speedCoefficient->text().toDouble();
    UDPMulticastAPI::Instance()->UpdateCarInfo(project,wagonname,line,carname,wheeldiameter,type,Coefficient);
    QMessageBox::information(this,"提示","设置成功");
}

void frmRunningGearConfing::on_btn_getInterval_clicked()
{
    if(currentwagon.isEmpty()) return;
    UDPMulticastAPI::Instance()->GetSaveInterval(currentwagon);
}

void frmRunningGearConfing::on_btn_setInterval_clicked()
{
    if (ui->txt_VIBInterval->text().isEmpty() || ui->txt_temInterval->text().isEmpty()){
        QMessageBox::warning(this,"提示","未填写完整");
        return;
    }
    int lowpowerspeedmax = ui->txt_lowpowerTaskSpeed->text().toInt();
    int temspeedmax = ui->txt_temTaskSpeed->text().toInt();
    bool Isenable = ui->checkbox_Isenable->isChecked();
    int vibInterval = ui->txt_VIBInterval->text().toInt();
    int temInterval = ui->txt_temInterval->text().toInt();
    UDPMulticastAPI::Instance()->UpdateSaveInterval(currentwagon,lowpowerspeedmax,temspeedmax,Isenable,vibInterval,temInterval);
    QMessageBox::information(this,"提示","设置成功");
}

void frmRunningGearConfing::on_btn_getbearing_clicked()
{
    if(currentwagon.isEmpty()) return;
    QString model = ui->cbox_bearing->currentText();
    UDPMulticastAPI::Instance()->GetBearingInfo(currentwagon,model);
}

void frmRunningGearConfing::on_btn_addbearing_clicked()
{
    if (ui->txt_model->text().isEmpty() || ui->txt_pitchDiameter->text().isEmpty() ||
            ui->txt_rollingDiameter->text().isEmpty() || ui->txt_rollerNum->text().isEmpty() ||
            ui->txt_contactAngle->text().isEmpty() || ui->cbox_bearing->currentIndex() == -1){
        QMessageBox::warning(this,"提示","未填写完整");
        return;
    }

    if(currentwagon.isEmpty()) return;


    QString model = ui->txt_model->text();

    int index = ui->cbox_bearing->findText(model);
    if (index != -1) {
        QMessageBox::warning(this,"提示","当前型号已存在");
        return;
    }

    float pitchDiameter = ui->txt_pitchDiameter->text().toFloat();
    float rollingDiameter = ui->txt_rollingDiameter->text().toFloat();
    int rollerNum = ui->txt_rollerNum->text().toInt();
    float contactAngle = ui->txt_contactAngle->text().toFloat();
    //添加进下拉框
    ui->cbox_bearing->addItem(model);
    //发送至走行部
    UDPMulticastAPI::Instance()->UpdateBearing(currentwagon,0,model,pitchDiameter,rollingDiameter,rollerNum,contactAngle);
    QMessageBox::information(this,"提示","添加成功");
}

void frmRunningGearConfing::on_btn_modifybearing_clicked()
{
    if (ui->txt_model->text().isEmpty() || ui->txt_pitchDiameter->text().isEmpty() ||
            ui->txt_rollingDiameter->text().isEmpty() || ui->txt_rollerNum->text().isEmpty() ||
            ui->txt_contactAngle->text().isEmpty() || ui->cbox_bearing->currentIndex() == -1){
        QMessageBox::warning(this,"提示","未填写完整");
        return;
    }
    QString currentmodel = ui->txt_model->text();
    int index = ui->cbox_bearing->findText(currentmodel);
    if (index != -1) {
        QMessageBox::warning(this,"提示","未找到该型号");
        return;
    }

    float pitchDiameter = ui->txt_pitchDiameter->text().toFloat();
    float rollingDiameter = ui->txt_rollingDiameter->text().toFloat();
    int rollerNum = ui->txt_rollerNum->text().toInt();
    float contactAngle = ui->txt_contactAngle->text().toFloat();

    UDPMulticastAPI::Instance()->UpdateBearing(currentwagon,1,currentmodel,pitchDiameter,rollingDiameter,rollerNum,contactAngle);
    QMessageBox::information(this,"提示","修改成功");
}

void frmRunningGearConfing::on_btn_deletebearing_clicked()
{
    QString currentmodel = ui->cbox_bearing->currentText();
    int index = ui->cbox_bearing->currentIndex();
    if(QMessageBox::question(this,"提示",QString("确认删除轴承: %1 吗？").arg(currentmodel)) == QMessageBox::Yes){
        float pitchDiameter = 0.0;
        float rollingDiameter =0.0;
        int rollerNum = 0;
        float contactAngle = 0.0;

        UDPMulticastAPI::Instance()->UpdateBearing(currentwagon,2,currentmodel,pitchDiameter,rollingDiameter,rollerNum,contactAngle);

        ui->cbox_bearing->removeItem(index);
        QMessageBox::information(this,"提示","已删除");
    }
}

void frmRunningGearConfing::on_btn_getnetinfo_clicked()
{
    if(currentwagon.isEmpty()) return;
    UDPMulticastAPI::Instance()->GetNetworkConfig(currentwagon);
}

void frmRunningGearConfing::on_btn_setnetinfo_clicked()
{
    if (ui->txt_eth0_ip->text().isEmpty() || ui->txt_eth0_mask->text().isEmpty() ||
            ui->txt_eth0_gateway->text().isEmpty() || ui->txt_eth0_dns->text().isEmpty() ||
            ui->txt_eth1_ip->text().isEmpty() || ui->txt_eth1_mask->text().isEmpty() ||
            ui->txt_eth1_gateway->text().isEmpty() || ui->txt_eth1_dns->text().isEmpty() ||
            ui->txt_serverip->text().isEmpty() || ui->txt_serverport->text().isEmpty() ||
            ui->txt_listenport->text().isEmpty() || ui->txt_standbyip->text().isEmpty() ||
            ui->txt_standbyport->text().isEmpty() || ui->txt_udpip->text().isEmpty() ||
            ui->txt_udpport->text().isEmpty() || currentwagon.isEmpty()){
        QMessageBox::warning(this,"提示","未填写完整");
        return;
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

    UDPMulticastAPI::Instance()->UpdateNetWorkConfing(currentwagon,ip,mask,gateway,dns,
                                                      ip1,mask1,gateway1,dns1,
                                                      serverip,serverport,listenport,standbyip,
                                                      standbyport,udpip,udpport);
    QMessageBox::information(this,"提示","修改成功");
}

void frmRunningGearConfing::on_btn_getalarmlimit_clicked()
{
    if(currentwagon.isEmpty())return;

    UDPMulticastAPI::Instance()->GetAlarmLimit(currentwagon);
}

void frmRunningGearConfing::on_btn_setalarmlimit_clicked()
{
    bool isempty = false;
    for( auto txt : txtAlarmLimit){
        if(txt->text().isEmpty()){
            isempty = true;
            break;
        }
    }

    if(isempty){
        QMessageBox::warning(this,"提示","信息填写不完整");
        return;
    }

    QStringList valuelist;
    for( auto txt : txtAlarmLimit){
        valuelist.append(txt->text());
    }
    UDPMulticastAPI::Instance()->UpdateAlarmLimit(currentwagon,valuelist);
    QMessageBox::information(this,"提示","修改成功");
}

void frmRunningGearConfing::on_btn_reboot_clicked()
{
    if(currentwagon.isEmpty()) return;
    if(QMessageBox::question(this,"二次确认",QString("确认重启设备 %1 吗").arg(currentwagon)) == QMessageBox::Yes){
        UDPMulticastAPI::Instance()->DeviceReboot(currentwagon);
    }
}

void frmRunningGearConfing::on_btn_DeviceUpgrade_clicked()
{
    if(currentwagon.isEmpty()){
        QMessageBox::warning(nullptr,"错误","请选择升级车厢");
        return;
    }
    DeviceUpgrade upgradeview(this,currentwagon);
    upgradeview.exec();
}

void frmRunningGearConfing::on_btn_getversion_clicked()
{
    if(currentwagon.isEmpty()) return;
    UDPMulticastAPI::Instance()->GetVersion(currentwagon);
}

void frmRunningGearConfing::on_btn_Datadownload_clicked()
{
    if(!M_TcpAPI::Instance()->GetConnectState()){
        QMessageBox::warning(nullptr,"失败","走行部未连接");
        return;
    }
    DataDownload downloadview(this);
    downloadview.exec();
}

void frmRunningGearConfing::on_btn_clear_clicked()
{
    clearFrom();
}

void frmRunningGearConfing::on_cbox_bearing_currentIndexChanged(const QString &arg1)
{
    //取出名称，然后去匹配信息进行显示
    QString model = arg1;
    for(auto bearing : PreData::bearinglist){
        if(bearing.Model == model){
            ui->txt_model->setText(model);
            ui->txt_pitchDiameter->setText(QString::number(bearing.PitchDiameter));
            ui->txt_rollingDiameter->setText(QString::number(bearing.RollingDiameter));
            ui->txt_rollerNum->setText(QString::number(bearing.RollerNum));
            ui->txt_contactAngle->setText(QString::number(bearing.ContactAngle));
            break;
        }
    }
}

void frmRunningGearConfing::on_btn_deleteData_clicked()
{
    if(QMessageBox::question(nullptr,"清空数据","确认清空走行部所有原始数据?") == QMessageBox::Yes){
        M_TcpAPI::Instance()->ClearZIPData();
    }
}
