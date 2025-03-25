#include "frmlivepreview.h"
#include "ui_frmlivepreview.h"
#include <QToolBar>
#include <QFileDialog>
#include <QRandomGenerator>
#include "predata.h"
#include "m_networkapi.h"
#include "m_dbapi.h"
#include "m_screenshot.h"

frmLivePreview::frmLivePreview(QStringList selectdevice, bool Isdemodulated, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmLivePreview),
    deviceList(selectdevice),
    isdemodulated(Isdemodulated)
{
    ui->setupUi(this);
    m_SpeedChartWidget = nullptr;
    m_DBChartWidget = nullptr;
    m_faultFrequency = nullptr;
    FaultFrequency_labels.clear();
    FaultFrequency_lines.clear();
    InitDevice();
    InitFrm();
    DataImport();
}

frmLivePreview::~frmLivePreview()
{
    disconnect(M_NetworkAPI::Instance(),&M_NetworkAPI::ReturnTemData,this,&frmLivePreview::DataImport);
    delete ui;
}

void frmLivePreview::InitDevice()
{
    if(deviceList.isEmpty()) return;
    QString device = deviceList.at(0);
    QStringList parts = device.split("(");
    if (parts.size() == 2) {
        QString devicename = parts.first();//设备名称
        QStringList wagon2 = parts.last().remove(")").split(":");
        QString devicewagon = wagon2.last();//车厢号
        //找出对应的id和ch
        for(int i=0;i<PreData::devicelist.size();i++){
            PreData::DEVICEINFO info = PreData::devicelist.at(i);
            if((info.Wagon == devicewagon) && (info.name == devicename)){
                livedeviceinfo.id = info.id;
                livedeviceinfo.ch = info.ch;
                livedeviceinfo.name = devicename;
                livedeviceinfo.wagon = devicewagon;
                livedeviceinfo.sensitivity = info.sensitivity;
                livedeviceinfo.currentecho = UIECHODATA();
                livedeviceinfo.FileValues = QVector<double>();
                livedeviceinfo.FileKeys = QVector<double>();
            }
        }
    }
}

void frmLivePreview::InitFrm()
{
    RPM = 1000;
    //    SetlabInfoName("xx车xx车厢走行部2号通道4");
    //    SetlabInfoTime("时间：" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    //    SetlabInfoAlarm("报警：温度一级报警 振动二级报警");
    ui->StartDateTime->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->EndDateTime->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->StartDateTime->setDateTime(QDateTime::fromString(App::LiveDataStartTime,"yyyy-MM-dd HH:mm:ss"));
    ui->EndDateTime->setDateTime(QDateTime::fromString(App::LiveDataEndTime,"yyyy-MM-dd HH:mm:ss"));
    //    ui->StartDateTime->setDateTime(QDateTime::currentDateTime().addDays(-3));
    //    ui->EndDateTime->setDateTime(QDateTime::currentDateTime());
    plotInit();

    //获取全部值
    txt_DDimension.clear();
    txt_ODimension.clear();

    QList<QLabel *> labellist = ui->widget_Dimension->findChildren<QLabel *>();
    for(QLabel * label : labellist){
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    txt_ODimension.append(ui->txt_Omax);
    txt_ODimension.append(ui->txt_Omin);
    txt_ODimension.append(ui->txt_Omean);
    txt_ODimension.append(ui->txt_Ovariance);
    txt_ODimension.append(ui->txt_Odeviation);
    txt_ODimension.append(ui->txt_Orms);
    txt_ODimension.append(ui->txt_Orectifiedmean);
    txt_ODimension.append(ui->txt_Okurtosis);
    txt_ODimension.append(ui->txt_Opeaktopeak);
    txt_ODimension.append(ui->txt_Ocrest_factor);
    txt_ODimension.append(ui->txt_Opeak_factor);
    txt_ODimension.append(ui->txt_Oimpulse_factor);
    txt_ODimension.append(ui->txt_Omargin_factor);

    txt_DDimension.append(ui->txt_Dmax);
    txt_DDimension.append(ui->txt_Dmin);
    txt_DDimension.append(ui->txt_Dmean);
    txt_DDimension.append(ui->txt_Dvariance);
    txt_DDimension.append(ui->txt_Ddeviation);
    txt_DDimension.append(ui->txt_Drms);
    txt_DDimension.append(ui->txt_Drectifiedmean);
    txt_DDimension.append(ui->txt_Dkurtosis);
    txt_DDimension.append(ui->txt_Dpeaktopeak);
    txt_DDimension.append(ui->txt_Dcrest_factor);
    txt_DDimension.append(ui->txt_Dpeak_factor);
    txt_DDimension.append(ui->txt_Dimpulse_factor);
    txt_DDimension.append(ui->txt_Dmargin_factor);

    for(QLineEdit *txt : txt_ODimension){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }

    for(QLineEdit *txt : txt_DDimension){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }

    ui->line_3->setVisible(false);
    ui->widget_Dimension->setVisible(false);
    ui->btn_SpeedVector->setCheckable(true);
    ui->btn_closeTrend->setCheckable(true);
    ui->btn_echoDBValue->setCheckable(true);
    ui->btn_faultFrequency->setCheckable(true);
    ui->widget_bottom->setEnabled(false);
    connect(M_NetworkAPI::Instance(),&M_NetworkAPI::ReturnTemData,this,&frmLivePreview::DataImport);
}

void frmLivePreview::plotInit(void)
{
    QString originaltable = "波 形 图";
    QString Spectrumtable = "频 谱 图";
    if(isdemodulated){
        originaltable = "包 络 图";
        Spectrumtable = "包 络 频 谱 图";
    }
    ui->CustomPlotOriginal->yAxis->setLabel(originaltable);
    ui->CustomPlotOriginal->xAxis->setLabel("time (s)");
    ui->CustomPlotOriginal->yAxis->setLabelPadding(10);
    ui->CustomPlotOriginal->yAxis->setLabelFont(QFont("Microsoft YaHei",12));
    ui->CustomPlotOriginal->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables| QCP::iMultiSelect);
    ui->CustomPlotOriginal->setInteraction(QCP::iRangeDrag,true);//使能拖动  失能直接该true为false

    ui->CustomPlot_Spectrum->yAxis->setLabel(Spectrumtable);
    ui->CustomPlot_Spectrum->xAxis->setLabel("Frequency (Hz)");
    ui->CustomPlot_Spectrum->yAxis->setLabelPadding(10);
    ui->CustomPlot_Spectrum->yAxis->setLabelFont(QFont("Microsoft YaHei",12));
    ui->CustomPlot_Spectrum->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables);//| QCP::iMultiSelect
    ui->CustomPlot_Spectrum->setInteraction(QCP::iRangeDrag,true);//使能拖动  失能直接该true为false

    ui->CustomPlot_Trend->yAxis->setLabel("趋 势 图");
    ui->CustomPlot_Trend->yAxis->setLabelPadding(10);
    ui->CustomPlot_Trend->yAxis->setLabelFont(QFont("Microsoft YaHei",12));
    ui->CustomPlot_Trend->setLocale(QLocale(QLocale::Chinese,QLocale::UnitedKingdom));
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("HH:mm:ss\nyyyy-MM-dd");//d,MMMM\nyyyy
    QDateTime epochDateTime = QDateTime::fromString("2024-01-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
    qint64 secondsSinceEpoch = epochDateTime.toSecsSinceEpoch();
    ui->CustomPlot_Trend->xAxis->setRange(secondsSinceEpoch,secondsSinceEpoch + 31536000 );
    ui->CustomPlot_Trend->xAxis->setTicker(dateTicker);
    ui->CustomPlot_Trend->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables| QCP::iMultiSelect);//滚动缩放<<图表可选<<多选
    ui->CustomPlot_Trend->setInteraction(QCP::iRangeDrag,true);//使能拖动  失能直接该true为false

    connect(ui->CustomPlotOriginal,&QCustomPlot::plottableClick,this,&frmLivePreview::TrendpopupToolTip);
    connect(ui->CustomPlot_Spectrum,&QCustomPlot::plottableClick,this,&frmLivePreview::TrendpopupToolTip);
    connect(ui->CustomPlot_Trend,&QCustomPlot::plottableClick,this,&frmLivePreview::TrendpopupToolTip);
    connect(ui->CustomPlot_Trend,&QCustomPlot::plottableDoubleClick,this,&frmLivePreview::TrendLoadFile);
    AddToolbar(ui->CustomPlotOriginal,1);
    AddToolbar(ui->CustomPlot_Spectrum,2);
    AddToolbar(ui->CustomPlot_Trend,3);
}

