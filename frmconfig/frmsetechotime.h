#ifndef FRMSETECHOTIME_H
#define FRMSETECHOTIME_H

#include <QDialog>
#include "app.h"
namespace Ui {
class FrmSetEchoTime;
}

class FrmSetEchoTime : public QDialog
{
    Q_OBJECT

public:
    explicit FrmSetEchoTime(QWidget *parent = nullptr);
    ~FrmSetEchoTime();

private slots:
    void on_btn_ok_clicked();

private:
    Ui::FrmSetEchoTime *ui;
    void InitFrom();
};

#endif // FRMSETECHOTIME_H
