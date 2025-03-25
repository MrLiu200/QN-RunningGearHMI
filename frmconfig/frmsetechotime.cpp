#include "frmsetechotime.h"
#include "ui_frmsetechotime.h"
#include <QMessageBox>
FrmSetEchoTime::FrmSetEchoTime(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmSetEchoTime)
{
    ui->setupUi(this);
    InitFrom();
}

FrmSetEchoTime::~FrmSetEchoTime()
{
    delete ui;
}

void FrmSetEchoTime::InitFrom()
{
    setWindowTitle("全局通用时间设置");
    resize(400,300);
    QList<QLabel *> labels = findChildren<QLabel *>();
    for(auto onelabel : labels){
        onelabel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    }


    ui->date_livestart->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->date_livestart->setDateTime(QDateTime::fromString(App::LiveDataStartTime,"yyyy-MM-dd HH:mm:ss"));
    ui->date_livestart->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->date_liveend->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->date_liveend->setDateTime(QDateTime::fromString(App::LiveDataEndTime,"yyyy-MM-dd HH:mm:ss"));
    ui->date_liveend->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->date_historystart->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->date_historystart->setDateTime(QDateTime::fromString(App::HistoryDataStartTime,"yyyy-MM-dd HH:mm:ss"));
    ui->date_historystart->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->date_historyend->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->date_historyend->setDateTime(QDateTime::fromString(App::HistoryDataEndTime,"yyyy-MM-dd HH:mm:ss"));
    ui->date_historyend->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    connect(ui->btn_cancel,&QPushButton::clicked,this,&FrmSetEchoTime::reject);
}

void FrmSetEchoTime::on_btn_ok_clicked()
{
    if(ui->date_liveend->date() < ui->date_livestart->date()){
        QMessageBox::warning(this,"错误","波形 结束时间不能小于开始时间");
        return;
    }
    if(ui->date_historyend->date() < ui->date_historystart->date()){
        QMessageBox::warning(this,"错误","趋势 结束时间不能小于开始时间");
        return;
    }
    QString livestart = ui->date_livestart->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString liveend = ui->date_liveend->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString hisstart = ui->date_historystart->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString hisend = ui->date_historyend->dateTime().toString("yyyy-MM-dd HH:mm:ss");

    App::LiveDataStartTime = livestart;
    App::LiveDataEndTime = liveend;
    App::HistoryDataStartTime = hisstart;
    App::HistoryDataEndTime = hisend;
    App::writeConfig();
    Q_EMIT FrmSetEchoTime::accept();
}