void frmLivePreview::AddToolbar(QCustomPlot *customplot,int type)
{
    //波形图添加工具栏
    QToolBar *toolbar1 = new QToolBar(customplot);
    toolbar1->setIconSize(QSize(24,24));
    toolbar1->setMovable(false);
    QAction *magnifier1 = toolbar1->addAction(QIcon(":/image/image/Icon/magnifier.png"),"框选");
    QAction *reset1 = toolbar1->addAction(QIcon(":/image/image/Icon/reset.png"),"重新加载");
    QAction *download1 = toolbar1->addAction(QIcon(":/image/image/Icon/download.png"),"下载");
    QAction *fullscreen1 = toolbar1->addAction(QIcon(":/image/image/Icon/fullscreen.png"), "全屏");
    magnifier1->setCheckable(true);  // 使得 magnifier1 可以被切换

    connect(download1, &QAction::triggered, [=]() {
        QString fileName = QFileDialog::getSaveFileName(nullptr,"保存图片","",
                                                        "png (*.png);;"
                                                        "jpg(*.jpg);;"
                                                        "bmp(*.bmp);;"
                                                        "pdf(*.pdf);;"
                                                        "All Files (*)");
        if(!fileName.isEmpty()){
            QFileInfo fileInfo(fileName);
            QString suffix = fileInfo.suffix().toLower();  // 转为小写，避免大小写问题
            if(suffix == "jpg"){
                customplot->saveJpg(fileName);
            }else if(suffix == "png"){
                customplot->savePng(fileName);
            }else if(suffix == "bmp"){
                customplot->saveBmp(fileName);
            }else if(suffix == "pdf"){
                customplot->savePdf(fileName);
            }else{
                QMessageBox::warning(this,"Error","请选择保存的类型");
            }
        }
    });
    connect(magnifier1,&QAction::toggled, [=](bool arg1) {
        if(arg1){
            customplot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
        }else{
            customplot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        }

    });

    connect(reset1,&QAction::triggered,[=]{
        customplot->xAxis->rescale();
        customplot->yAxis->rescale();
        customplot->replot();
    });

    connect(fullscreen1,&QAction::triggered,[=]{
        QList<QCustomPlot*> viewlist = ui->widget->findChildren<QCustomPlot*>();
        if(fullscreen1->text() == "全屏"){
            for (QCustomPlot* view : viewlist) {
                if (view != customplot) {
                    view->setVisible(false);  // 将其余控件设置为不可见
                }
            }
            fullscreen1->setIcon(QIcon(":/image/image/Icon/fullscreen_exit.png"));
            fullscreen1->setText("缩小");
        }else if(fullscreen1->text() == "缩小"){
            for (QCustomPlot* view : viewlist) {
                if (view != customplot) {
                    view->setVisible(true);  // 将其余控件设置为不可见
                }
            }
            fullscreen1->setIcon(QIcon(":/image/image/Icon/fullscreen.png"));
            fullscreen1->setText("全屏");
        }
    });

    if(type == 1){//波形图，增加对比按钮
        QAction *DifferFre = toolbar1->addAction(QIcon(":/image/image/Icon/fre_differ.png"),"频率差");
        connect(DifferFre,&QAction::triggered,[=]{
            QVector<double> points;
            for (int i = 0; i < customplot->graphCount(); ++i) {
                QCPGraph *graph = customplot->graph(i);
                QCPDataSelection selection = graph->selection();

                // 遍历选中范围
                for (int j = 0; j < selection.dataRangeCount(); ++j) {
                    QCPDataRange dataRange = selection.dataRange(j);

                    // 遍历每个选中数据点
                    for (int k = dataRange.begin(); k < dataRange.end(); ++k) {
                        double key = graph->data()->at(k)->key;
                        double value = graph->data()->at(k)->value;
                        points << key;
                        // 输出或保存选中的数据点信息
                        qDebug() << "选中的点: key=" << key << ", value=" << value;
                    }
                }
            }
            if(points.size() > 1){
                for(int i =points.size();i>1;i--){
                    double differ = points.at(i-1) - points.at(i-2);
                    differ = 1/differ;
                    QString info = QString("点%1与%2之间频率差为%3").arg(points.at(i-1)).arg(points.at(i-2)).arg(differ);
                    QMessageBox::information(this,"频率差",info);
                }
            }
            points.clear();
        });

    }else if(type == 2){
        //定义全局变量
        static QMap<double, QCPItemLine*> verticalLines;//特别标记线容器
        static QMap<double, QCPItemText*> labels;//特别标记标签容器
        static QMap<double, QCPItemLine*> pinpulines;//等频率差值线容器
        static QMap<double, QCPItemText*> pinpulabels;//等频率差值标签容器
        static QMap<double, QCPItemLine*> frequency_lines;//倍频线容器
        static QMap<double, QCPItemText*> frequency_labels;//倍频标签容器
        static QList<QCPItemLine*> localtion_lines;
        static QList<QCPItemText*> localtion_labels;

        /*******等频率差值******/
        QAction *Pinpu = toolbar1->addAction(QIcon(":/image/image/Icon/pinpu.png"),"等频率差");
        connect(Pinpu,&QAction::triggered,[=]{
            // 获取选中频率
            double centerFreq = 0.0;
            for (int i = 0; i < customplot->graphCount(); ++i) {
                QCPGraph *graph = customplot->graph(i);
                QCPDataSelection selection = graph->selection();

                // 确保只选中一个数据点
                if (selection.dataRangeCount() == 1) {
                    QCPDataRange dataRange = selection.dataRange(0);  // 获取选中的数据范围

                    // 只处理单个点
                    if (dataRange.size() == 1) {
                        centerFreq = graph->data()->at(dataRange.begin())->key;  // 获取选中点的x值
                    }
                }
            }
            if(centerFreq == 0) return;
            // 弹出输入对话框让用户输入频率差和谱线数量
            bool ok;
            double deltaFreq = QInputDialog::getDouble(this, tr("输入频率差"), tr("频率差 (Hz):"), 50.0, 0, 1e6, 2, &ok);
            if (!ok) return;

            int numLines = QInputDialog::getInt(this, tr("输入谱线数量"), tr("每侧谱线数量:"), 3, 1, 10, 1, &ok);
            if (!ok) return;

            // 清除现有的标注和垂直线
            for (QCPItemLine* line : pinpulines) {
                customplot->removeItem(line);  // 删除垂直线
            }
            for (QCPItemText* label : pinpulabels) {
                customplot->removeItem(label);  // 删除标注
            }
            pinpulines.clear();
            pinpulabels.clear();


            //添加底层图层，否则当前图层覆盖区域将不可选中
            if (!customplot->layer("PinpuLayer")) {
                customplot->addLayer("PinpuLayer", customplot->layer("grid"));
            }


            // 左右各 numLines 条线
            for (int i = -numLines; i <= numLines; ++i) {
                //                     if (i == 0) continue; // 跳过中心频率

                double freq = centerFreq + i * deltaFreq;

                // 添加垂直线
                QCPItemLine *line = new QCPItemLine(customplot);
                line->start->setCoords(freq, 0);
                line->end->setCoords(freq, customplot->yAxis->range().upper);
                line->setPen(QPen(Qt::DashLine));
                //                if(i==0){
                //                    line->setPen(QPen(QColor(255, 165, 0), 1, Qt::DashLine));
                //                }else{
                //                    line->setPen(QPen(Qt::red, 1, Qt::DashLine));
                //                }
                line->setLayer("PinpuLayer");
                pinpulines[freq] = line;
                // 添加标注
                QCPItemText *label = new QCPItemText(customplot);
                label->position->setType(QCPItemPosition::ptPlotCoords);
                label->position->setCoords(freq, customplot->yAxis->range().upper);
                label->setText(QString::number(freq, 'f', 1) + " Hz");
                label->setFont(QFont("Arial", 8));
                label->setColor(Qt::black);
                label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
                label->setLayer("PinpuLayer");
                pinpulabels[freq] = label;
            }
            // 刷新图表
            customplot->replot();
        });

        /********hanning窗*******/
        QAction *Hanning = toolbar1->addAction(QIcon(":/image/image/Icon/hanning.png"),"汉宁窗");
        Hanning->setCheckable(true);
        Hanning->setChecked(true);
        connect(Hanning,&QAction::toggled, [=](bool arg1){//-----根据使用与否，重新加载视图
            UpdateSpectrumChart(arg1);
            if(arg1){
                QMessageBox::information(this,"Hanning Information","使用Hanning窗");
            }else{
                QMessageBox::information(this,"Hanning Information","取消Hanning窗");
            }
        });

        /*****柱状图<<折线图切换******/
        QAction *barchart = toolbar1->addAction(QIcon(":/image/image/Icon/bar_chart.png"),"切换柱状图");
        barchart->setCheckable(true);
        connect(barchart,&QAction::toggled, [=](bool arg1){
            // 清除当前图形
            customplot->clearGraphs();
            customplot->clearItems();
            while (customplot->plottableCount() > 0) {
                customplot->removePlottable(customplot->plottable(0));  // 删除所有plottable对象（包括柱状图等）
            }
            QVector<double> frequencies,magnitudes;
            for(int i=0;i<2000;i++){
                frequencies.append(i);
                magnitudes.append(qSin(i/(double)500*M_PI*2)+3);
            }
            if(arg1){
                // 创建柱状图
                QCPBars *bars = new QCPBars(customplot->xAxis, customplot->yAxis);
                bars->setData(frequencies, magnitudes); // xData 和 yData 为你的数据
                bars->setPen(QPen(QColor(160,160,255))); // 设置柱状图的颜色
                bars->setBrush(QBrush(QColor(160,160,255))); // 设置柱状图的填充颜色
            }else{
                // 创建连线图
                QCPGraph *graph = customplot->addGraph();
                graph->setName("实时数据");
                graph->setData(frequencies, magnitudes); // xData 和 yData 为你的数据
                graph->setPen(QPen(Qt::red)); // 设置连线图的颜色
                graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle,3));//设置数据点显示图标
            }
            // 重新绘制图表
            customplot->replot();
        });

        /******标注转频（对转速rpm除以60后，在频谱图中找到对应频率并标注）******/
        QAction *location = toolbar1->addAction(QIcon(":/image/image/Icon/location.png"),"标注转频");
        location->setCheckable(true);
        connect(location,&QAction::toggled,[=](bool arg1){
            if (!customplot->layer("LocationLayer")) {
                customplot->addLayer("LocationLayer", customplot->layer("grid"));
            }

            double location_value = livedeviceinfo.currentecho.currentspeed/60;
            if(arg1){
                QCPItemLine *line = new QCPItemLine(customplot);
                line->start->setCoords(location_value, 0);
                line->end->setCoords(location_value, customplot->yAxis->range().upper);
                line->setPen(QPen(QColor(255, 165, 0), 1, Qt::DashLine));
                line->setLayer("LocationLayer");
                localtion_lines.append(line);
                // 添加标注
                QCPItemText *label = new QCPItemText(customplot);
                label->position->setType(QCPItemPosition::ptPlotCoords);
                label->setFont(QFont("Arial", 8));
                label->setColor(Qt::black);
                label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
                label->position->setCoords(location_value, customplot->yAxis->range().upper);
                label->setText(QString::number(location_value, 'f', 1) + " Hz\n备注:rpm/60");
                label->setLayer("LocationLayer");
                localtion_labels.append(label);

            }else{
                for (auto line : localtion_lines) {
                    customplot->removeItem(line);  // 从图表中删除线
                }
                for (auto label : localtion_labels) {
                    customplot->removeItem(label);  // 从图表中删除标签
                }
                localtion_lines.clear();
                localtion_labels.clear();
            }
            customplot->replot();
        });

        /******切换显示频率 超出频率X的进行显示******/
        QAction *exceed = toolbar1->addAction(QIcon(":/image/image/Icon/exceed.png"),"指定最小值");
        exceed->setCheckable(true);
        connect(exceed,&QAction::toggled,[=](bool arg1){
            //清空界面上所有内容
            customplot->clearGraphs();
            customplot->clearItems();
            while (customplot->plottableCount() > 0) {
                customplot->removePlottable(customplot->plottable(0));  // 删除所有plottable对象（包括柱状图等）
            }

            //LMG----未完成
            //找出频谱中key的最大最小值，需要在设置图时设置最大最小值！！！！！！！！！！！！！！！！
            QVector<double> newkeys,newvalues;
            if(arg1){
                double min = customplot->xAxis->range().minRange, max = customplot->xAxis->range().maxRange;
                bool ok = false;
                double exceed_value = QInputDialog::getDouble(this, tr("输入最小频率"), tr("最小值 (Hz):"), 50.0, min, max, 2, &ok);
                if (!ok) return;
                //判断数据的key值，将高于exceed_value的key值取出放到新的QVector中---LMG 先填充个假数据
                for(int i=exceed_value;i<2000;i++){
                    newkeys.append(i);
                    newvalues.append(qSin(i/(double)500*M_PI*2)+3);
                }
            }else{
                for(int i=0;i<2000;i++){
                    newkeys.append(i);
                    newvalues.append(qSin(i/(double)500*M_PI*2)+3);
                }
            }

            customplot->addGraph();
            customplot->graph()->addData(newkeys,newvalues);
            customplot->graph()->setName("频谱");
            customplot->graph()->setPen(QPen(QColor(236,156,255)));
            customplot->graph()->setSelectable(QCP::SelectionType::stSingleData);//每次只能选择一个数据点
            customplot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle,3));//设置数据点显示图标
            customplot->xAxis->setRange(0,2000);
            customplot->replot();
        });

        /******切换显示频率 指定X轴的显示范围进行显示******/
        QAction *range_grape = toolbar1->addAction(QIcon(":/image/image/Icon/range.png"),"指定范围");
        range_grape->setCheckable(true);
        connect(range_grape,&QAction::toggled,[=](bool arg1){
            //清空界面上所有内容
            customplot->clearGraphs();
            customplot->clearItems();
            while (customplot->plottableCount() > 0) {
                customplot->removePlottable(customplot->plottable(0));  // 删除所有plottable对象（包括柱状图等）
            }

            //LMG----未完成
            //找出频谱中key的最大最小值，需要在设置图时设置最大最小值！！！！！！！！！！！！！！！！
            QVector<double> newkeys,newvalues;
            if(arg1){
                double min = customplot->xAxis->range().minRange, max = customplot->xAxis->range().maxRange;
                bool ok = false;
                double min_value = QInputDialog::getDouble(this, tr("输入最小频率"), tr("最小值 (Hz):"), 50.0, min, max, 2, &ok);
                if (!ok) return;
                double max_value = QInputDialog::getDouble(this, tr("输入最大频率"), tr("最大值 (Hz):"), 50.0, min_value, max, 2, &ok);
                if (!ok) return;
                //判断数据的key值，将高于exceed_value的key值取出放到新的QVector中---LMG 先填充个假数据
                for(int i=min_value;i<max_value;i++){
                    newkeys.append(i);
                    newvalues.append(qSin(i/(double)500*M_PI*2)+3);
                }
            }else{
                for(int i=0;i<2000;i++){
                    newkeys.append(i);
                    newvalues.append(qSin(i/(double)500*M_PI*2)+3);
                }
            }

            customplot->addGraph();
            customplot->graph()->addData(newkeys,newvalues);
            customplot->graph()->setName("频谱");
            customplot->graph()->setPen(QPen(QColor(236,156,255)));
            customplot->graph()->setSelectable(QCP::SelectionType::stSingleData);//每次只能选择一个数据点
            customplot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle,3));//设置数据点显示图标
            customplot->xAxis->setRange(0,2000);
            customplot->replot();
        });

        /*显示倍频，标注所选频率的1-10倍频*/
        QAction *frequency_doubling = toolbar1->addAction(QIcon(":/image/image/Icon/frequency_doubling.png"),"显示倍频");
        connect(frequency_doubling,&QAction::triggered,[=]{
            // 获取选中频率
            double centerFreq = 0.0;
            for (int i = 0; i < customplot->graphCount(); ++i) {
                QCPGraph *graph = customplot->graph(i);
                QCPDataSelection selection = graph->selection();

                // 确保只选中一个数据点
                if (selection.dataRangeCount() == 1) {
                    QCPDataRange dataRange = selection.dataRange(0);  // 获取选中的数据范围

                    // 只处理单个点
                    if (dataRange.size() == 1) {
                        centerFreq = graph->data()->at(dataRange.begin())->key;  // 获取选中点的x值
                    }
                }
            }
            if(centerFreq == 0) return;

            // 清除现有的标注和垂直线
            for (QCPItemLine* line : frequency_lines) {
                customplot->removeItem(line);  // 删除垂直线
            }
            for (QCPItemText* label : frequency_labels) {
                customplot->removeItem(label);  // 删除标注
            }
            frequency_lines.clear();
            frequency_labels.clear();


            //添加底层图层，否则当前图层覆盖区域将不可选中
            if (!customplot->layer("frequencyLayer")) {
                customplot->addLayer("frequencyLayer", customplot->layer("grid"));
            }


            // 左右各 numLines 条线
            for (int i = 1; i <= 11; ++i) {
                //                     if (i == 0) continue; // 跳过中心频率

                double freq = centerFreq * i;

                // 添加垂直线
                QCPItemLine *line = new QCPItemLine(customplot);
                line->start->setCoords(freq, 0);
                line->end->setCoords(freq, customplot->yAxis->range().upper);
                line->setPen(QPen(QColor(255, 165, 0), 1, Qt::DashLine));

                line->setLayer("frequencyLayer");
                frequency_lines[freq] = line;
                // 添加标注
                QCPItemText *label = new QCPItemText(customplot);
                label->position->setType(QCPItemPosition::ptPlotCoords);
                label->position->setCoords(freq, customplot->yAxis->range().upper* 0.95);
                label->setText(QString::number(freq, 'f', 1) + " Hz");
                label->setFont(QFont("Arial", 8));
                label->setColor(Qt::black);
                label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
                label->setLayer("frequencyLayer");
                frequency_labels[freq] = label;
            }
            // 刷新图表
            customplot->replot();
        });

        /*******添加特别标记******/
        QAction *fixed_add = toolbar1->addAction(QIcon(":/image/image/Icon/fixed_add.png"),"添加标注");
        connect(fixed_add,&QAction::triggered,[=]{
            //找到选中的点
            double FFT_Fixed_points = 0;
            for (int i = 0; i < customplot->graphCount(); ++i) {
                QCPGraph *graph = customplot->graph(i);
                QCPDataSelection selection = graph->selection();

                // 确保只选中一个数据点
                if (selection.dataRangeCount() == 1) {
                    QCPDataRange dataRange = selection.dataRange(0);  // 获取选中的数据范围

                    // 只处理单个点
                    if (dataRange.size() == 1) {
                        FFT_Fixed_points = graph->data()->at(dataRange.begin())->key;  // 获取选中点的x值
                    }
                }
            }
            if(FFT_Fixed_points == 0) return;
            // 创建图层（确保图层已创建）
            if (!customplot->layer("FixedLayer")) {
                customplot->addLayer("FixedLayer", customplot->layer("grid"));
            }
            // 添加垂直线
            QCPItemLine *line = new QCPItemLine(customplot);
            line->start->setCoords(FFT_Fixed_points, 0);
            line->end->setCoords(FFT_Fixed_points, customplot->yAxis->range().upper);
            line->setPen(QPen(Qt::red, 1, Qt::DashLine));
            // 将线放到最底层
            line->setLayer("FixedLayer"); // 设置图层
            verticalLines[FFT_Fixed_points] = line;// 存储垂直线引用，使用频率值作为键
            // 添加标注
            QCPItemText *label = new QCPItemText(customplot);
            label->position->setType(QCPItemPosition::ptPlotCoords);
            label->position->setCoords(FFT_Fixed_points, customplot->yAxis->range().upper);
            label->setText(QString::number(FFT_Fixed_points, 'f', 1) + " Hz");
            label->setFont(QFont("Arial", 8));
            label->setColor(Qt::black);
            label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
            // 将标注放到最底层
            label->setLayer("FixedLayer"); // 设置图层
            labels[FFT_Fixed_points] = label; // 存储标注引用，使用频率值作为键
            customplot->replot();
        });

        /******清除标记******/
        QAction *fixed_reduce = toolbar1->addAction(QIcon(":/image/image/Icon/fixed_reduce.png"),"清除标注");
        connect(fixed_reduce,&QAction::triggered,[=]{
            double FFT_Fixed_points;
            for (int i = 0; i < customplot->graphCount(); ++i) {
                QCPGraph *graph = customplot->graph(i);
                QCPDataSelection selection = graph->selection();

                // 确保只选中一个数据点
                if (selection.dataRangeCount() == 1) {
                    QCPDataRange dataRange = selection.dataRange(0);  // 获取选中的数据范围

                    // 只处理单个点
                    if (dataRange.size() == 1) {
                        FFT_Fixed_points = graph->data()->at(dataRange.begin())->key;  // 获取选中点的x值
                    }
                }
            }

            if (verticalLines.contains(FFT_Fixed_points)) {
                customplot->removeItem(verticalLines[FFT_Fixed_points]);
                verticalLines.remove(FFT_Fixed_points);  // 删除容器中的引用
            }

            // 查找并删除指定的标注
            if (labels.contains(FFT_Fixed_points)) {
                customplot->removeItem(labels[FFT_Fixed_points]);
                labels.remove(FFT_Fixed_points);  // 删除容器中的引用
            }
#if 0
            for (QCPItemLine* line : verticalLines) {
                customplot->removeItem(line);  // 删除垂直线
            }
            for (QCPItemText* label : labels) {
                customplot->removeItem(label);  // 删除标注
            }
            verticalLines.clear();
            labels.clear();
#endif
            customplot->replot();
        });

        /*清除所有线和标签*/
        QAction *broom = toolbar1->addAction(QIcon(":/image/image/Icon/broom.png"),"清理");
        connect(broom,&QAction::triggered,[=]{
            //location有可选中状态，所以需要判断是否已经选中，若选中则不处理
            for (QCPItemLine* line : verticalLines) {
                customplot->removeItem(line);  // 删除特别标记线
            }
            for (QCPItemText* label : labels) {
                customplot->removeItem(label);  // 删除特别标记标签
            }

            for (QCPItemLine* line : pinpulines) {
                customplot->removeItem(line);  // 删除等频率差值线
            }
            for (QCPItemText* label : pinpulabels) {
                customplot->removeItem(label);  // 删除等频率差值标签
            }

            for (QCPItemLine* line : frequency_lines) {
                customplot->removeItem(line);  // 删除倍频线
            }
            for (QCPItemText* label : frequency_labels) {
                customplot->removeItem(label);  // 删除倍频标签
            }

//            for (QCPItemText* label : FaultFrequency_labels) {
//                customplot->removeItem(label);  // 删除故障频率
//            }

//            for (QCPItemLine* line : FaultFrequency_lines) {
//                customplot->removeItem(line);  // 删除故障线
//            }

            verticalLines.clear();
            labels.clear();
            pinpulines.clear();
            pinpulabels.clear();
            frequency_lines.clear();
            frequency_labels.clear();

            customplot->replot();
        });

    }else if(type == 3){
        /******趋势图，增加打开按钮******/
        QAction *loading = toolbar1->addAction(QIcon(":/image/image/Icon/loading.png"), "加载");
        connect(loading,&QAction::triggered,[=]{//------LMG 未完成
            //读取X,Y的值，然后去寻找该文件，将该文件加载到波形+频谱图
            for (int i = 0; i < customplot->graphCount(); ++i) {
                QCPGraph *graph = customplot->graph(i);
                QCPDataSelection selection = graph->selection();

                // 遍历选中范围
                for (int j = 0; j < selection.dataRangeCount(); ++j) {
                    QCPDataRange dataRange = selection.dataRange(j);

                    // 遍历每个选中数据点
                    for (int k = dataRange.begin(); k < dataRange.end(); ++k) {
                        double key = graph->data()->at(k)->key;
                        //                        double value = graph->data()->at(k)->value;
                        QString str_key = QDateTime::fromTime_t(key).toString("yyyy-MM-dd HH:mm:ss");

                        //更新结构体属性
                        int dataindex = livedeviceinfo.FileKeys.indexOf(key);
                        livedeviceinfo.currentecho.currentfile = livedeviceinfo.filelist.at(dataindex);
                        //加载数据
                        LoadVIBData();
                        // 输出或保存选中的数据点信息
                        //                        qDebug() << "选中的点: key=" << str_key << ", value=" << value;
                    }
                }
            }
        });

    }

    // 将工具栏放置在右上角
    QHBoxLayout *layout1 = new QHBoxLayout(customplot);
    layout1->setAlignment(Qt::AlignTop | Qt::AlignRight);
    layout1->addWidget(toolbar1);
    customplot->setLayout(layout1);
}

