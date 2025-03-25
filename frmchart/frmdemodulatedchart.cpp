#include "frmdemodulatedchart.h"
#include "ui_frmdemodulatedchart.h"
#include "corehelper.h"
#include "toggleswitch.h"

frmDemodulatedChart::frmDemodulatedChart(QWidget *parent) :QDialog(parent),
    ui(new Ui::frmDemodulatedChart)
{
    ui->setupUi(this);
    setWindowTitle("波形对比图");
    InitFrom();
    InitData();

}

frmDemodulatedChart::~frmDemodulatedChart()
{
    delete ui;
}

void frmDemodulatedChart::InitFrom()
{
    Linecolors.append(QColor(31, 119, 180));    // 蓝色
    Linecolors.append(QColor(255, 127, 14));    // 橙色
    Linecolors.append(QColor(44, 160, 44));     // 绿色
    Linecolors.append(QColor(214, 39, 40));     // 红色
    Linecolors.append(QColor(148, 103, 189));   // 紫色
    Linecolors.append(QColor(140, 86, 75));     // 棕色
    Linecolors.append(QColor(227, 119, 194));   // 粉色
    Linecolors.append(QColor(127, 127, 127));   // 灰色
    Linecolors.append(QColor(188, 189, 34));    // 黄绿色
    Linecolors.append(QColor(23, 190, 207));    // 青色

    ui->customplot_O->yAxis->setLabel("波 形 图");
    ui->customplot_O->xAxis->setLabel("time (s)");
    ui->customplot_O->yAxis->setLabelPadding(10);
    ui->customplot_O->yAxis->setLabelFont(QFont("Microsoft YaHei",12));
    ui->customplot_O->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables| QCP::iMultiSelect);
    ui->customplot_O->setInteraction(QCP::iRangeDrag,true);//使能拖动  失能直接该true为false

//        QFont font("Microsoft YaHei",12);
//        //设置提示框字体
//        ui->customplot_O->legend->setFont(font);
//        //显示提示框
//        ui->customplot_O->legend->setVisible(true);
//        //设置画刷
//        ui->customplot_O->legend->setBrush(QBrush(QColor(255, 255, 255, 125)));
//        //将提示框放在左上角
//        ui->customplot_O->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignLeft);

    ui->customplot_FFT->yAxis->setLabel("频 谱 图");
    ui->customplot_FFT->xAxis->setLabel("Frequency (Hz)");
    ui->customplot_FFT->yAxis->setLabelPadding(10);
    ui->customplot_FFT->yAxis->setLabelFont(QFont("Microsoft YaHei",12));
    ui->customplot_FFT->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables);//| QCP::iMultiSelect
    ui->customplot_FFT->setInteraction(QCP::iRangeDrag,true);//使能拖动  失能直接该true为false
//        //设置提示框字体
//        ui->customplot_FFT->legend->setFont(font);
//        //显示提示框
//        ui->customplot_FFT->legend->setVisible(true);
//        //设置画刷
//        ui->customplot_FFT->legend->setBrush(QBrush(QColor(255, 255, 255, 125)));
//        //将提示框放在左上角
//        ui->customplot_FFT->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignLeft);


    ui->widget_2->hide();
    ui->widget_3->hide();
    AddToolbar(ui->customplot_O, 1);
    AddToolbar(ui->customplot_FFT, 2);
    connect(ui->customplot_O,&QCustomPlot::plottableClick,this,&frmDemodulatedChart::TrendpopupToolTip);
    connect(ui->customplot_FFT,&QCustomPlot::plottableClick,this,&frmDemodulatedChart::TrendpopupToolTip);
    connect(ui->btn_refresh,&QPushButton::clicked,this,&frmDemodulatedChart::RefreshView);
    UpdateOriginalChart();
    UpdateSpectrumChart();
}

