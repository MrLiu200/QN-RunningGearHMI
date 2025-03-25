#ifndef FRMRMDATASAVE_H
#define FRMRMDATASAVE_H

#include <QWidget>
#include <QLineEdit>
#include "udpmulticastapi.h"
namespace Ui {
class FrmRMDataSave;
}

class FrmRMDataSave : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRMDataSave(QWidget *parent = nullptr);
    ~FrmRMDataSave();

private:
    Ui::FrmRMDataSave *ui;
    QList<QLineEdit *> txtSaveInterval;
    QList<int> intervalvalue;
    QString Wagon;
    bool NeedSave;
    bool EnableInterval;
    bool btn_State;

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
    void UDPDataReturn(QString wagon,int lowpowerspeedmax, int temspeedmax, bool IsEnable, int VibInterval, int TemInterval);
};

#endif // FRMRMDATASAVE_H