void frmLivePreview::SetlabInfoName(QString Info)
{
    ui->labInfoName->setText(Info);
}

void frmLivePreview::SetlabInfoTime(QString time)
{
    ui->labInfoTime->setText(time);
}

void frmLivePreview::SetlabInfoAlarm(QString alarm)
{
    ui->labInfoAlarm->setText(alarm);
}

void frmLivePreview::UpdateTrendChart()
{
    QCustomPlot *customPlot = ui->CustomPlot_Trend;

    //清空图表
    customPlot->clearGraphs();

    //添加图表
    customPlot->addGraph();
    QCPGraph *graph = customPlot->graph(0);

    //加载数据
    graph->setData(livedeviceinfo.FileKeys,livedeviceinfo.FileValues);
    graph->setLineStyle(QCPGraph::lsLine);// lsNone 表示没有线
    //    graph->setName(livedeviceinfo.name);
    graph->setName("趋势图");
    QPen pen(QColor(236, 156, 255));
    graph->setPen(pen);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
    graph->setSelectable(QCP::SelectionType::stSingleData);

    customPlot->rescaleAxes();
    customPlot->replot();
}

void frmLivePreview::UpdateOriginalChart()
{
    QVector<double> S_Data;
    if(isdemodulated){
        S_Data = livedeviceinfo.currentecho.DemodulateData;
    }else{
        S_Data = livedeviceinfo.currentecho.RawData;
    }

    int len = S_Data.size();
    //求和
    double sum = std::accumulate(S_Data.begin(), S_Data.end(), 0.0);

    //求平均值
    double ave = sum/len;

    for(int i=0;i<len;i++){
        S_Data[i] -= ave;//求直流分量
    }

    /*************画图**************/
    QVector<double> keys;
    double fs = 20480;
    if(len == 120000){
        fs = 12000;
    }else if(len == 1000000){
        fs = 100000;
    }
    for(double i=0;i<len/fs;i+= 1/fs){
        keys.append(i);
    }
    //     for (int i=0;i<len;i++) {
    //         keys.append(len/fs*i);
    //     }

    QCustomPlot *customPlot = ui->CustomPlotOriginal;

    //清空图表
    customPlot->clearGraphs();

    //添加图表
    customPlot->addGraph();
    QCPGraph *graph = customPlot->graph(0);

    //加载数据
    graph->setData(keys,S_Data);
    graph->setLineStyle(QCPGraph::lsLine);// 暂定直线
    //    graph->setName(livedeviceinfo.name);
    graph->setName("波形图");
    //     QPen pen(QColor(236, 156, 255));
    //     graph->setPen(pen);
    //         graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    //    graph->setSelectable(QCP::SelectionType::stSingleData);
    graph->setSelectable(QCP::SelectionType::stMultipleDataRanges);
    customPlot->rescaleAxes();
    customPlot->replot();
}

