#ifndef FRMREALTIMEDEBUG_H
#define FRMREALTIMEDEBUG_H

#include <QDialog>
#include <QLineEdit>
#include <QContextMenuEvent>
#include <QLabel>
namespace Ui {
class frmRealTimeDebug;
}

class frmRealTimeDebug : public QDialog
{
    Q_OBJECT

public:
    explicit frmRealTimeDebug(QWidget *parent = nullptr);
    ~frmRealTimeDebug();

private:
    Ui::frmRealTimeDebug *ui;

    QVector<float> O_Dimension;
    QVector<float> D_Dimension;
    QList<QLineEdit *> txt_ODimension;
    QList<QLineEdit *> txt_DDimension;
    QList<QLabel *> temstate;
    QStringList filterlist;
    bool enablefilter;
    bool pauseecho;
    void Init();

    //更新量纲
    void UpdateDimensionChart(const QVector<double> &RawData,const QVector<double> &DemodulateData);
    //更新报警信息
    void UpdateAlarmInfo(QStringList alarmlist);
    //查询是否需要显示
    bool InFilterList(QString wagon, qint8 id, qint8 ch, bool temmodel = false);
private slots:
    void ReturnEigenvalue(QString Wagon, qint8 id, qint8 ch, quint32 speed, double AmbientTem,double PointTem,QString time,
                          QStringList Dimensional,QStringList Demodulated,QStringList alarmlist);
    void RealtimeSpeedUpdate(quint32 speed);
    //温度巡检返回
    void ReturnTemValue(QString Wagon, qint8 id,QString time,qint8 ChannelCount,QStringList State);
    void on_btn_pause_clicked();

    void on_btn_clear_clicked();

    void on_btn_filter_clicked(bool checked);

    void on_txt_filter_returnPressed();

protected:
#if 1
    void contextMenuEvent(QContextMenuEvent *event) override;
#else
    bool eventFilter(QObject *watched, QEvent *event) override;
#endif
};

#endif // FRMREALTIMEDEBUG_H
