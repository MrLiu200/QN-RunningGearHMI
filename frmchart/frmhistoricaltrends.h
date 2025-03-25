#ifndef FRMHISTORICALTRENDS_H
#define FRMHISTORICALTRENDS_H

#include <QDialog>
#include <QCheckBox>
#include "qcustomplot.h"
namespace Ui {
class frmHistoricalTrends;
}
#define PlotCount 13
typedef struct HISTORYDEVICE{
    QString name;
    QString wagon;
    int id;
    int ch;
    QVector<double> values;
    QVector<double> keys;
}HISTORYDEVICE;

typedef struct SELECTSTRUCE{
    QString starttime;
    QString endtime;
    bool usespeed;
    double speedMax;
    double speedMin;
    bool uselimit;
    double limitMax;
    double limitMin;
}SELECTSTRUCE;
class frmHistoricalTrends : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString Widgettitle READ GetWidgettitle WRITE SetWidgettitle)
public:
    explicit frmHistoricalTrends(QStringList selectdevice, bool IsEnvelope = false, QWidget *parent = nullptr);
    ~frmHistoricalTrends();

private:
    Ui::frmHistoricalTrends *ui;
    QList <QRadioButton *> menubtns;
    QList <QColor> lineColors;
//    QCPItemText *textLabel;
    QString Widgettitle;
    QList<HISTORYDEVICE> HistoryDevice;
    QStringList deviceList;
    SELECTSTRUCE FilterCriteria;
    QList <QString> ColumnNames;
    bool envelope;

    //初始化函数
    void InitFrm(void);
    void InitDevice();
    void DataGetInit(void);
    void SeleteInit(void);
    void ImageCheckInit(void);
    void InitPlot(void);

public:
    QString GetWidgettitle() const;
    void SetWidgettitle(QString name);
private slots:
    void on_btnImageLoad_clicked();

    void on_widgetCustomplot_selectionChangedByUser();

    void on_cboxScreenValue_clicked(bool checked);

    void on_cboxScreenSpeed_clicked(bool checked);

    void on_btnSelete_clicked();

    void plot_mousepress(QMouseEvent *event);

    void rescaleAxes();

    void contextMenuRequest(QPoint pos);

    void mousedoubleclicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);


private:
    //图像清理
    void CustomPlot_clear(QCustomPlot *customPlot);

};

#endif // FRMHISTORICALTRENDS_H