void frmDemodulatedChart::InitData()
{
    int leftRowCount = 0;
    int rightRowCount = 0;
    int len = PreData::pkdatalist.size();
    if(len == 0){
        return;
    }else if(len > 1){
        ui->widget_2->show();
        ui->widget_3->show();
    }else{
        ui->widget_2->show();
    }
    if(len > 10) len = 10;
    for(int i=0; i<len; i++){
        auto Data = PreData::pkdatalist.at(i);
        QLabel *lab_color = new QLabel(this);
        ToggleSwitch *toggle = new ToggleSwitch(this);
        QLabel *lab_wagon = new QLabel(this);
        QLabel *lab_ID = new QLabel(this);
        QLabel *lab_CH = new QLabel(this);
        QLabel *lab_Time = new QLabel(this);
        QLabel *lab_Name = new QLabel(this);
        QPushButton *removeBtn = new QPushButton("Delete", this);
        removeBtn->setProperty("wagon", Data.Wagon);
        removeBtn->setProperty("id", Data.id);
        removeBtn->setProperty("ch", Data.ch);
        removeBtn->setProperty("time", Data.time);
        removeBtn->setProperty("name",Data.name);
        connect(removeBtn,&QPushButton::clicked,this,&frmDemodulatedChart::DeleteButton_clicked);

        toggle->setProperty("wagon", Data.Wagon);
        toggle->setProperty("id", Data.id);
        toggle->setProperty("ch", Data.ch);
        toggle->setProperty("time", Data.time);
        toggle->setProperty("name",Data.name);
        toggle->setProperty("index",i);
        toggle->setChecked(true);
        connect(toggle, &ToggleSwitch::toggled, this, &frmDemodulatedChart::SwitchButton_toggled);

        lab_color->setStyleSheet(QString("background-color:%1").arg(Linecolors.at(i).name()));
        lab_wagon->setText(Data.Wagon);
        lab_ID->setText(QString::number(Data.id));
        lab_CH->setText(QString::number(Data.ch));
        lab_Name->setText(Data.name);
        lab_Time->setText(Data.time);

        lab_color->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lab_wagon->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lab_ID->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lab_CH->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lab_Name->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lab_Time->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        if(i%2 == 0){//偶数填充到左边
            //            int row = (i/2) + 1;
            int row = leftRowCount + 1;
            ui->gridLayout_left->addWidget(lab_color,row,0);
            ui->gridLayout_left->addWidget(lab_wagon,row,1);
            ui->gridLayout_left->addWidget(lab_ID,row,2);
            ui->gridLayout_left->addWidget(lab_CH,row,3);
            ui->gridLayout_left->addWidget(lab_Name,row,4);
            ui->gridLayout_left->addWidget(lab_Time,row,5);
            ui->gridLayout_left->addWidget(toggle,row,6);
            ui->gridLayout_left->addWidget(removeBtn,row,7);
            leftRowCount++;
        }else{//奇数填充到右边
            //            int row = (i/2) + (i%2) + 1;
            int row = rightRowCount + 1;
            ui->gridLayout_right->addWidget(lab_color,row,0);
            ui->gridLayout_right->addWidget(lab_wagon,row,1);
            ui->gridLayout_right->addWidget(lab_ID,row,2);
            ui->gridLayout_right->addWidget(lab_CH,row,3);
            ui->gridLayout_right->addWidget(lab_Name,row,4);
            ui->gridLayout_right->addWidget(lab_Time,row,5);
            ui->gridLayout_right->addWidget(toggle,row,6);
            ui->gridLayout_right->addWidget(removeBtn,row,7);
            rightRowCount++;
        }
    }
    // **补齐较短的一侧**
    int maxRows = qMax(leftRowCount, rightRowCount);
    if(leftRowCount < maxRows) {
        ui->gridLayout_left->addItem(new QSpacerItem(1, 20, QSizePolicy::Minimum, QSizePolicy::Expanding), maxRows, 0);
    } else if(rightRowCount < maxRows) {
        ui->gridLayout_right->addItem(new QSpacerItem(1, 20, QSizePolicy::Minimum, QSizePolicy::Expanding), maxRows, 0);
    }
}

void frmDemodulatedChart::AddToolbar(QCustomPlot *customplot, int type)
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
        QList<QCustomPlot*> viewlist;
        viewlist.append(ui->customplot_O);
        viewlist.append(ui->customplot_FFT);
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

            double location_value = 50;
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
                label->position->setCoords(freq, customplot->yAxis->range().upper);
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

            verticalLines.clear();
            labels.clear();
            pinpulines.clear();
            pinpulabels.clear();
            frequency_lines.clear();
            frequency_labels.clear();

            customplot->replot();
        });

    }
    // 将工具栏放置在右上角
    QHBoxLayout *layout1 = new QHBoxLayout(customplot);
    layout1->setAlignment(Qt::AlignTop | Qt::AlignRight);
    layout1->addWidget(toolbar1);
    customplot->setLayout(layout1);
}

