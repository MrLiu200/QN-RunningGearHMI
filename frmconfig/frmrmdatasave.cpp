#include "frmrmdatasave.h"
#include "ui_frmrmdatasave.h"
#include <QMessageBox>
#include "corehelper.h"
FrmRMDataSave::FrmRMDataSave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMDataSave)
{
    ui->setupUi(this);
    NeedSave = false;
    EnableInterval = false;
    btn_State = false;
    frmInit();
}

FrmRMDataSave::~FrmRMDataSave()
{
    delete ui;
}

void FrmRMDataSave::frmInit()
{
    QList<QLabel *> labels = findChildren<QLabel *>();

    for(auto lab : labels){
        lab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    txtSaveInterval.append(ui->txt_lowpowerTaskSpeed);
    txtSaveInterval.append(ui->txt_temTaskSpeed);
    txtSaveInterval.append(ui->txt_VIBInterval);
    txtSaveInterval.append(ui->txt_temInterval);

    for(auto txt : txtSaveInterval){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setProperty("lastText",-1);
        connect(txt,&QLineEdit::editingFinished,this,&FrmRMDataSave::DataChange);
    }

    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::DatasaveIntervalreturn,this,&FrmRMDataSave::UDPDataReturn);
}

void FrmRMDataSave::RefreshData()
{
    if(intervalvalue.isEmpty()) return;

    for(int i=0;i<intervalvalue.size();i++){
        txtSaveInterval.at(i)->setText(QString::number(intervalvalue.at(i)));
        txtSaveInterval.at(i)->setProperty("lastText",QString::number(intervalvalue.at(i)));
    }
    btn_State = EnableInterval;
    ui->widget_btn->setChecked(EnableInterval);
}

QString FrmRMDataSave::GetCurrentWagon() const
{
return this->Wagon;
}

void FrmRMDataSave::SetCurrentWagon(const QString &wagon)
{
    for(auto txt : txtSaveInterval){
        txt->clear();
    }
    intervalvalue.clear();
    for(int i=0;i<txtSaveInterval.size();i++){
        txtSaveInterval.at(i)->setProperty("lastText",-1);
    }
    EnableInterval = false;
    this->Wagon = wagon;
    UDPMulticastAPI::Instance()->GetSaveInterval(Wagon);
}

void FrmRMDataSave::SaveData()
{
    if(!NeedSave) return;

    if(QMessageBox::question(this,"保存","是否将改变内容提交") == QMessageBox::No){
        NeedSave = false;
    }else{
        on_btn_save_clicked();
    }
}

void FrmRMDataSave::RedrawData()
{
    RefreshData();
}

void FrmRMDataSave::on_btn_save_clicked()
{
    if (ui->txt_VIBInterval->text().isEmpty() || ui->txt_temInterval->text().isEmpty()){
        QMessageBox::warning(this,"提示","未填写完整");
        return;
    }
    int lowpowerspeedmax = ui->txt_lowpowerTaskSpeed->text().toInt();
    int temspeedmax = ui->txt_temTaskSpeed->text().toInt();
    bool Isenable = ui->widget_btn->isChecked();
    int vibInterval = ui->txt_VIBInterval->text().toInt();
    int temInterval = ui->txt_temInterval->text().toInt();

    UDPMulticastAPI::Instance()->UpdateSaveInterval(Wagon,lowpowerspeedmax,temspeedmax,Isenable,vibInterval,temInterval);
    NeedSave = false;
    CoreHelper::showFadingMessage(this,"已发送提交");
}

void FrmRMDataSave::DataChange()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
    if (!lineEdit) return;

    QString LastText = lineEdit->property("lastText").toString();
    QString newText = lineEdit->text();
    if(LastText != newText){
        lineEdit->setProperty("lastText",newText);
        NeedSave = true;
    }
    lineEdit->clearFocus();
}

void FrmRMDataSave::UDPDataReturn(QString wagon, int lowpowerspeedmax, int temspeedmax, bool IsEnable, int VibInterval, int TemInterval)
{
    if(wagon != this->Wagon) return;
    intervalvalue.clear();
    intervalvalue.append(lowpowerspeedmax);
    intervalvalue.append(temspeedmax);
    intervalvalue.append(VibInterval);
    intervalvalue.append(TemInterval);
    EnableInterval = IsEnable;
    RefreshData();
}
