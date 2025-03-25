#ifndef FRMDBCHART_H
#define FRMDBCHART_H

#include <QDialog>
#include <QLineEdit>
#include <QGroupBox>
namespace Ui {
class frmDBChart;
}

class frmDBChart : public QDialog
{
    Q_OBJECT

public:
    explicit frmDBChart(const QStringList &DBResultsList,const QString BearingType,QWidget *parent = nullptr);
    ~frmDBChart();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::frmDBChart *ui;
    QStringList dblist;
    QString BearingModel;
    QStringList Fault_Type;
    QList<QLineEdit*> txt_baowai;
    QList<QLineEdit*> txt_baonei;
    QList<QLineEdit*> txt_waihuan;
    QList<QLineEdit*> txt_neihuan;
    QList<QLineEdit*> txt_gundan;

    QList<QLineEdit*> txt_baowai_2;
    QList<QLineEdit*> txt_baonei_2;
    QList<QLineEdit*> txt_waihuan_2;
    QList<QLineEdit*> txt_neihuan_2;
    QList<QLineEdit*> txt_gundan_2;

    QList<QLineEdit*> txt_baowai_3;
    QList<QLineEdit*> txt_baonei_3;
    QList<QLineEdit*> txt_waihuan_3;
    QList<QLineEdit*> txt_neihuan_3;
    QList<QLineEdit*> txt_gundan_3;

    QList<QLineEdit*> txt_baowai_4;
    QList<QLineEdit*> txt_baonei_4;
    QList<QLineEdit*> txt_waihuan_4;
    QList<QLineEdit*> txt_neihuan_4;
    QList<QLineEdit*> txt_gundan_4;

    QList<QLineEdit*> txt_dachilun;
    QList<QLineEdit*> txt_xiaochilun;
    QList<QLineEdit*> txt_tamian;
    QList<QList<QLineEdit*>> txt_aggregate;

    QList<QGroupBox*> groupList;

    void frmInit();
    void UpdateChart();
    void FlushData();
    void ClearChart();

public:
    void UpdateData(const QStringList &DBResultsList, const QString BearingType);

signals:
    void closedSignal();
};

#endif // FRMDBCHART_H
