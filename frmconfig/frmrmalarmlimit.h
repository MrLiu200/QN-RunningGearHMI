#ifndef FRMRMALARMLIMIT_H
#define FRMRMALARMLIMIT_H

#include <QWidget>
#include <QLineEdit>
#include "udpmulticastapi.h"
#include "toggleswitch.h"
namespace Ui {
class FrmRMAlarmLimit;
}

class FrmRMAlarmLimit : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRMAlarmLimit(QWidget *parent = nullptr);
    ~FrmRMAlarmLimit();

private:
    Ui::FrmRMAlarmLimit *ui;
    QList<ToggleSwitch*> switchbtns;
    QList<QLineEdit *> txtAlarmLimit;
    QStringList alarmlimitinfo;
    QList <bool> btnsstate;
    QString Wagon;
    bool NeedSave;

    //初始化内容
    void frmInit();

    //刷新数据
    void RefreshData();

public:
    QString GetCurrentWagon() const;
    void SetCurrentWagon(const QString &wagon);
    void SaveData();
    void RedrawData();

private slots:
    void on_btn_save_clicked();
    void DataChange();
    void UDPDataReturn(QString wagon,QStringList linklist);
};

#endif // FRMRMALARMLIMIT_H
