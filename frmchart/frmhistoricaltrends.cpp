#include "frmhistoricaltrends.h"
#include "ui_frmhistoricaltrends.h"
#include "m_dbapi.h"
#include "predata.h"
frmHistoricalTrends::frmHistoricalTrends(QStringList selectdevice, bool IsEnvelope, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmHistoricalTrends),
    deviceList(selectdevice),
    envelope(IsEnvelope)
{
    ui->setupUi(this);
    InitDevice();
    InitFrm();
}

frmHistoricalTrends::~frmHistoricalTrends()
{
    delete ui;
}

void frmHistoricalTrends::InitFrm()
{
    DataGetInit();
    SeleteInit();
    ImageCheckInit();
    if(!M_DbAPI::Instance()->GetDataConnectState()){
        QMessageBox::warning(nullptr,"连接错误","数据库未连接");
        ui->btnSelete->setEnabled(false);
        ui->btnImageLoad->setEnabled(false);
    }else{
        on_btnSelete_clicked();
//        connect(ui->widgetCustomplot,&QCustomPlot::plottableDoubleClick,this,&frmHistoricalTrends::mousedoubleclicked);
    }
}

void frmHistoricalTrends::InitDevice()
{
    if(deviceList.isEmpty()) return;
    HistoryDevice = QList<HISTORYDEVICE>();
    //提取名称，找到id和ch
    for(auto device : deviceList){
        QStringList parts = device.split("(");
        if (parts.size() == 2) {
            QString devicename = parts.first();//设备名称
            QStringList wagon2 = parts.last().remove(")").split(":");
            QString devicewagon = wagon2.last();//车厢号
            //找出对应的id和ch
            for(int i=0;i<PreData::devicelist.size();i++){
                PreData::DEVICEINFO info = PreData::devicelist.at(i);
                if((info.Wagon == devicewagon) && (info.name == devicename)){
                    HISTORYDEVICE history;
                    history.id = info.id;
                    history.ch = info.ch;
                    history.name = devicename;
                    history.wagon = devicewagon;
                    history.values = QVector<double>();
                    history.keys = QVector<double>();
                    HistoryDevice.append(history);
                    break;
                }
            }
        }
    }
}

void frmHistoricalTrends::DataGetInit()
{
    ui->datetimeStart->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->dateTimeEnd->setDisplayFormat("yyyy-MM-dd HH:mm:ss");

    QDateTime starttime = QDateTime::fromString("2024-01-01 00:00:00","yyyy-MM-dd HH:mm:ss");
    QDateTime endtime = QDateTime::fromString("2124-12-31 23:59:59","yyyy-MM-dd HH:mm:ss");
    ui->datetimeStart->setDateTimeRange(starttime,endtime);
    ui->dateTimeEnd->setDateTimeRange(starttime,endtime);

//    ui->datetimeStart->setDateTime(QDateTime::currentDateTime().addDays(-3));
//    ui->dateTimeEnd->setDateTime(QDateTime::currentDateTime());

    ui->datetimeStart->setDateTime(QDateTime::fromString(App::HistoryDataStartTime,"yyyy-MM-dd HH:mm:ss"));
    ui->dateTimeEnd->setDateTime(QDateTime::fromString(App::HistoryDataEndTime,"yyyy-MM-dd HH:mm:ss"));
}

void frmHistoricalTrends::SeleteInit()
{
    ColumnNames.clear();
    ColumnNames << "Max" << "Min" << "Mean" << "Variance" << "std_deviation" << "Rms"
                << "Rectified_mean" << "Kurtosis" << "peak_to_peak" << "crest_factor"
                << "Peak_factor" << "Impulse_factor" << "Margin_factor" << "AmbientTem" << "PointTem";

    QList <QSpinBox *> lists  = ui->widget_select->findChildren<QSpinBox *>();
    foreach(QSpinBox * sbox,lists){
        sbox->setRange(0,2000);
        sbox->setValue(500);
        sbox->setSuffix(" r/min");
        sbox->setEnabled(false);
    }

    QList <QDoubleSpinBox *> doublelist = ui->widget_select->findChildren<QDoubleSpinBox *>();
    foreach(QDoubleSpinBox * dsbox,doublelist){
        dsbox->setRange(-500.00,500.00);
        dsbox->setValue(0.00);
        dsbox->setEnabled(false);
    }
    ui->cboxScreenSpeed->setChecked(false);
    ui->cboxScreenSpeed->setChecked(false);
}

