#include "linkdeviceconfig.h"
#include "ui_linkdeviceconfig.h"

LinkDeviceConfig::LinkDeviceConfig(QWidget *parent, QString windowstitle) :
    QDialog(parent),
    ui(new Ui::LinkDeviceConfig),
    title(windowstitle)
{
    ui->setupUi(this);
    this->setWindowTitle(title);

    connect(ui->btn_cancel,&QPushButton::clicked,this,&LinkDeviceConfig::reject);

    QList<QLineEdit *> txtlist = ui->widget_3->findChildren<QLineEdit *>();
    for(QLineEdit *txt : txtlist){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    QStringList list;
    list << "Host" << "StandbyHost" << "Slave";
    ui->cbox_type->addItems(list);

    this->setFocus();
}

LinkDeviceConfig::~LinkDeviceConfig()
{
    delete ui;
}

QStringList LinkDeviceConfig::GetLinkInfo()
{
    QStringList list;
    list.append(ui->cbox_type->currentText());
    list.append(ui->txt_ip->text());
    list.append(ui->txt_port->text());
    list.append(ui->txt_wagon->text());

    return list;
}

void LinkDeviceConfig::SetDefaultInfo(QStringList values)
{
    ui->cbox_type->setCurrentText(values.at(0));
    ui->txt_ip->setText(values.at(1));
    ui->txt_port->setText(values.at(2));
    ui->txt_wagon->setText(values.at(3));
    ui->txt_ip->setReadOnly(true);
}

void LinkDeviceConfig::on_btn_ok_clicked()
{
    // 检查每个控件的内容是否为空
    bool allFilled = true;

    // 检查文本框和组合框的内容
    if (ui->txt_ip->text().isEmpty() || ui->txt_port->text().isEmpty() ||
            ui->txt_wagon->text().isEmpty() || ui->cbox_type->currentIndex() == -1)
    {
        allFilled = false; // 如果有任何控件为空，设置为 false
    }
    if(allFilled){
        Q_EMIT LinkDeviceConfig::accept();
    }else{
        QMessageBox::warning(this,"错误","未填充完整");
    }
}
