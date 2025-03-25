#ifndef FRMUSERCHANGE_H
#define FRMUSERCHANGE_H

#include <QDialog>

namespace Ui {
class frmUserChange;
}

class frmUserChange : public QDialog
{
    Q_OBJECT

public:
    explicit frmUserChange(QWidget *parent = nullptr);
    ~frmUserChange();

private:
    Ui::frmUserChange *ui;

private slots:
    void verify();

signals:
    void UserChanged(QString username);
};

#endif // FRMUSERCHANGE_H
