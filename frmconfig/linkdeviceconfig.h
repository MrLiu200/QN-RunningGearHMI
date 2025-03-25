#ifndef LINKDEVICECONFIG_H
#define LINKDEVICECONFIG_H

#include <QDialog>
#include <QMessageBox>
namespace Ui {
class LinkDeviceConfig;
}

class LinkDeviceConfig : public QDialog
{
    Q_OBJECT

public:
    explicit LinkDeviceConfig(QWidget *parent = nullptr, QString windowstitle = "联动设备配置");
    ~LinkDeviceConfig();

private:
    Ui::LinkDeviceConfig *ui;
    QString title;

public:
    QStringList GetLinkInfo();
    void SetDefaultInfo(QStringList values);

private slots:
    void on_btn_ok_clicked();
};

#endif // LINKDEVICECONFIG_H