void frmDemodulatedChart::UpdateOriginalChart()
{
    if(PreData::pkdatalist.isEmpty()) return;
    QCustomPlot *customPlot = ui->customplot_O;
    //清空图表
    customPlot->clearGraphs();

    for(int i=0; i<PreData::pkdatalist.size();i++){
        auto pkdata = PreData::pkdatalist.at(i);
        QVector<double> S_Data = pkdata.values;
        if(S_Data.isEmpty()){
            continue;
        }
        int len = S_Data.size();

        //求和
        double sum = std::accumulate(S_Data.begin(), S_Data.end(), 0.0);

        //求平均值
        double ave = sum/len;

        for(int i=0;i<len;i++){
            S_Data[i] -= ave;//求直流分量
        }

        //求Key
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

        //生成曲线名称
        QString graphname = QString("%1-%2-%3").arg(pkdata.Wagon).arg(pkdata.name).arg(pkdata.time);
        //添加图表
        customPlot->addGraph();
        QCPGraph *graph = customPlot->graph(i);
        graph->setName(graphname);
        graph->setPen(Linecolors.at(i));
        graph->setLineStyle(QCPGraph::LineStyle::lsLine);
        graph->rescaleAxes(true);
        graph->setSelectable(QCP::stSingleData);
        graph->setData(keys,S_Data);
    }
    customPlot->rescaleAxes();
    customPlot->replot();
}

