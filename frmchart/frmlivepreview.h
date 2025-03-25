#ifndef FRMLIVEPREVIEW_H
#define FRMLIVEPREVIEW_H

#include <QWidget>
#include "qcustomplot.h"
#include <QMouseEvent>
#include "frmspeedlinechart.h"
#include "frmdemodulatedchart.h"
#include "frmdbchart.h"
#include <QThread>
#include "frmblockinginterface.h"
#include "frmfaultfrequencychart.h"
namespace Ui {
class frmLivePreview;
}

typedef struct UIECHODATA{
    QString currentfile;
    uint32_t currentspeed;
    QVector<double> RawData;
    QVector<double> DemodulateData;
    QVector<float> O_Dimension;
    QVector<float> D_Dimension;
    QVector<uint32_t> SpeedVector;
    QStringList DiagnosticResults;
    QString BearingType;
}UIECHODATA;

typedef struct LIVEDEVICEINFO{
    QString wagon;
    int id;
    int ch;
    QString name;
    float sensitivity;
    QStringList filelist;
    UIECHODATA currentecho;
    QVector<double> FileValues;
    QVector<double> FileKeys;
}LIVEDEVICEINFO;
class frmLivePreview : public QWidget
{
    Q_OBJECT

public:
    explicit frmLivePreview(QStringList selectdevice, bool Isdemodulated = false, QWidget *parent = nullptr);
    ~frmLivePreview();

private:
    Ui::frmLivePreview *ui;
    QStringList deviceList;
    LIVEDEVICEINFO livedeviceinfo;
    QString DataFileDir;
    QString InfoName;
    int RPM;
    QString InfoTime;
    QString InfoAlarm;
    QString CurrentFileName;
    QVector<double> RawData;
    QVector<double> FFTData;
    QList<QLineEdit *> txt_ODimension;
    QList<QLineEdit *> txt_DDimension;
    frmSpeedLineChart *m_SpeedChartWidget;
    frmDBChart *m_DBChartWidget;
    FrmFaultFrequencyChart *m_faultFrequency;
    bool isdemodulated;
    QMap<double, QCPItemText*> FaultFrequency_labels;
    QMap<double, QCPItemLine*> FaultFrequency_lines;

    void InitDevice();
    void InitFrm(void);
    void plotInit(void);
    //type =1 波形图， =2 频谱图  =3 趋势图
    void AddToolbar(QCustomPlot *customplot, int type);
public:
    void SetlabInfoName(QString Info);
    void SetlabInfoTime(QString time);
    void SetlabInfoAlarm(QString alarm);
    void Test20250206(QString Dirname){
        livedeviceinfo.id = 1;
        livedeviceinfo.ch = 1;
        livedeviceinfo.name = "测试车厢";
        livedeviceinfo.wagon = "QN-t0021";
        livedeviceinfo.sensitivity = 10;
        livedeviceinfo.currentecho = UIECHODATA();
        livedeviceinfo.FileValues = QVector<double>();
        livedeviceinfo.FileKeys = QVector<double>();
        DataImport();
    };

private:
    //更新趋势图
    void UpdateTrendChart();
    //更新波形图
    void UpdateOriginalChart();
    //更新频谱图
    void UpdateSpectrumChart(bool hanning = true);
    //加载数据
    void LoadVIBData();
    //更新信息栏
    void UpdateInformationBar();
    //更新量纲图
    void UpdateDimensionChart();

private slots:
    void DataImport();
    void TrendpopupToolTip(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void TrendLoadFile(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
#if 0
    void on_CustomPlot_Trend_selectionChangedByUser();
    void on_CustomPlotOriginal_selectionChangedByUser();
    void on_CustomPlot_Spectrum_selectionChangedByUser();
#endif
    void on_btn_GetData_clicked();
    void on_btn_echoDimension_clicked();
    void on_btn_SpeedVector_clicked(bool checked);
    void on_btn_closeTrend_clicked(bool checked);
    void on_btn_echoDBValue_clicked(bool checked);
    void on_btn_QueryData_clicked();

    void on_btn_addPK_clicked();
#if 0
    void showFadingMessage(const QString &message){
        // 创建 QLabel 作为提示框
        QLabel *label = new QLabel(message, this);
        label->setStyleSheet("background-color: rgba(0, 0, 0, 180); color: white; "
                             "border-radius: 5px; padding: 5px;");
        label->setAlignment(Qt::AlignCenter);

        // 设置提示框的位置（居中）
        int x = (this->width() - 100) / 2;
        int y = (this->height() - 100) / 2;
        label->setGeometry(x, y, 100, 100);
        label->show();

        // 创建透明度效果
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(label);
        label->setGraphicsEffect(effect);

        // 创建透明度动画（淡出效果）
        QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity");
        animation->setDuration(1500);  // 2 秒完成动画
        animation->setStartValue(0.8); // 初始完全不透明
        animation->setEndValue(0.0);   // 结束时完全透明

        // 动画完成后，删除 label
        connect(animation, &QPropertyAnimation::finished, label, &QLabel::deleteLater);

        // 启动动画
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    };
#endif
    void on_btn_faultFrequency_clicked(bool checked);
    void SetMarkState(QString &Describe, double &value, bool state);

signals:
    void unzipsuccess();
    void unzipfail();
    void copysuccess();
    void copyfail();

};

#endif // FRMLIVEPREVIEW_H
