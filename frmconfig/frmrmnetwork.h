#ifndef FRMRMNETWORK_H
#define FRMRMNETWORK_H

#include <QWidget>
#include <QLineEdit>
#include "udpmulticastapi.h"
namespace Ui {
class FrmRMNetwork;
}

class FrmRMNetwork : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRMNetwork(QWidget *parent = nullptr);
    ~FrmRMNetwork();

private:
    Ui::FrmRMNetwork *ui;
    QList<QLineEdit *> txtNetInfo;
    QStringList networkinfo;
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

#endif // FRMRMNETWORK_H
