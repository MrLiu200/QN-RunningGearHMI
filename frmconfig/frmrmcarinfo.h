#ifndef FRMRMCARINFO_H
#define FRMRMCARINFO_H

#include <QWidget>
#include <QLineEdit>
#include "udpmulticastapi.h"
namespace Ui {
class FrmRMCarInfo;
}

class FrmRMCarInfo : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRMCarInfo(QWidget *parent = nullptr);
    ~FrmRMCarInfo();

private:
    Ui::FrmRMCarInfo *ui;
    QList<QLineEdit *> txtCarInfo;
    QStringList carinfo;
    QString Wagon;
    bool NeedSave;
    QString cartype;

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
    void UDPDataReturn(QString project, int linenum, QString carnum, QString wagon,float WheelDiameter,QString type,double sppedcoefficient);
};

#endif // FRMRMCARINFO_H