void frmHistoricalTrends::ImageCheckInit()
{

    menubtns.clear();
    menubtns.append(ui->rbtn_max);
    menubtns.append(ui->rbtn_min);
    menubtns.append(ui->rbtn_mean);
    menubtns.append(ui->rbtn_variance);
    menubtns.append(ui->rbtn_std_deviation);
    menubtns.append(ui->rbtn_rms);
    menubtns.append(ui->rbtn_rectifiedmean);
    menubtns.append(ui->rbtn_kurtosis);
    menubtns.append(ui->rbtn_peak_to_peak);
    menubtns.append(ui->rbtn_crest_factor);
    menubtns.append(ui->rbtn_peak_factor);
    menubtns.append(ui->rbtn_Impulse_factor);
    menubtns.append(ui->rbtn_margin_factor);
    menubtns.append(ui->rbtn_AmbientTem);
    menubtns.append(ui->rbtn_PointTem);

    lineColors.clear();
    lineColors.append(QColor(240,135,132));
    lineColors.append(QColor(235,51,36));
    lineColors.append(QColor(119,67,66));
    lineColors.append(QColor(115,251,253));
    lineColors.append(QColor(0,35,245));
    lineColors.append(QColor(116,27,124));
    lineColors.append(QColor(240,134,80));
    lineColors.append(QColor(129,127,38));
    lineColors.append(QColor(0,2,61));
    lineColors.append(QColor(119,250,97));
    lineColors.append(QColor(238,138,248));
    lineColors.append(QColor(127,130,187));
    lineColors.append(QColor(192,192,192));
    ui->rbtn_max->setChecked(true);
    InitPlot();
}

void frmHistoricalTrends::InitPlot()
{
    QCustomPlot *customPlot = ui->widgetCustomplot;
    QFont font("Microsoft YaHei",12);
//    QColor textColor = QColor(255,255,255);
//    QColor backColor = QColor(80,80,80);
    for(int i=0;i<HistoryDevice.size();i++){
        customPlot->addGraph();
        customPlot->graph(i)->setName(HistoryDevice.at(i).name);//设置名称
        customPlot->graph(i)->setPen(lineColors.at(i));//设置颜色
//        customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc,8));//设置数据点显示图标
        customPlot->graph(i)->setLineStyle(QCPGraph::LineStyle::lsLine);//设置连接的方式
        customPlot->graph(i)->rescaleAxes(true);//设置数据自适应范围QCP::stSingleData
//        customPlot->graph(i)->setSelectable(QCP::SelectionType::stMultipleDataRanges);//数据多选
        customPlot->graph(i)->setSelectable(QCP::stSingleData);
    }
    customPlot->setLocale(QLocale(QLocale::Chinese,QLocale::UnitedKingdom));
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("HH:mm:ss\nyyyy-MM-dd");
    customPlot->xAxis->setTicker(dateTicker);

//    customPlot->xAxis->setVisible(true);

    //设置提示框字体
    customPlot->legend->setFont(font);
    //显示提示框
    customPlot->legend->setVisible(true);
    //设置画刷
    customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 125)));
    //将提示框放在左上角
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignLeft);
#if 0
    //设置背景颜色
    customPlot->setBackground(QBrush(backColor));
    //设置标签颜色
    customPlot->xAxis->setLabelColor(textColor);
    customPlot->yAxis->setLabelColor(textColor);
    //设置刻度标签颜色
    customPlot->xAxis->setTickLabelColor(textColor);
    customPlot->yAxis->setTickLabelColor(textColor);
    //设置X,Y轴颜色和宽度
    customPlot->xAxis->setBasePen(QPen(textColor, 1));
    customPlot->yAxis->setBasePen(QPen(textColor, 1));
    //设置刻度颜色
    customPlot->xAxis->setTickPen(QPen(textColor, 2));
    customPlot->yAxis->setTickPen(QPen(textColor, 2));
    //设置子刻度颜色(大刻度之间的小刻度)
    customPlot->xAxis->setSubTickPen(QPen(textColor, 1.5));
    customPlot->yAxis->setSubTickPen(QPen(textColor, 1.5));
