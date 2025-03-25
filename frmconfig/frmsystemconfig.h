#ifndef FRMSYSTEMCONFIG_H
#define FRMSYSTEMCONFIG_H

#include <QDialog>
#include <QtNetwork>
#include <QMessageBox>

namespace Ui {
class frmSystemConfig;
}

class frmSystemConfig : public QDialog
{
    Q_OBJECT

public:
    static frmSystemConfig *Instance();
    explicit frmSystemConfig(QWidget *parent = nullptr);
    ~frmSystemConfig();

private:
    static frmSystemConfig *self;
    void initfrm(void);
    void baseconfigInit(void);
    void aboutconfigInit(void);
    void netconfigInit(void);


private slots:
    void on_btnGetNetConfig_clicked();

    void on_btnSetNetConfig_clicked();
    void baseconfigupdate(void);

private:
    Ui::frmSystemConfig *ui;
};

#endif // FRMSYSTEMCONFIG_H
