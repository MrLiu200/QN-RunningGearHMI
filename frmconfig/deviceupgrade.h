#ifndef DEVICEUPGRADE_H
#define DEVICEUPGRADE_H

#include <QDialog>
#include <QRadioButton>

namespace Ui {
class DeviceUpgrade;
}

class DeviceUpgrade : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceUpgrade(QWidget *parent = nullptr, QString windowstitle = "未知");
    ~DeviceUpgrade();

private slots:
    void on_rbtn_Pre_clicked(bool checked);

    void on_btn_start_clicked();

    void on_btn_Openfile_clicked();

    void Updateprogress(int currentcount);

private:
    Ui::DeviceUpgrade *ui;
    QString wagon;
    QString title;
    QString currentfile;
    qint64 currentsize;
    uint32_t currentcrc;
    QString currentversion;
    uint16_t PackSize;
    int BlockSize;
    QList <QRadioButton *> menubtns;

    void InitFrom();
    void LoadFileInfo();
};

#endif // DEVICEUPGRADE_H
