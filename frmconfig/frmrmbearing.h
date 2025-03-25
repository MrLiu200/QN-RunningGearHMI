#ifndef FRMRMBEARING_H
#define FRMRMBEARING_H

#include <QWidget>
#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include "predata.h"
#include "udpmulticastapi.h"
namespace Ui {
class FrmRMBearing;
}

class FrmRMBearing : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString Wagon READ GetCurrentWagon WRITE SetCurrentWagon);

public:
    explicit FrmRMBearing(QWidget *parent = nullptr);
    ~FrmRMBearing();

private:
    Ui::FrmRMBearing *ui;
    QStandardItemModel *model;
    QString Wagon;
    QStringList columnNames;
    QList<int> columnWidths;
    bool NeedSave;
    bool NeedInit;

    //初始化UI
    void InitTable();

    //刷新数据
    void RefreshData();

public:
    QString GetCurrentWagon() const;
    void SetCurrentWagon(const QString &wagon);

    void SaveData();
    void RedrawData();
private slots:
    void on_btn_add_clicked();
    void on_btn_delete_clicked();
    void on_btn_clear_clicked();
    void on_btn_save_clicked();
    void DataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};

#endif // FRMRMBEARING_H
