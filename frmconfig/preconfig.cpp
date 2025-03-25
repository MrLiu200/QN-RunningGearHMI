#include "preconfig.h"
#include "ui_preconfig.h"

PreConfig::PreConfig(QWidget *parent, QString windowstitle) :
    QDialog(parent),
    ui(new Ui::PreConfig),
    title(windowstitle)
{
    ui->setupUi(this);
    //    connect(ui->btn_ok,&QPushButton::clicked,this,&PreConfig::accept);
    connect(ui->btn_cancel,&QPushButton::clicked,this,&PreConfig::reject);
    this->setWindowTitle(title);

    QList<QLineEdit *> txtlist = ui->widget_2->findChildren<QLineEdit *>();
    for(QLineEdit *txt : txtlist){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    QStringList labellist;
    labellist << "轴箱" << "电机传动端" << "电机非传动端" << "齿轮箱";
    ui->comboBox->addItems(labellist);

    labellist.clear();
    labellist << "否" << "是";
    ui->comboBox_2->addItems(labellist);

    this->setFocus();
}

PreConfig::~PreConfig()
{
    delete ui;
}

void PreConfig::SetPreDefault(QStringList values)
{
    ui->txt_id->setText(values.at(0));
    ui->txt_ch->setText(values.at(1));
    ui->txt_name->setText(values.at(2));
    ui->comboBox->setCurrentText(values.at(3));
    ui->txt_Sensitivity->setText(values.at(4));
    ui->txt_ShaftDiameter->setText(values.at(5));
    ui->txt_bearing1_name->setText(values.at(6));
    ui->cbox_bearing1_model->setCurrentText(values.at(7));
    ui->txt_bearing2_name->setText(values.at(8));
    ui->cbox_bearing2_model->setCurrentText(values.at(9));
    ui->txt_bearing3_name->setText(values.at(10));
    ui->cbox_bearing3_model->setCurrentText(values.at(11));
    ui->txt_bearing4_name->setText(values.at(12));
    ui->cbox_bearing4_model->setCurrentText(values.at(13));
    ui->txt_capstanName->setText(values.at(14));
    ui->txt_capstanTeethNum->setText(values.at(15));
    ui->txt_DrivenwheelName->setText(values.at(16));
    ui->txt_DrivenwheelTeethNum->setText(values.at(17));
    ui->txt_version->setText(values.at(18));
    int currentEnable = values.last() == "Disabled" ? 0:1;
    ui->comboBox_2->setCurrentIndex(currentEnable);

    if(this->title == "修改"){
        ui->txt_id->setReadOnly(true);
        ui->txt_ch->setReadOnly(true);
    }
}

void PreConfig::SetBearingDefault(QStringList bearinglist)
{
    if(bearinglist.isEmpty()) return;

    ui->cbox_bearing1_model->addItems(bearinglist);
    ui->cbox_bearing2_model->addItems(bearinglist);
    ui->cbox_bearing3_model->addItems(bearinglist);
    ui->cbox_bearing4_model->addItems(bearinglist);
}

QStringList PreConfig::GetPreInfo()
{
    QStringList preinfo;
    preinfo.append(ui->txt_id->text());
    preinfo.append(ui->txt_ch->text());
    preinfo.append(ui->txt_name->text());
    preinfo.append(ui->comboBox->currentText());
    preinfo.append(ui->txt_Sensitivity->text());
    preinfo.append(ui->txt_ShaftDiameter->text());
    preinfo.append(ui->txt_bearing1_name->text());
    preinfo.append(ui->cbox_bearing1_model->currentText());
    preinfo.append(ui->txt_bearing2_name->text());
    preinfo.append(ui->cbox_bearing2_model->currentText());
    preinfo.append(ui->txt_bearing3_name->text());
    preinfo.append(ui->cbox_bearing3_model->currentText());
    preinfo.append(ui->txt_bearing4_name->text());
    preinfo.append(ui->cbox_bearing4_model->currentText());
    preinfo.append(ui->txt_capstanName->text());
    preinfo.append(ui->txt_capstanTeethNum->text());
    preinfo.append(ui->txt_DrivenwheelName->text());
    preinfo.append(ui->txt_DrivenwheelTeethNum->text());
    preinfo.append(ui->txt_version->text());
    preinfo.append(QString::number(ui->comboBox_2->currentIndex()));
    return preinfo;
}

void PreConfig::on_btn_ok_clicked()
{
    // 检查每个控件的内容是否为空
    bool allFilled = true;

    // 检查文本框和组合框的内容
    if (ui->txt_id->text().isEmpty() || ui->txt_ch->text().isEmpty() ||
            ui->txt_name->text().isEmpty() || ui->comboBox->currentText().isEmpty() ||
            ui->txt_Sensitivity->text().isEmpty() || ui->txt_ShaftDiameter->text().isEmpty() ||
            ui->txt_bearing1_name->text().isEmpty() || ui->cbox_bearing1_model->currentText().isEmpty() ||
            ui->txt_bearing2_name->text().isEmpty() || ui->cbox_bearing2_model->currentText().isEmpty() ||
            ui->txt_bearing3_name->text().isEmpty() || ui->cbox_bearing3_model->currentText().isEmpty() ||
            ui->txt_bearing4_name->text().isEmpty() || ui->cbox_bearing4_model->currentText().isEmpty() ||
            ui->txt_capstanName->text().isEmpty() || ui->txt_capstanTeethNum->text().isEmpty() ||
            ui->txt_DrivenwheelName->text().isEmpty() || ui->txt_DrivenwheelTeethNum->text().isEmpty() ||
            ui->txt_version->text().isEmpty() || ui->comboBox_2->currentIndex() == -1)
    {
        allFilled = false; // 如果有任何控件为空，设置为 false
    }
    if(allFilled){
        Q_EMIT PreConfig::accept();
    }else{
        QMessageBox::warning(this,"错误","未填充完整");
    }
}
