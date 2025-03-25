#ifndef FRMRMCONFIG_H
#define FRMRMCONFIG_H

#include <QWidget>
#include <QButtonGroup>
#include <QToolButton>
namespace Ui {
class FrmRMConfig;
}

class FrmRMConfig : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRMConfig(QWidget *parent = nullptr);
    ~FrmRMConfig();

private:
    Ui::FrmRMConfig *ui;
    QList <QToolButton*> functionbtns;
    QList <QToolButton*> wagonbtns;
    QButtonGroup *FunctionbtnGroup;
    QButtonGroup *WagonbtnGroup;
    QString selectedwagon;
    QString currentFunction;
    bool NeedSave;



private:
    void InitFrom();
    void InitWagon();
    void InitWidget();

    //提交当前已变更的界面
    void SubmitCurrentChanges();

    //检查是否需要切换车厢
    void CheckWagon();

    //设备重启
    void RMReboot();


private slots:
    void FunctionViewSwitch(QAbstractButton *btn);
    void WagonSwitch(QAbstractButton *btn);
};

#endif // FRMRMCONFIG_H
