#include "frmrmcarinfo.h"
#include "ui_frmrmcarinfo.h"
#include <QMessageBox>
#include "corehelper.h"
FrmRMCarInfo::FrmRMCarInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMCarInfo)
{
    ui->setupUi(this);
    frmInit();
    NeedSave = false;
}

FrmRMCarInfo::~FrmRMCarInfo()
{
    delete ui;
}

void FrmRMCarInfo::frmInit()
{
    QList<QLabel *> labels = findChildren<QLabel *>();

    for(auto lab : labels){
        lab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    QStringList typelist;
    typelist << "主机" << "备用主机" << "从机";

    ui->cbox_wagontype->addItems(typelist);


    txtCarInfo.append(ui->txt_projectname);
    txtCarInfo.append(ui->txt_linenum);
    txtCarInfo.append(ui->txt_carnum);
    txtCarInfo.append(ui->txt_wagonnum);
    txtCarInfo.append(ui->txt_wheeldiameter);
    txtCarInfo.append(ui->txt_speedCoefficient);

    for(auto txt : txtCarInfo){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setProperty("lastText","Null");
        connect(txt,&QLineEdit::editingFinished,this,&FrmRMCarInfo::DataChange);
    }

    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::carinforeturn,this,&FrmRMCarInfo::UDPDataReturn);
}

void FrmRMCarInfo::RefreshData()
{
    for(auto txt : txtCarInfo){
        txt->clear();
    }
    for(int i=0;i<carinfo.size();i++){
        txtCarInfo.at(i)->setText(carinfo.at(i));
        txtCarInfo.at(i)->setProperty("lastText",carinfo.at(i));
    }

    //查找索引
    if(!cartype.isEmpty()){
        int index = ui->cbox_wagontype->findText(cartype, Qt::MatchExactly);
        if(index != -1){
            ui->cbox_wagontype->setCurrentIndex(index);
        }
    }
}

QString FrmRMCarInfo::GetCurrentWagon() const
{
    return this->Wagon;
}

void FrmRMCarInfo::SetCurrentWagon(const QString &wagon)
{
    for(auto txt : txtCarInfo){
        txt->clear();
    }
    carinfo.clear();
    for(int i=0;i<txtCarInfo.size();i++){
        txtCarInfo.at(i)->setProperty("lastText","Null");
    }
    this->Wagon = wagon;
    UDPMulticastAPI::Instance()->GetCarConfig(Wagon);
}

void FrmRMCarInfo::SaveData()
{
    if(!NeedSave) return;

    if(QMessageBox::question(this,"保存","是否将改变内容提交") == QMessageBox::No){
        NeedSave = false;
    }else{
        on_btn_save_clicked();
    }
}

void FrmRMCarInfo::RedrawData()
{
    RefreshData();
}

void FrmRMCarInfo::on_btn_save_clicked()
{
    if(Wagon.isEmpty()) return;
    if (ui->txt_linenum->text().isEmpty() || ui->txt_carnum->text().isEmpty() ||
            ui->txt_wagonnum->text().isEmpty() || ui->txt_wheeldiameter->text().isEmpty() ||
            ui->txt_speedCoefficient->text().isEmpty() || ui->txt_projectname->text().isEmpty()){
        QMessageBox::warning(this,"提示","未填写完整");
        return;
    }
    QString project = ui->txt_projectname->text();
    QString line = ui->txt_linenum->text();
    QString carname = ui->txt_carnum->text();
    QString wagonname = ui->txt_wagonnum->text();
    float wheeldiameter = ui->txt_wheeldiameter->text().toFloat();
    QString oldtype = ui->cbox_wagontype->currentText();
    double Coefficient = ui->txt_speedCoefficient->text().toDouble();
    QString newtype = "Slave";
    //车辆类别转换
    if(oldtype == "主机"){
        newtype = "Host";
    }else if(oldtype == "备用主机"){
         newtype = "StandbyHost";
    }else if(oldtype == "从机"){
         newtype = "Slave";
    }

    UDPMulticastAPI::Instance()->UpdateCarInfo(project,wagonname,line,carname,wheeldiameter,newtype,Coefficient);
    NeedSave = false;
    CoreHelper::showFadingMessage(this,"已发送提交");
}

void FrmRMCarInfo::DataChange()
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

void FrmRMCarInfo::UDPDataReturn(QString project, int linenum, QString carnum, QString wagon, float WheelDiameter, QString type, double sppedcoefficient)
{
    if(wagon != this->Wagon) return;
    carinfo.clear();
    cartype.clear();
    carinfo.append(project);
    carinfo.append(QString::number(linenum));
    carinfo.append(carnum);
    carinfo.append(wagon);
    carinfo.append(QString::number(WheelDiameter));
    //更新一下视图，将车厢类型更改
    if(type == "Host"){
        cartype = "主机";
    }else if(type == "StandbyHost"){
        cartype = "备用主机";
    }else if(type == "Slave"){
        cartype = "从机";
    }
    carinfo.append(QString::number(sppedcoefficient));

    RefreshData();
}
