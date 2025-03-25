#include "timertaskconfig.h"
#include "ui_timertaskconfig.h"

TimerTaskConfig::TimerTaskConfig(QWidget *parent, QString windowstitle) :
    QDialog(parent),
    ui(new Ui::TimerTaskConfig),
    title(windowstitle)
{
    ui->setupUi(this);
    this->setWindowTitle(title);

    connect(ui->btn_ok,&QPushButton::clicked,this,&TimerTaskConfig::accept);
    connect(ui->btn_cancel,&QPushButton::clicked,this,&TimerTaskConfig::reject);
    QStringList list;
    list << "重启" << "压缩";
    ui->comboBox->addItems(list);

    this->setFocus();
}

TimerTaskConfig::~TimerTaskConfig()
{
    delete ui;
}

QStringList TimerTaskConfig::GetTaskInfo()
{
    QStringList list;
    list.append(ui->comboBox->currentText());
    QString time = ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    list.append(time);

    return list;
}

void TimerTaskConfig::SetDefaultInfo(QString action)
{
    ui->comboBox->setCurrentText(action);
    ui->comboBox->setEnabled(false);
}
