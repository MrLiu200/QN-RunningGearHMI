#include "frmftpview.h"
#include "ui_frmftpview.h"

frmFtpView::frmFtpView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmFtpView)
{
    ui->setupUi(this);
}

frmFtpView::~frmFtpView()
{
    delete ui;
}