void frmLivePreview::UpdateSpectrumChart(bool hanning)
{
    QVector<double> FFT_Data;
    if(isdemodulated){
       FFT_Data  = livedeviceinfo.currentecho.DemodulateData;
    }else{
        FFT_Data  = livedeviceinfo.currentecho.RawData;
    }
    int len = FFT_Data.size();
    //求和
    double sum = std::accumulate(FFT_Data.begin(), FFT_Data.end(), 0.0);

    //求平均值
    double ave = sum/len;

    for(int i=0;i<len;i++){
        FFT_Data[i] -= ave;//求直流分量
        if(hanning){
            FFT_Data[i] = FFT_Data[i] * (0.5 - 0.5 * cos(2 * M_PI * i / len));//hanning 窗
        }
    }

    //FFT
    CoreHelper::FFT(FFT_Data);

    for (int i=0;i<len;i++) {
        FFT_Data[i] = FFT_Data[i]*4/len;//傅里叶变换后 幅值会*数据长度，所以在这里除回来
    }

    /*************画图**************/
    QVector<double> keys;
    QVector<double> values;
    double fs = 20480;
    if(len == 120000){
        fs = 12000;
    }else if(len == 1000000){
        fs = 100000;
    }

    //    for(float i=0;i<len/fs;i+= 1/fs){
    //        keys.append(i);
    //    }
    for (int i=0;i<len/2;i++) {
        keys.append(fs * i/len);
        values.append(FFT_Data.at(i));
    }

    QCustomPlot *customPlot = ui->CustomPlot_Spectrum;

    //清空图表
    customPlot->clearGraphs();

    //添加图表
    customPlot->addGraph();
    QCPGraph *graph = customPlot->graph(0);

    //加载数据
    graph->setData(keys,values);
    graph->setLineStyle(QCPGraph::lsLine);// 暂定直线
    //    graph->setName(livedeviceinfo.name);
    graph->setName("频谱图");
    //     QPen pen(QColor(236, 156, 255));
    //     graph->setPen(pen);
    //     graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 5));
    graph->setSelectable(QCP::SelectionType::stSingleData);

    customPlot->rescaleAxes();
    customPlot->replot();
}

