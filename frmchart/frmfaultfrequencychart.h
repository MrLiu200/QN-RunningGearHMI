#ifndef FRMFAULTFREQUENCYCHART_H
#define FRMFAULTFREQUENCYCHART_H

#include <QDialog>
#include <QTreeWidgetItem>
namespace Ui {
class FrmFaultFrequencyChart;
}

class FrmFaultFrequencyChart : public QDialog
{
    Q_OBJECT

public:
    explicit FrmFaultFrequencyChart(QWidget *parent = nullptr);
    ~FrmFaultFrequencyChart();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::FrmFaultFrequencyChart *ui;

    QStringList BearingModelList;//轴承名称
    QString Wagon;//车厢名称
    QVector<double> RawData;//原始数据
    uint32_t SpeedRev;   //转速
    quint8 ID;
    quint8 CH;
    QString wagonType;
    bool NeedInit;

    QList<QPair<QString, double>> GearMeshFrequencyValues;//啮合频率
    QList<QPair<QString, double>> BearingFrequencyValue;//轴承通过频率
    QList<QPair<QString, double>> RotatingFrequencyValue;//转频
    QList<QTreeWidgetItem *> GearMeshItems;
    QList<QTreeWidgetItem *> BearingItems;
    QList<QTreeWidgetItem *> RotatingItems;//转频设置的项
    QStringList GearMeshKeys;


    void treewidget_init();
    void TreeGearMesh_Init();
    void TreeBearing_Init();
    void TreeRotating_Init();
    void DataInit();
    QVector<double> QueryBearingParameters(QString model);   //查询轴承参数
    void flushData();

public:
    void SetDataInfo(const QString wagon, const quint8 id, const quint8 ch, const uint32_t speed,
                     const QString bearingModels, const QVector<double> data);
    void UpdateInfo(const uint32_t speed, const QVector<double> data);

private slots:
    void onItemChanged(QTreeWidgetItem *item, int column);
    void SetSimulateSpeedRev();

    void on_checkBox_toggled(bool checked);

signals:
    void UserChecked(QString &Describe, double &value, bool state);
    void closedSignal();
};

#endif // FRMFAULTFREQUENCYCHART_H
