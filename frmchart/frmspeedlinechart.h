#ifndef FRMSPEEDLINECHART_H
#define FRMSPEEDLINECHART_H

#include <QDialog>
#include "qcustomplot.h"
namespace Ui {
class frmSpeedLineChart;
}

class frmSpeedLineChart : public QDialog
{
    Q_OBJECT

public:
    explicit frmSpeedLineChart(const QVector<uint32_t> &Data,QWidget *parent = nullptr);
    ~frmSpeedLineChart();

    void UpdateData(const QVector<uint32_t> &Data);

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::frmSpeedLineChart *ui;
    QVector<uint32_t> SpeedData;

    void drawChart();

signals:
    void closedSignal();

private slots:
    void on_customplot_selectionChangedByUser();
};

#endif // FRMSPEEDLINECHART_H