void frmLivePreview::LoadVIBData()
{
    QString filename = livedeviceinfo.currentecho.currentfile;
    if(filename.isEmpty()){
        qDebug()<<"请在趋势图中选择一个点";
        return;
    }

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        return;
    }
    QFileInfo fileinfo(filename);
    QDataStream in(&file);
    int VIBDataLen = 262144;
    //振动数据
    QByteArray vibData;
    vibData.resize(VIBDataLen);
    in.readRawData(vibData.data(), VIBDataLen);

    //解析出数据 格式：前一半是原始数据，后一半是滤波后的数据,低位在前，高位在后
    livedeviceinfo.currentecho.RawData.clear();//原始数据
    livedeviceinfo.currentecho.DemodulateData.clear();//解调数据
    float Rawsum = 0,Demsum=0,Rawave=0,Demave = 0,tempdouble = 0;
    uint16_t temp = 0;
    QVector <float> RawData;                       //原始数据
    QVector<float> DemodulatedData;                //解调数据
    for(int i=0;i<VIBDataLen/2;i+=2){
        temp = ((quint8)(vibData.at(i+1)) << 8) | (quint8)(vibData.at(i));
        tempdouble = ((((float)temp*3.3*1000)/65535)-1650)/livedeviceinfo.sensitivity;//换算出电压值(double)(double)/灵敏度
        RawData.append(tempdouble);
    }
    for(int i=VIBDataLen/2;i<VIBDataLen;i+=2){
        temp = (static_cast<quint8>(vibData.at(i+1)) << 8) | static_cast<quint8>(vibData.at(i));
        tempdouble = ((((float)temp*3.3*1000)/65535)-1650)/livedeviceinfo.sensitivity;//换算出电压值(double)(double)/灵敏度
        DemodulatedData.append(tempdouble);
    }

    //求和并找出原始数据最大值
    Rawsum = std::accumulate(RawData.begin(), RawData.end(), 0.0);
    Demsum = std::accumulate(DemodulatedData.begin(), DemodulatedData.end(), 0.0);
    qint32 datelen = RawData.size();
    //求出平均值
    Rawave = Rawsum/datelen;
    Demave = Demsum/datelen;

    //去直流分量
    for(int i=0;i<datelen;i++){
        livedeviceinfo.currentecho.RawData.append(RawData[i] - Rawave);
        DemodulatedData[i] -= Demave;
        DemodulatedData[i] = DemodulatedData[i] * 1;
        livedeviceinfo.currentecho.DemodulateData.append(DemodulatedData[i]);
    }
    livedeviceinfo.currentecho.RawData[datelen-2] = 0;
    livedeviceinfo.currentecho.RawData[datelen-1] = 0;
    livedeviceinfo.currentecho.DemodulateData[datelen-2] = 0;
    livedeviceinfo.currentecho.DemodulateData[datelen-1] = 0;

    QByteArray remainingData = file.readAll();
    QDataStream out(&remainingData,QIODevice::ReadOnly);
    //转速
    livedeviceinfo.currentecho.currentspeed = 0;
    QString label;
    out >> label;
    if(label == "speedAve:"){
        uint32_t speed;
        out >> speed;
        livedeviceinfo.currentecho.currentspeed = speed;
    }
    label.clear();
    //区间转速容器
    livedeviceinfo.currentecho.SpeedVector.clear();
    out >> label;
    if(label == "SpeedVector:"){
        int len = 0;
        out >> len;
        for(int i=0;i<len;i++){
            uint32_t value;
            out >> value;
            livedeviceinfo.currentecho.SpeedVector.append(value);
        }
    }
    label.clear();
    //诊断结果
    livedeviceinfo.currentecho.DiagnosticResults.clear();
    out >> label;
    if(label == "result:"){
        int len = 0;
        out >> len;
        for(int i=0;i<len;i++){
            QString str;
            out >> str;
            livedeviceinfo.currentecho.DiagnosticResults.append(str);
        }
    }
    //轴承型号
    label.clear();
    out >> label;
    if(label == "bearingInfo:"){
        QString str;
        out >> str;
        livedeviceinfo.currentecho.BearingType = str;
    }

    file.close();

    ui->widget_bottom->setEnabled(true);
    //需要更新其余视图和信息栏
    UpdateInformationBar();
    UpdateOriginalChart();
    UpdateSpectrumChart();
    UpdateDimensionChart();
    if(m_SpeedChartWidget){
        m_SpeedChartWidget->UpdateData(livedeviceinfo.currentecho.SpeedVector);
    }
    if(m_DBChartWidget){
        m_DBChartWidget->UpdateData(livedeviceinfo.currentecho.DiagnosticResults,livedeviceinfo.currentecho.BearingType);
    }
    if(m_faultFrequency){
        m_faultFrequency->UpdateInfo(livedeviceinfo.currentecho.currentspeed,livedeviceinfo.currentecho.RawData);
    }

}

