#ifndef FRMRMUPGRADE_H
#define FRMRMUPGRADE_H

#include <QWidget>
#include <QRadioButton>
#include "udpmulticastapi.h"
namespace Ui {
class FrmRMUpgrade;
}

class FrmRMUpgrade : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRMUpgrade(QWidget *parent = nullptr);
    ~FrmRMUpgrade();

private:
    Ui::FrmRMUpgrade *ui;
    struct UPGRADEFILEINFO{
        QString filename;
        qint64 filesize;
        QString Version_hardware;
        QString Version_software;
        QString Date;
        QString softwareType;
        QString boardName;
        QString ProjectName;
        uint32_t crc;
    };

    QString Wagon;
    QString title;
    uint16_t PackSize;
    int BlockSize;
    QList <QRadioButton *> menubtns;
    QButtonGroup *buttonGroup;
    bool NeedInit;
    UPGRADEFILEINFO upgradefileinfo;

    void InitFrom();
    void LoadPre();
    void LoadFileInfo();
    //初始化内容
    void frmInit();
    void clearCboxPre();

public:
    QString GetCurrentWagon() const;
    void SetCurrentWagon(const QString &wagon);
    void RedrawData();
private slots:
    void on_btn_save_clicked();
    void on_btn_Openfile_clicked();
    void Updateprogress(int currentcount);
    void on_rbtn_Pre_toggled(bool checked);
};

#endif // FRMRMUPGRADE_H