void frmDemodulatedChart::UpdateSpectrumChart(bool hanning)
{

    if(PreData::pkdatalist.isEmpty()) return;
    QCustomPlot *customPlot = ui->customplot_FFT;
    //清空图表
    customPlot->clearGraphs();

    for(int i=0; i<PreData::pkdatalist.size();i++){
        auto pkdata = PreData::pkdatalist.at(i);
        QVector<double> FFT_Data = pkdata.values;
        if(FFT_Data.isEmpty()){
            continue;
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
        QVector<double> keys,values;
        double fs = 20480;
        if(len == 120000){
            fs = 12000;
        }else if(len == 1000000){
            fs = 100000;
        }
        for (int i=0;i<len/2;i++) {
            keys.append(fs * i/len);
            values.append(FFT_Data.at(i));
        }

        //生成曲线名称
        QString graphname = QString("%1-%2-%3").arg(pkdata.Wagon).arg(pkdata.name).arg(pkdata.time);
        //添加图表
        customPlot->addGraph();
        QCPGraph *graph = customPlot->graph(i);
        graph->setName(graphname);
        graph->setPen(Linecolors.at(i));
        graph->setLineStyle(QCPGraph::LineStyle::lsLine);
        graph->rescaleAxes(true);
        graph->setSelectable(QCP::stSingleData);
        graph->setData(keys,values);
    }

    customPlot->rescaleAxes();
    customPlot->replot();
}

void frmDemodulatedChart::RefreshView()
{
//    frmBlockingInterface::Instance()->SetLabelText("重绘中,请勿操作");
//    frmBlockingInterface::Instance()->StartShow();

    //移除layout中的所有设备
    clearLayoutFromRow(ui->gridLayout_left,1);
    clearLayoutFromRow(ui->gridLayout_right,1);

    //隐藏设备界面
    ui->widget_2->hide();
    ui->widget_3->hide();
    //清空绘图的所有内容
    CustomPlot_clear(ui->customplot_O);
    CustomPlot_clear(ui->customplot_FFT);

    //重新生成设备信息
    InitData();
    //重新绘图
    UpdateOriginalChart();
    UpdateSpectrumChart();

//    frmBlockingInterface::Instance()->Stop();

}

void frmDemodulatedChart::clearLayoutFromRow(QGridLayout *layout, int startRow) {
    if (!layout) return;

    // 逆序遍历，防止索引错乱
    for (int i = layout->count() - 1; i >= 0; --i) {
        QLayoutItem *item = layout->itemAt(i);
        if (!item) continue;

        int row, column, rowSpan, colSpan;
        layout->getItemPosition(i, &row, &column, &rowSpan, &colSpan);

        // 只移除 startRow 及之后的行
        if (row >= startRow) {
            if (QWidget *widget = item->widget()) {
                layout->removeWidget(widget);
                widget->deleteLater();  // 防止内存泄漏
            } else if (QSpacerItem *spacer = item->spacerItem()) {
                // 删除 QSpacerItem
                layout->removeItem(spacer);
                delete spacer;
            } else {
                // 其他类型的 QLayoutItem
                delete item;
            }
        }
    }
}


void frmDemodulatedChart::CustomPlot_clear(QCustomPlot *customPlot)
{
    int count = customPlot->graphCount();
    for(int i=0;i<count;i++){
        customPlot->graph(i)->clearData();
    }
    customPlot->replot();
}

void frmDemodulatedChart::closeEvent(QCloseEvent *event)
{
    Q_EMIT closesignals();
    QDialog::closeEvent(event);
}

void frmDemodulatedChart::TrendpopupToolTip(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    if (auto *graph = qobject_cast<QCPGraph *>(plottable)) {
        double key = graph->data()->at(dataIndex)->key;
        double value = graph->data()->at(dataIndex)->value;
        QString str_at= graph->name();
        QStringList textlist;
        textlist.append("类型: " + str_at);
        textlist.append("时间(s): " + QString::number(key));
        textlist.append("幅值：" + QString::number(value));

        if(str_at == "频谱图"){
            textlist[1] = "频率(Hz): " + QString::number(key);
        }
        if (event->button() == Qt::LeftButton && event->type() != QEvent::MouseButtonDblClick){
            QToolTip::showText(QCursor::pos(), textlist.join("\n"));
        }
    }
}

void frmDemodulatedChart::SwitchButton_toggled(bool checked)
{
    ToggleSwitch *toggle = qobject_cast<ToggleSwitch*>(sender());
    if (!toggle) return;

    // 获取 ToggleSwitch 绑定的属性数据
    QString wagon = toggle->property("wagon").toString();
    int id = toggle->property("id").toInt();
    int ch = toggle->property("ch").toInt();
    QString time = toggle->property("time").toString();
    QString name = toggle->property("name").toString();
    int index = toggle->property("index").toInt();

    //添加至不显示队列，然后重新绘图
    PreData::PKDATASTRUCT hideview;
    hideview.id = id;
    hideview.ch = ch;
    hideview.time = time;
    hideview.Wagon = wagon;
    hideview.name = name;
    hideview.values = QVector<double>();
    if(checked && HideList.contains(hideview)){
        HideList.removeOne(hideview);
        //根据车厢名称-id-ch 移除图像
        qDebug()<<"移除 id ==" << id << "ch == " << ch;
        ui->customplot_O->graph(index)->setVisible(true);
        ui->customplot_FFT->graph(index)->setVisible(true);
    }else{
        HideList.append(hideview);
        ui->customplot_O->graph(index)->setVisible(false);
        ui->customplot_FFT->graph(index)->setVisible(false);
        qDebug()<<"添加 id ==" << id << "ch == " << ch;
    }
    ui->customplot_O->replot();
    ui->customplot_FFT->replot();
}

void frmDemodulatedChart::DeleteButton_clicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QString wagon = btn->property("wagon").toString();
    int id = btn->property("id").toInt();
    int ch = btn->property("ch").toInt();
    QString time = btn->property("time").toString();
    QString name = btn->property("name").toString();
    if(QMessageBox::question(this,"移出对比","从对比队列中删除?") == QMessageBox::Yes){
        PreData::PKDATASTRUCT hideview;
        hideview.id = id;
        hideview.ch = ch;
        hideview.time = time;
        hideview.Wagon = wagon;
        hideview.name = name;
        hideview.values = QVector<double>();
        if(PreData::pkdatalist.contains(hideview)){
            PreData::pkdatalist.removeOne(hideview);
            qDebug()<<"移除成功 id = " << id << "ch = " << ch;
            //重新画图
            // 找到按钮所在的行号
            QGridLayout *layout = qobject_cast<QGridLayout*>(btn->parentWidget()->layout());
            if (!layout) return;

            int rowToRemove = -1;
            for (int row = 0; row < layout->rowCount(); ++row) {
                QLayoutItem *item = layout->itemAtPosition(row, 7); // 获取该行第七列的 item
                if (item && item->widget() == btn) {
                    rowToRemove = row;
                    break;
                }
            }

            if (rowToRemove == -1) return; // 未找到该按钮所在的行

            // 删除该行的所有控件
            for (int col = 0; col < layout->columnCount(); ++col) {
                QLayoutItem *item = layout->itemAtPosition(rowToRemove, col);
                if (item) {
                    QWidget *widget = item->widget();
                    if (widget) {
                        layout->removeWidget(widget);
                        widget->deleteLater(); // 释放控件
                    } else {
                        layout->removeItem(item);
                        delete item;
                    }
                }
            }

            // 检查是否只剩第 0 行（即所有数据行都被删除）
            bool isEmpty = true;
            for (int row = 1; row < layout->rowCount(); ++row) {
                for (int col = 0; col < layout->columnCount(); ++col) {
                    if (layout->itemAtPosition(row, col)) {
                        isEmpty = false;
                        break;
                    }
                }
                if (!isEmpty) break;
            }

            // 如果 layout 只剩第 0 行，隐藏父窗口
            if (isEmpty) {
                QWidget *parentWidget = layout->parentWidget();
                if (parentWidget) {
                    parentWidget->hide();
                }
            }
        }
    }
}



void frmDemodulatedChart::on_btn_clearpklist_clicked()
{
    if(QMessageBox::question(nullptr,"确认","清除所有对比数据？") == QMessageBox::Yes){
        CoreHelper::showFadingMessage(this,"已清空");
        PreData::pkdatalist.clear();
        RefreshView();
    }
}