void frmLivePreview::UpdateInformationBar()
{
    //提取时间信息
    //绝对地址转为相对地址
    QFileInfo fileInfo(livedeviceinfo.currentecho.currentfile);
    QString filename = fileInfo.baseName();
    QStringList filelist = filename.split("_");
    //    QString str_key = QDateTime::fromTime_t(key).toString("yyyy-MM-dd HH:mm:ss");
    QDateTime time = QDateTime::fromString(filelist.last(),"yyyyMMddHHmmss");
    ui->labInfoTime->setText("时间:" + time.toString("yyyy-MM-dd HH:mm:ss"));

    uint32_t speed = livedeviceinfo.currentecho.currentspeed;
    ui->labInfoName->setText(QString("转速:%1").arg(speed));
    //分析报警状态
    QString alarmstr;
    QStringList names;
    QStringList alarmlist;
    names << "保外" << "保内" << "外环" << "内环" << "滚单" << "大齿轮" << "小齿轮" << "踏面";
    QStringList list = livedeviceinfo.currentecho.DiagnosticResults.at(0).split("|");
    for(int i=0;i<list.size();i++){
        uint8_t state = list.at(i).toInt();
        if(state != 0xFF){
            QString grade;
            switch (state) {
            case 0:
                grade = "预警";
                break;
            case 1:
                grade = "一级报警";
                break;
            case 2:
                grade = "二级报警";
                break;
            }
            if(!grade.isEmpty()){
                alarmlist.append(names.at(i) + grade);
            }
        }
    }
    QString stylestr = "color: red;";
    if(alarmlist.isEmpty()){
        alarmstr = "无报警信息";
        stylestr = "color: green;";
    }else{
        alarmstr = alarmlist.join(" | ");
    }
    ui->labInfoAlarm->setText(alarmstr);
    ui->labInfoAlarm->setStyleSheet(stylestr);
}

