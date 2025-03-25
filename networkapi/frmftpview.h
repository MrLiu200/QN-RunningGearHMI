#ifndef FRMFTPVIEW_H
#define FRMFTPVIEW_H

#include <QDialog>

namespace Ui {
class frmFtpView;
}

class frmFtpView : public QDialog
{
    Q_OBJECT

public:
    explicit frmFtpView(QWidget *parent = nullptr);
    ~frmFtpView();

private:
    Ui::frmFtpView *ui;
};

#endif // FRMFTPVIEW_H
