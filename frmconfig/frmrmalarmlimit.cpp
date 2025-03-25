#include "frmrmalarmlimit.h"
#include "ui_frmrmalarmlimit.h"
#include <QMessageBox>
#include "corehelper.h"
FrmRMAlarmLimit::FrmRMAlarmLimit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMAlarmLimit)
{
    ui->setupUi(this);
    NeedSave = false;
    frmInit();
//    ui->groupBox_6->setVisible(false);
}

FrmRMAlarmLimit::~FrmRMAlarmLimit()
{
    delete ui;
}

void FrmRMAlarmLimit::frmInit()
{
    QList<QLabel *> labels = findChildren<QLabel *>();

    for(auto lab : labels){
        lab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    txtAlarmLimit.append(ui->txt_baowaiwarning);
    txtAlarmLimit.append(ui->txt_baoneiwaring);
    txtAlarmLimit.append(ui->txt_waihuanwarning);
    txtAlarmLimit.append(ui->txt_neihuanwarning);
    txtAlarmLimit.append(ui->txt_gundanwarning);
    txtAlarmLimit.append(ui->txt_gunshuangwarning);
    txtAlarmLimit.append(ui->txt_tamianwarning);
    txtAlarmLimit.append(ui->txt_benzhouwarning);
    txtAlarmLimit.append(ui->txt_linzhouwarning);
    txtAlarmLimit.append(ui->txt_baowaifirst);
    txtAlarmLimit.append(ui->txt_baoneifirst);
    txtAlarmLimit.append(ui->txt_waihuanfirst);
    txtAlarmLimit.append(ui->txt_neihuanfirst);
    txtAlarmLimit.append(ui->txt_gundanfirst);
    txtAlarmLimit.append(ui->txt_gunshuangfirst);
    txtAlarmLimit.append(ui->txt_tamianfirst);
    txtAlarmLimit.append(ui->txt_benzhoufirst);
    txtAlarmLimit.append(ui->txt_linzhoufirst);
    txtAlarmLimit.append(ui->txt_baowaisecond);
    txtAlarmLimit.append(ui->txt_baoneisecond);
    txtAlarmLimit.append(ui->txt_waihuansecond);
    txtAlarmLimit.append(ui->txt_neihuansecond);
    txtAlarmLimit.append(ui->txt_gundansecond);
    txtAlarmLimit.append(ui->txt_gunshuangsecond);
    txtAlarmLimit.append(ui->txt_tamiansecond);
    txtAlarmLimit.append(ui->txt_benzhousecond);
    txtAlarmLimit.append(ui->txt_linzhousecond);
    txtAlarmLimit.append(ui->txt_absolutetem);
    txtAlarmLimit.append(ui->txt_relativetem);
    txtAlarmLimit.append(ui->txt_temwarning);

    txtAlarmLimit.append(ui->txt_RmsAx_first);
    txtAlarmLimit.append(ui->txt_RmsGe_first);
    txtAlarmLimit.append(ui->txt_RmsMo_first);
    txtAlarmLimit.append(ui->txt_RmsAx_second);
    txtAlarmLimit.append(ui->txt_RmsGe_second);
    txtAlarmLimit.append(ui->txt_RmsMo_second);
    txtAlarmLimit.append(ui->txt_PPAx_first);
    txtAlarmLimit.append(ui->txt_PPGe_first);
    txtAlarmLimit.append(ui->txt_PPMo_first);
    txtAlarmLimit.append(ui->txt_PPAx_second);
    txtAlarmLimit.append(ui->txt_PPGe_second);
    txtAlarmLimit.append(ui->txt_PPMo_second);


    for(auto txt : txtAlarmLimit){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setProperty("lastText","Null");
        connect(txt,&QLineEdit::editingFinished,this,&FrmRMAlarmLimit::DataChange);
    }

    switchbtns.append(ui->widget_usedimensional);
    switchbtns.append(ui->widget_usevibwarning);
    switchbtns.append(ui->widget_usevibfirst);
    switchbtns.append(ui->widget_usevibsecond);
    switchbtns.append(ui->widget_usetem);
    switchbtns.append(ui->widget_useoffline);
    switchbtns.append(ui->widget_useexternalspeed);

    for(auto btn : switchbtns){
        btn->setChecked(false);
        connect(btn,&ToggleSwitch::toggled,[=](){NeedSave = true;});
    }

    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::Alarmlimit,this,&FrmRMAlarmLimit::UDPDataReturn);
}

void FrmRMAlarmLimit::RefreshData()
{
    if(alarmlimitinfo.isEmpty() || (alarmlimitinfo.size() != txtAlarmLimit.size())) return;

    for(int i=0;i<alarmlimitinfo.size();i++){
        txtAlarmLimit.at(i)->setText(alarmlimitinfo.at(i));
        txtAlarmLimit.at(i)->setProperty("lastText",alarmlimitinfo.at(i));
    }

    //状态栏
    for(int i=0;i<switchbtns.size();i++){
        switchbtns.at(i)->setChecked(btnsstate.at(i));
    }
}

QString FrmRMAlarmLimit::GetCurrentWagon() const
{
    return this->Wagon;
}

void FrmRMAlarmLimit::SetCurrentWagon(const QString &wagon)
{
    for(auto txt : txtAlarmLimit){
        txt->clear();
    }
    alarmlimitinfo.clear();
    for(int i=0;i<txtAlarmLimit.size();i++){
        txtAlarmLimit.at(i)->setProperty("lastText","Null");
    }
    this->Wagon = wagon;
    UDPMulticastAPI::Instance()->GetAlarmLimit(Wagon);
}

void FrmRMAlarmLimit::SaveData()
{
    if(!NeedSave) return;

    if(QMessageBox::question(this,"保存","是否将改变内容提交") == QMessageBox::No){
        NeedSave = false;
    }else{
        on_btn_save_clicked();
    }
}

void FrmRMAlarmLimit::RedrawData()
{
    RefreshData();
}

void FrmRMAlarmLimit::on_btn_save_clicked()
{
    bool isempty = false;
    for( auto txt : txtAlarmLimit){
        if(txt->text().isEmpty()){
            isempty = true;
            break;
        }
    }

    if(isempty){
        QMessageBox::warning(this,"提示","信息填写不完整");
        return;
    }

    QStringList valuelist;
    for( auto txt : txtAlarmLimit){
        valuelist.append(txt->text());
    }
    for(auto btn : switchbtns){
        valuelist.append(QString::number(btn->isChecked()));
    }
    UDPMulticastAPI::Instance()->UpdateAlarmLimit(Wagon,valuelist);
    NeedSave = false;
    CoreHelper::showFadingMessage(this,"已发送提交");
}

void FrmRMAlarmLimit::DataChange()
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

void FrmRMAlarmLimit::UDPDataReturn(QString wagon, QStringList linklist)
{
    if((wagon != this->Wagon) ||linklist.size() < 40){
        return;
    }
    alarmlimitinfo.clear();
    btnsstate.clear();
    //后7位是状态字节
    QStringList lastSix = linklist.mid(linklist.size() - 7, 7); // 取最后7个元素
    for (const QString &str : lastSix) {
        btnsstate.append(str.toInt() != 0);
    }
    //剩下的是报警阈值
    QStringList remaining = linklist.mid(0, linklist.size() - 7);
    alarmlimitinfo = remaining;
    RefreshData();
}
