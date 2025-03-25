#ifndef FRMRMTIMERTASK_H
#define FRMRMTIMERTASK_H

#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include "udpmulticastapi.h"
namespace Ui {
class FrmRMTimertask;
}

class FrmRMTimertask : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRMTimertask(QWidget *parent = nullptr);
    ~FrmRMTimertask();

private:
    Ui::FrmRMTimertask *ui;
    QStandardItemModel *model;
    QString Wagon;
    QStringList columnNames;
    QList<int> columnWidths;
    bool NeedSave;
    QStringList Tasks;
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
    void UDPDataReturn(QString wagon,QStringList linklist);
};

#endif // FRMRMTIMERTASK_H
