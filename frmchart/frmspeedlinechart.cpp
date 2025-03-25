#include "frmspeedlinechart.h"
#include "ui_frmspeedlinechart.h"

frmSpeedLineChart::frmSpeedLineChart(const QVector<uint32_t> &Data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmSpeedLineChart),
    SpeedData(Data)
{
    ui->setupUi(this);
    setWindowTitle("转速区间 图示");
//    setFixedSize(600,300);
    this->resize(600,300);
    ui->customplot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables| QCP::iMultiSelect);
    ui->customplot->setInteraction(QCP::iRangeDrag,true);
    ui->customplot->yAxis->setLabel("转速(RPM)");
    ui->customplot->xAxis->setLabel("次数");
    drawChart();
}

frmSpeedLineChart::~frmSpeedLineChart()
{
    delete ui;
}

void frmSpeedLineChart::UpdateData(const QVector<uint32_t> &Data)
{
    this->SpeedData.clear();
    SpeedData = Data;
    drawChart();

}

void frmSpeedLineChart::closeEvent(QCloseEvent *event)
{
    Q_EMIT closedSignal();
    QDialog::closeEvent(event);
}

void frmSpeedLineChart::drawChart()
{
    if(SpeedData.isEmpty()) return;

    QVector<double> keys,values;
    for(int i=0;i<SpeedData.size();i++){
        keys.append(i);
        values.append(SpeedData.at(i));
    }

    QCustomPlot *customPlot = ui->customplot;

    customPlot->clearGraphs();
    customPlot->addGraph();
    QCPGraph *graph = customPlot->graph(0);

    //加载数据
    graph->setData(keys,values);
    graph->setLineStyle(QCPGraph::lsLine);// 暂定直线
//     QPen pen(QColor(236, 156, 255));
//     graph->setPen(pen);
//     graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 5));
    graph->setSelectable(QCP::SelectionType::stSingleData);

    customPlot->rescaleAxes();
    customPlot->replot();
}

void frmSpeedLineChart::on_customplot_selectionChangedByUser()
{
    QCustomPlot* customPlot=ui->customplot;

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
                    QString dataText = QString("次数: %2\n转速: %3(rpm)").arg(double_key).arg(str_value);
                    QToolTip::showText(QCursor::pos(), dataText);
                }
            }
        }
    }
}
