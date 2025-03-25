#ifndef FRMDEMODULATEDCHART_H
#define FRMDEMODULATEDCHART_H

#include <QDialog>
#include "qcustomplot.h"
#include "predata.h"
#include "frmblockinginterface.h"
namespace Ui {
class frmDemodulatedChart;
}

class frmDemodulatedChart : public QDialog
{
    Q_OBJECT

public:
    explicit frmDemodulatedChart(QWidget *parent = nullptr);
    ~frmDemodulatedChart();

private:
    Ui::frmDemodulatedChart *ui;

    QList<QColor> Linecolors;
    QList<PreData::PKDATASTRUCT> HideList;
    void InitFrom();
    void InitData();
    void AddToolbar(QCustomPlot *customplot, int type);
    //更新波形图
    void UpdateOriginalChart();
    //更新频谱图
    void UpdateSpectrumChart(bool hanning = true);
    //刷新界面
    void RefreshView();
    //清空所有设备信息
    void clearLayoutFromRow(QGridLayout *layout, int startRow);
    //图像清理
    void CustomPlot_clear(QCustomPlot *customPlot);

protected:
    void closeEvent(QCloseEvent *event) override;


private slots:
    void TrendpopupToolTip(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);

    //开关信号
    void SwitchButton_toggled(bool checked);
    void DeleteButton_clicked();
    void on_btn_clearpklist_clicked();

signals:
    void closesignals();
};

#endif // FRMDEMODULATEDCHART_H