#endif
    //设置刻度名称和名称标签
//    customPlot->xAxis->setTickLabels()
    //设置标签字体
    customPlot->xAxis->setLabel("时间");
    customPlot->yAxis->setLabel("幅值");
    customPlot->xAxis->setLabelFont(font);
    customPlot->yAxis->setLabelFont(font);
    //设置刻度标签字体
    customPlot->xAxis->setTickLabelFont(font);
    customPlot->yAxis->setTickLabelFont(font);
    //设置零刻度线的颜色
//    customPlot->xAxis->grid()->setZeroLinePen(QPen(QColor(Qt::yellow)));
//    customPlot->yAxis->grid()->setZeroLinePen(QPen(QColor(Qt::yellow)));

    //设置网格线的颜色和宽度
//    customPlot->xAxis->grid()->setPen(QPen(QColor(Qt::white), 1, Qt::DotLine));

    //滚动缩放、图表可选、多选
    customPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables| QCP::iMultiSelect);
//    ui->widgetCustomplot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);//使能鼠标左键选中放大  失能：QCP::SelectionRectMode::srmNone
    customPlot->setInteraction(QCP::iRangeDrag,true);//使能拖动  失能直接该true为false
#if 0
    //添加文本框,用于点击某点时显示信息
    textLabel = new QCPItemText(customPlot);//在QCustomplot中新建文字框
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);//文字布局：顶、右对齐
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);//位置类型（当前轴范围的比例为单位/实际坐标为单位）
    textLabel->position->setCoords(1, 0); //把文字框放在X轴的中间，Y轴的最顶部

    textLabel->setFont(font); //字体大小
    textLabel->setPen(QPen(Qt::black)); //设置边框颜色
    textLabel->setBrush(QBrush(QColor(255, 255, 255, 125)));//设置文本框背景
    textLabel->setColor(Qt::black);//设置文本颜色
    textLabel->setPadding(QMargins(2,2,2,2));//文字距离边框几个像素
    textLabel->setTextAlignment(Qt::AlignLeft);//设置文本对齐方式
    textLabel->setText("类型: \n时间: \n幅值: ");
#endif
    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QColor(255,255,255,150));
#if 1
    //添加左键按下显示X，Y
//    connect(customPlot,&QCustomPlot::mousePress,this,&frmHistoricalTrends::plot_mousepress);

    //右键菜单自定义
    customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    //信号连接槽函数
    connect(customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
#endif
}

QString frmHistoricalTrends::GetWidgettitle() const
{
    return this->Widgettitle;
}

void frmHistoricalTrends::SetWidgettitle(QString name)
{
    this->Widgettitle = name;
}