void frmLivePreview::UpdateDimensionChart()
{
    QVector<double> O_Data = livedeviceinfo.currentecho.RawData;
    QVector<double> D_Data = livedeviceinfo.currentecho.DemodulateData;
    livedeviceinfo.currentecho.O_Dimension.clear();
    livedeviceinfo.currentecho.D_Dimension.clear();
    //    int len = D_Data.size();
    //    //求和
    //    double sum = std::accumulate(O_Data.begin(), O_Data.end(), 0.0);
    //    double sum1 = std::accumulate(D_Data.begin(), D_Data.end(), 0.0);
    //    //求平均值
    //    double ave = sum/len;
    //    double ave1 = sum1/len;

    //    for(int i=0;i<len;i++){
    //        O_Data[i] -= ave;//求直流分量
    //        D_Data[i] -= ave1;
    //    }
    livedeviceinfo.currentecho.O_Dimension = CoreHelper::CalculateDimension(O_Data);
    livedeviceinfo.currentecho.D_Dimension = CoreHelper::CalculateDimension(D_Data);
    for(int i=0;i<txt_ODimension.size();i++){
        txt_ODimension[i]->setText(QString::number(livedeviceinfo.currentecho.O_Dimension.at(i)));
    }
    for(int i=0;i<txt_DDimension.size();i++){
        txt_DDimension[i]->setText(QString::number(livedeviceinfo.currentecho.D_Dimension.at(i)));
    }
}

void frmLivePreview::DataImport()
{
    DataFileDir = App::DownLoadTempDataDir;
    QDir dir(App::DownLoadTempDataDir);
    // 查找符合条件的文件
    QStringList filters;
    filters << "*.bin"; // 查找所有 .bin 文件
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files);
    QFileInfoList files = dir.entryInfoList();

    QDateTime starttime = QDateTime::fromString(App::LiveDataStartTime,"yyyy-MM-dd HH:mm:ss");
    QDateTime endtime = QDateTime::fromString(App::LiveDataEndTime,"yyyy-MM-dd HH:mm:ss");
    // 设置随机数生成器
    QRandomGenerator generator;
    //将各自的数据填充至各自的结构体中
    foreach (const QFileInfo &fileInfo, files) {
        //格式 ：数据类型_车厢号_前置ID_通道号_时间.bin 例如 vib_2_1_2_20240101133000254.bin
        QString filename = fileInfo.baseName();
        QStringList list = filename.split("_");
        QString prefix = list.at(0);
        QString filewagon = list.at(1);
        int fileID = list.at(2).toInt();
        int fileCH = list.at(3).toInt();
        QDateTime time = QDateTime::fromString(list.at(4),"yyyyMMddHHmmss");
        if((filewagon == livedeviceinfo.wagon) && (fileID == livedeviceinfo.id) && (fileCH == livedeviceinfo.ch) &&
                (time < endtime) && (time > starttime)){
            livedeviceinfo.filelist.append(fileInfo.absoluteFilePath());
            //查找数据库中的峰峰值，如果存在的话，如果没有数据库连接或者没数据则随机生成一个值
            double value = generator.bounded(5.00);  // 默认值
            if (M_DbAPI::Instance()->GetDataConnectState()) {
                QString str = QString("SELECT peak_to_peak FROM DimensionalValue WHERE TriggerTime = :triggertime "
                                      "AND DeviceID = :deviceID "
                                      "AND DeviceCH = :deviceCH "
                                      "AND WagonNumber = :wagonNumber");

                QSqlQuery query(M_DbAPI::Instance()->GetDataDBHandle());
                query.prepare(str);
                query.bindValue(":triggertime", time.toString("yyyy-MM-dd HH:mm:ss"));
                query.bindValue(":deviceID", fileID);
                query.bindValue(":deviceCH", fileCH);
                query.bindValue(":wagonNumber", filewagon);

                if (query.exec() && query.next()) {
                    value = query.value(0).toDouble();  // 仅在查询成功且有数据时更新值
                }
            }
            livedeviceinfo.FileValues.append(value);
            QDateTime epochDateTime = QDateTime::fromString("2024-01-01 00:00:00", "yyyy-MM-dd HH:mm:ss");
            qint64 key = time.toSecsSinceEpoch();
            livedeviceinfo.FileKeys.append(key);
        }
    }

    //需要更新趋势图
    UpdateTrendChart();
}

void frmLivePreview::TrendpopupToolTip(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    if (auto *graph = qobject_cast<QCPGraph *>(plottable)) {
        double key = graph->data()->at(dataIndex)->key;
        double value = graph->data()->at(dataIndex)->value;
        QString str_at= graph->name();

        QStringList textlist;
        textlist.append("类型: " + str_at);
        textlist.append("时间(s): " + QString::number(key));
        textlist.append("幅值：" + QString::number(value));
        if(str_at == "趋势图"){
            QString str_key = QDateTime::fromTime_t(key).toString("yyyy-MM-dd HH:mm:ss");
            textlist[1] = "时间: " + str_key;
        }else if(str_at == "频谱图"){
            textlist[1] = "频率(Hz): " + QString::number(key);
        }
        if (event->button() == Qt::LeftButton && event->type() != QEvent::MouseButtonDblClick){
            QToolTip::showText(QCursor::pos(), textlist.join("\n"));
        }
    }
}

void frmLivePreview::TrendLoadFile(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (auto *graph = qobject_cast<QCPGraph *>(plottable)) {
            double key = graph->data()->at(dataIndex)->key;
            int dataindex = livedeviceinfo.FileKeys.indexOf(key);
            livedeviceinfo.currentecho.currentfile = livedeviceinfo.filelist.at(dataindex);
            //加载数据
            LoadVIBData();
        }
    }
}

#if 0
void frmLivePreview::on_CustomPlot_Trend_selectionChangedByUser()
{
    QCustomPlot* customPlot=ui->CustomPlot_Trend;

    for(int i=0;i<customPlot->graphCount();i++)
    {
        //遍历有被选中的graph
        if(customPlot->graph(i)->selected())
        {
            QString layerName = customPlot->graph(i)->layer()->name();
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
                    //                    qDebug()<< dataText;
                    QToolTip::showText(QCursor::pos(), dataText);
                }
            }
        }
    }
}
#endif
void frmLivePreview::on_btn_GetData_clicked()
{
    if(ui->EndDateTime->date() < ui->StartDateTime->date()){
        QMessageBox::warning(this,"错误","结束时间不能小于开始时间");
        return;
    }
    QString starttime = ui->StartDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString endtime = ui->EndDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss");

    QStringList list;
    list.append(livedeviceinfo.wagon);
    list.append(QString::number(livedeviceinfo.id));
    list.append(QString::number(livedeviceinfo.ch));
    list.append(starttime);
    list.append(endtime);
    qDebug()<<"获取字符串为： \n" << list.join(";");

    M_TcpAPI::Instance()->GetDataFile(list.join(";"));
}
#if 0
void frmLivePreview::on_CustomPlotOriginal_selectionChangedByUser()
{
    QCustomPlot* customPlot=ui->CustomPlotOriginal;

    for(int i=0;i<customPlot->graphCount();i++)
    {
        //遍历有被选中的graph
        if(customPlot->graph(i)->selected())
        {
            QString layerName = customPlot->graph(i)->layer()->name();
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
                    QString dataText = QString("类型: %1\ntimes(s): %2\n幅值: %3").arg(str_at).arg(double_key).arg(str_value);
                    QToolTip::showText(QCursor::pos(), dataText);
                }
            }
        }
    }
}

