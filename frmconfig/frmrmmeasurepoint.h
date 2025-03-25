#ifndef FRMRMMEASUREPOINT_H
#define FRMRMMEASUREPOINT_H

#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include "predata.h"
#include "udpmulticastapi.h"
namespace Ui {
class FrmRMMeasurePoint;
}

class FrmRMMeasurePoint : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString Wagon READ GetCurrentWagon WRITE SetCurrentWagon);
public:
    explicit FrmRMMeasurePoint(QWidget *parent = nullptr);
    ~FrmRMMeasurePoint();

private:
    Ui::FrmRMMeasurePoint *ui;
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
    void linshifun(QString str);

public:
    QString GetCurrentWagon() const;
    void SetCurrentWagon(const QString &wagon);
    void SaveData();
    void RedrawData();

private slots:
    void on_btn_add_clicked();
    void on_btn_delete_clicked();
    void on_btn_clear_clicked();
    void on_btn_setsametype_clicked();
    void on_btn_setall_clicked();
    void on_btn_save_clicked();
    void DataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};

#endif // FRMRMMEASUREPOINT_H