void frmHistoricalTrends::on_btnImageLoad_clicked()
{
    //找出当前选中的数据类型
    int index = -1;
    for(int i=0;i<menubtns.size();i++){
        if(menubtns.at(i)->isChecked()){
            index = i;
            break;
        }
    }
    if(index == -1) return;

    //找出要查询的数据类型
    QString selectedColumn = ColumnNames.at(index);

    //构建SQL语句,加载数据
    QString queryStr;
    QSqlQuery query(M_DbAPI::Instance()->GetDataDBHandle());

    QString tablename = "DimensionalValue";
    if(envelope){
        tablename = "DemodulatedValue";
    }
    for(auto &device : HistoryDevice){


         queryStr = QString("SELECT TriggerTime,%1 FROM %2 WHERE TriggerTime BETWEEN :starttime AND :endtime "
                            "AND DeviceID = :deviceID "
                            "AND DeviceCH = :deviceCH "
                            "AND WagonNumber = :wagonNumber")
                 .arg(selectedColumn).arg(tablename);

           if (FilterCriteria.usespeed) {
               queryStr += " AND Speed BETWEEN :speedMin AND :speedMax";
           }

           if (FilterCriteria.uselimit) {
               queryStr += QString(" AND %1 BETWEEN :limitMin AND :limitMax").arg(selectedColumn);

           }
           query.prepare(queryStr);

           // 绑定参数
           query.bindValue(":starttime", FilterCriteria.starttime);
           query.bindValue(":endtime", FilterCriteria.endtime);
           query.bindValue(":deviceID", device.id);
           query.bindValue(":deviceCH", device.ch);
           query.bindValue(":wagonNumber", device.wagon);

           if (FilterCriteria.usespeed) {
               query.bindValue(":speedMin", FilterCriteria.speedMin);
               query.bindValue(":speedMax", FilterCriteria.speedMax);
           }

           if (FilterCriteria.uselimit) {
               query.bindValue(":limitMin", FilterCriteria.limitMin);
               query.bindValue(":limitMax", FilterCriteria.limitMax);
           }
//        qDebug() << "Generated Query:" << queryStr;  // 查看动态生成的 SQL 语句
//        for (const auto &key : query.boundValues().keys()) {
//            qDebug() << key << ":" << query.boundValues().value(key).toString();
//        }
        if (query.exec()){
            //填充value和key
            device.keys.clear();
            device.values.clear();
             while (query.next()){
                 double d_time = QDateTime::fromString(query.value(0).toString(),"yyyy-MM-dd HH:mm:ss").toTime_t();
                 device.keys.append(d_time);
                 device.values.append(query.value(1).toDouble());
             }
        }
    }

    //刷新界面
    QCustomPlot *customPlot = ui->widgetCustomplot;
    CustomPlot_clear(customPlot);

    for (int i=0;i<HistoryDevice.size();i++) {
        customPlot->graph(i)->setData(HistoryDevice.at(i).keys,HistoryDevice.at(i).values);
        customPlot->graph(i)->rescaleAxes(true);
    }
    customPlot->rescaleAxes();
    customPlot->replot();
}

void frmHistoricalTrends::on_widgetCustomplot_selectionChangedByUser()
{
    QCustomPlot* customPlot=ui->widgetCustomplot;

    for(int i=0;i<customPlot->graphCount();i++)
    {
        //遍历有被选中的graph
        if(customPlot->graph(i)->selected())
        {
            QCPDataSelection selection =customPlot->graph(i)->selection();
            //遍历选中范围
            for(int j=0;j<selection.dataRangeCount();j++)
            {
                QCPDataRange dataRange = selection.dataRange(j);
                //遍历数据
                for(int k=dataRange.begin();k<dataRange.end();k++)
                {
                    double double_key = customPlot->graph(i)->data()->at(k)->key;
                    QString str_value = QString::number(customPlot->graph(i)->data()->at(k)->value);
                    QString str_at= customPlot->graph(i)->name();
                    QString str_key = QDateTime::fromTime_t(double_key).toString("yyyy-MM-dd HH:mm:ss");
                    QString dataText = QString("类型: %1\n时间: %2\n幅值: %3").arg(str_at).arg(str_key).arg(str_value);

                    QToolTip::showText(QCursor::pos(), dataText);
                }
            }
        }
    }
}