void frmLivePreview::on_CustomPlot_Spectrum_selectionChangedByUser()
{
    QCustomPlot* customPlot=ui->CustomPlot_Spectrum;

    for(int i=0;i<customPlot->graphCount();i++)
    {
        //遍历有被选中的graph
        if(customPlot->graph(i)->selected())
        {
            QString layerName = customPlot->graph(i)->layer()->name();
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
                    QString dataText = QString("类型: %1\n频率: %2\n幅值: %3").arg(str_at).arg(double_key).arg(str_value);
                    QToolTip::showText(QCursor::pos(), dataText);
                }
            }
        }
    }
}
#endif
void frmLivePreview::on_btn_echoDimension_clicked()
{
    QString text = ui->btn_echoDimension->text();
    if(text == "显示量纲"){
        ui->btn_echoDimension->setText("关闭量纲");
        ui->widget_Dimension->setVisible(true);
    }else if(text == "关闭量纲"){
        ui->btn_echoDimension->setText("显示量纲");
        ui->widget_Dimension->setVisible(false);
    }
}

void frmLivePreview::on_btn_SpeedVector_clicked(bool checked)
{
    if(checked && !m_SpeedChartWidget){
        //构建一个视图,显示转速
        m_SpeedChartWidget = new frmSpeedLineChart(livedeviceinfo.currentecho.SpeedVector,this);
        m_SpeedChartWidget->show();
        connect(m_SpeedChartWidget,&frmSpeedLineChart::closedSignal,this,[this](){
            ui->btn_SpeedVector->setChecked(false);
            m_SpeedChartWidget = nullptr;
        });
    }else if(!checked && m_SpeedChartWidget){
        m_SpeedChartWidget->close();
        m_SpeedChartWidget = nullptr;
    }
}

void frmLivePreview::on_btn_closeTrend_clicked(bool checked)
{
    ui->CustomPlot_Trend->setVisible(!checked);
    if(checked){
        ui->btn_closeTrend->setText("显示趋势");
    }else{
        ui->btn_closeTrend->setText("关闭趋势");
    }
}

void frmLivePreview::on_btn_echoDBValue_clicked(bool checked)
{
    if(checked && !m_DBChartWidget){
        //构建一个视图,显示包络
        m_DBChartWidget = new frmDBChart(livedeviceinfo.currentecho.DiagnosticResults,livedeviceinfo.currentecho.BearingType,this);
        m_DBChartWidget->show();
        connect(m_DBChartWidget,&frmDBChart::closedSignal,this,[this](){
            ui->btn_echoDBValue->setChecked(false);
            m_DBChartWidget = nullptr;
        });
    }else if(!checked && m_DBChartWidget){
        m_DBChartWidget->close();
        m_DBChartWidget = nullptr;
    }
}

void frmLivePreview::on_btn_QueryData_clicked()
{
    //检查时间是否合法
    if(ui->EndDateTime->date() < ui->StartDateTime->date()){
        QMessageBox::warning(this,"错误","结束时间不能小于开始时间");
        return;
    }
    QString starttime = ui->StartDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString endtime = ui->EndDateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    App::LiveDataStartTime = starttime;
    App::LiveDataEndTime = endtime;
    App::writeConfig();

    DataImport();
}


void frmLivePreview::on_btn_addPK_clicked()
{
    if(PreData::pkdatalist.size() >= PreData::PKListSizeMax){
        QMessageBox::warning(this,"添加对比","添加失败,队列已满");
        return;
    }
    PreData::PKDATASTRUCT pkDATA;
    pkDATA.id = livedeviceinfo.id;
    pkDATA.ch = livedeviceinfo.ch;
    pkDATA.Wagon = livedeviceinfo.wagon;
    pkDATA.name = livedeviceinfo.name;
    if(isdemodulated){
        pkDATA.values = livedeviceinfo.currentecho.DemodulateData;
    }else{
        pkDATA.values = livedeviceinfo.currentecho.RawData;
    }
    //从时间显示框内提取出时间
    QStringList list = ui->labInfoTime->text().split("时间:");
    if(list.size() < 2){
        //只能去找时间文件了
        QFileInfo fileInfo(livedeviceinfo.currentecho.currentfile);
        QString filename = fileInfo.baseName();
        QStringList filelist = filename.split("_");
        QDateTime time = QDateTime::fromString(filelist.last(),"yyyyMMddHHmmss");
        pkDATA.time = time.toString("yyyy-MM-dd HH:mm:ss");
    }else{
        pkDATA.time = list.last();
    }

    if(!PreData::pkdatalist.contains(pkDATA)){
        PreData::pkdatalist.append(pkDATA);
        CoreHelper::showFadingMessage(this,"添加成功");
    }else{
        QMessageBox::warning(this,"添加对比","添加失败,信息已存在");
    }

}

void frmLivePreview::on_btn_faultFrequency_clicked(bool checked)
{
    if(checked && !m_faultFrequency){
        //构建一个视图,显示包络
        m_faultFrequency = new FrmFaultFrequencyChart(this);
        m_faultFrequency->SetDataInfo(livedeviceinfo.wagon,livedeviceinfo.id,livedeviceinfo.ch,livedeviceinfo.currentecho.currentspeed,
                                      livedeviceinfo.currentecho.BearingType,livedeviceinfo.currentecho.RawData);
        m_faultFrequency->show();
        connect(m_faultFrequency,&FrmFaultFrequencyChart::closedSignal,this,[this](){
            ui->btn_faultFrequency->setChecked(false);
//            m_faultFrequency = nullptr;
            m_faultFrequency->hide();
        });
        connect(m_faultFrequency,&FrmFaultFrequencyChart::UserChecked,this,&frmLivePreview::SetMarkState);
    }else if(!checked && m_faultFrequency){
//        disconnect(m_faultFrequency,&FrmFaultFrequencyChart::UserChecked,this,&frmLivePreview::SetMarkState);
//        m_faultFrequency->close();
//        m_faultFrequency = nullptr
        m_faultFrequency->hide();
    }else if(checked && m_faultFrequency){
        m_faultFrequency->show();
    }
}

void frmLivePreview::SetMarkState(QString &Describe, double &value, bool state)
{
//    qDebug()<<"key == " << Describe << " value = " << value << "checkstate = " << state;
    if(value < 0) return;
    QCustomPlot *customplot = ui->CustomPlot_Spectrum;
    if(!state){
        if(FaultFrequency_lines.contains(value)){
//            delete FaultFrequency_lines[value];
            customplot->removeItem(FaultFrequency_lines[value]);
            FaultFrequency_lines.remove(value);
        }

        if(FaultFrequency_labels.contains(value)){
//            delete FaultFrequency_labels[value];
            customplot->removeItem(FaultFrequency_labels[value]);
            FaultFrequency_labels.remove(value);
        }
    }else{
        // 创建图层（确保图层已创建）
        if (!customplot->layer("FaultFrequency")) {
            customplot->addLayer("FaultFrequency", customplot->layer("grid"));
        }


        if(FaultFrequency_lines.contains(value) || FaultFrequency_labels.contains(value)) return;
        // 添加垂直线
        QCPItemLine *line = new QCPItemLine(customplot);
        line->start->setCoords(value, 0);
        line->end->setCoords(value, customplot->yAxis->range().upper);
        line->setPen(QPen(QColor("#800080"), 1, Qt::DashLine));
        // 将线放到最底层
        line->setLayer("FaultFrequency"); // 设置图层

        FaultFrequency_lines[value] = line;// 存储垂直线引用，使用频率值作为键
        // 添加标注
        QCPItemText *label = new QCPItemText(customplot);
        label->position->setType(QCPItemPosition::ptPlotCoords);
        label->position->setCoords(value, customplot->yAxis->range().upper);
        label->setText(QString("%1\n%2Hz").arg(Describe).arg(QString::number(value, 'f', 1)));
        label->setFont(QFont("Arial", 8));
        label->setColor(Qt::black);
        label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
        // 将标注放到最底层
        label->setLayer("FaultFrequency"); // 设置图层
        FaultFrequency_labels[value] = label; // 存储标注引用，使用频率值作为键
    }
    customplot->replot();
}
