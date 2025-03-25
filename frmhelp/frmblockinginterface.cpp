#include "frmblockinginterface.h"
#include "ui_frmblockinginterface.h"
#include <QMutex>
#include <QTime>
frmBlockingInterface *frmBlockingInterface::self = 0;
frmBlockingInterface *frmBlockingInterface::Instance()
{
    if(!self){
        QMutex mutex;
        QMutexLocker lock(&mutex);
        if(!self){
            QString str = "等待窗口";
            self = new frmBlockingInterface;
        }
    }
    return self;
}

frmBlockingInterface::frmBlockingInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmBlockingInterface)
{
    ui->setupUi(this);
    InitFrm();
//    setStyleSheet("QDialog{"
//                  "border: 2px solid blue;"
//                  "border-radius: 15px;"
//                  "};"
//                  "QWidget{"
//                  "border: 2px solid blue;"
//                  "border-radius: 15px;"
//                  "};"
//                  "QLabel{"
//                  "border: 2px solid blue;"
//                  "border-radius: 15px;"
//                  "};"
//    );
    setStyleSheet("QDialog{border: 1px solid blue; border-radius: 10px;}"
                  "QLabel#label{border: 1px solid blue; border-radius: 10px;}");
    Sec = 0;
    FailColor = "#FF0000";
    colorisfail = false;
    timerSetColor = new QTimer;
    timerSetColor->setInterval(500);
    connect(timerSetColor,&QTimer::timeout,this,&frmBlockingInterface::SwitchColor);
    timerCount = new QTimer;
    timerCount->setInterval(1000);
    connect(timerCount,&QTimer::timeout,this,&frmBlockingInterface::Updatetime);

    setModal(true);
//    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    ui->btn_close->setVisible(false);
}

frmBlockingInterface::~frmBlockingInterface()
{
    if(timerSetColor->isActive()){
        timerSetColor->stop();
    }
    if(timerCount->isActive()){
        timerCount->stop();
    }
    ui->lab_time->setText("00:00:00");
    delete ui;
}

void frmBlockingInterface::StartShow()
{
    QString str = QString("background-color:#00DB00;color:#000000");
    ui->btn_close->setVisible(false);
    ui->label->setText(lab_Text);
    ui->label->setStyleSheet(str);
    timerCount->start();
    this->show();
}

void frmBlockingInterface::SetLabelText(QString text)
{
    this->lab_Text = text;
}

void frmBlockingInterface::Stop()
{
    if(timerSetColor->isActive()){
        timerSetColor->stop();
    }
    if(timerCount->isActive()){
        timerCount->stop();
    }
    Sec = 0;
    ui->lab_time->setText("00:00:00");
    this->hide();
}

void frmBlockingInterface::SetLabelColor(QString color)
{
    QString str = QString("background-color:%1").arg(color);
    ui->label->setStyleSheet(str);
}

void frmBlockingInterface::SetFalse(QString colorHtml, QString text)
{
    QString str = QString("background-color:%1;color:#FFFD55").arg(colorHtml);
    ui->label->setText(text);
    ui->label->setStyleSheet(str);
    colorisfail = true;
//    setWindowFlag(Qt::WindowCloseButtonHint,true);
    timerSetColor->start();
    ui->btn_close->setVisible(true);
}

void frmBlockingInterface::InitFrm()
{
    this->setFixedSize(150,120);
    ui->label->setText(lab_Text);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);  // 移除所有装饰
//    setWindowFlag(Qt::WindowCloseButtonHint,false);
    setWindowTitle("任务进行中...");
}

void frmBlockingInterface::SwitchColor()
{
    if(colorisfail){
        ui->label->setStyleSheet("background-color:#ffffff");
    }else{
        ui->label->setStyleSheet("background-color:#ff0000");
    }
    colorisfail = !colorisfail;
}

void frmBlockingInterface::Updatetime()
{
    Sec++;
    QString condition = QString("%1").arg(Sec > 9 ? "ss" : "s");
    QString time = QTime::currentTime().fromString(QString::number(Sec),condition).toString("HH:mm:ss");
    ui->lab_time->setText(time);
}

void frmBlockingInterface::on_btn_close_clicked()
{
    Stop();
//    Q_EMIT closeview();
}