void frmHistoricalTrends::CustomPlot_clear(QCustomPlot *customPlot)
{
    int count = customPlot->graphCount();
    for(int i=0;i<count;i++){
        customPlot->graph(i)->clearData();
    }
}


void frmHistoricalTrends::on_cboxScreenValue_clicked(bool checked)
{
    ui->sboxValueUp->setEnabled(checked);
    ui->sboxValueDown->setEnabled(checked);
}

void frmHistoricalTrends::on_cboxScreenSpeed_clicked(bool checked)
{
    ui->sboxSpeedUp->setEnabled(checked);
    ui->sboxSpeedDown->setEnabled(checked);
}

void frmHistoricalTrends::on_btnSelete_clicked()
{
    if(ui->dateTimeEnd->date() < ui->datetimeStart->date()){
        QMessageBox::warning(this,"错误","结束时间不能小于开始时间");
        return;
    }
    bool speedchecked = ui->cboxScreenSpeed->isChecked();
    bool Valuechecked = ui->cboxScreenValue->isChecked();
    double speedup = ui->sboxSpeedUp->value();
    double speeddown = ui->sboxSpeedDown->value();
    double valueup = ui->sboxValueUp->value();
    double valuedown = ui->sboxValueDown->value();
    QString starttime = ui->datetimeStart->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString endtime = ui->dateTimeEnd->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    App::HistoryDataStartTime = starttime;
    App::HistoryDataEndTime = endtime;
    App::writeConfig();

    //更新筛选条件结构体
    FilterCriteria.usespeed = speedchecked;
    FilterCriteria.uselimit = Valuechecked;
    FilterCriteria.speedMin = speeddown;
    FilterCriteria.speedMax = speedup;
    FilterCriteria.limitMin = valuedown;
    FilterCriteria.limitMax = valueup;
    FilterCriteria.starttime = starttime;
    FilterCriteria.endtime = endtime;


    //更新当前选中的界面
    on_btnImageLoad_clicked();
}

void frmHistoricalTrends::plot_mousepress(QMouseEvent *event)
{
    if(event->type() == QMouseEvent::MouseButtonPress){
        if(event->button() == Qt::LeftButton){//左键
            QCustomPlot *customPlot = ui->widgetCustomplot;
            // 获得鼠标在图表上的像素坐标
            QPointF pos = customPlot->mapFromGlobal(event->globalPos());
            // 将像素坐标转换为图表坐标
            double x = customPlot->xAxis->pixelToCoord(pos.x());
            double y = customPlot->yAxis->pixelToCoord(pos.y());
            qDebug()<< "x = " << x << ", y = " << y;
        }
    }
}

void frmHistoricalTrends::rescaleAxes()
{
    QCustomPlot* customPlot=ui->widgetCustomplot;
    customPlot->xAxis->rescale();
    customPlot->yAxis->rescale();
    customPlot->replot();
//    int count = customPlot->graphCount();
//    for(int i=0;i<count;i++){
//        customPlot->graph(i)->rescaleAxes(true);

//    }
//    customPlot->replot();
}

void frmHistoricalTrends::contextMenuRequest(QPoint pos)
{
    QCustomPlot* customPlot=ui->widgetCustomplot;
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction("调整范围",this, SLOT(rescaleAxes()));
    menu->popup(customPlot->mapToGlobal(pos));
}

void frmHistoricalTrends::mousedoubleclicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    HISTORYDEVICE selectdevice;
    QString str_key;
    //获取到点击的点
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (auto *graph = qobject_cast<QCPGraph *>(plottable)) {
            double key = graph->data()->at(dataIndex)->key;
            QString str_at= graph->name();
            str_key = QDateTime::fromTime_t(key).toString("yyyyMMddHHmmss");

            //循环匹配设备列表
            for(auto device : HistoryDevice){
                if(device.name == str_at){
                    selectdevice = device;
                }
            }
        }
    }

    //稍加判断后，组成标准的文件名称并去查找是否存在该文件

}
