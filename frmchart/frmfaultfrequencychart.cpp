#include "frmfaultfrequencychart.h"
#include "ui_frmfaultfrequencychart.h"
#include "predata.h"
#include <QtMath>
#include <QMessageBox>
#include "frmblockinginterface.h"
FrmFaultFrequencyChart::FrmFaultFrequencyChart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmFaultFrequencyChart)
{
    ui->setupUi(this);
    NeedInit = true;
    setWindowTitle("故障频率");
    connect(ui->lineEdit,&QLineEdit::editingFinished,this,&FrmFaultFrequencyChart::SetSimulateSpeedRev);
    connect(ui->tree_Rotating,&QTreeWidget::itemChanged,this,&FrmFaultFrequencyChart::onItemChanged);
    connect(ui->tree_GearMesh,&QTreeWidget::itemChanged,this,&FrmFaultFrequencyChart::onItemChanged);
    connect(ui->tree_Bearing,&QTreeWidget::itemChanged,this,&FrmFaultFrequencyChart::onItemChanged);
}

FrmFaultFrequencyChart::~FrmFaultFrequencyChart()
{
    delete ui;
}

void FrmFaultFrequencyChart::closeEvent(QCloseEvent *event)
{
    Q_EMIT closedSignal();
    QWidget::closeEvent(event);
}

void FrmFaultFrequencyChart::treewidget_init()
{
    TreeGearMesh_Init();
    TreeBearing_Init();
    TreeRotating_Init();
    ui->lineEdit->setEnabled(false);
}

void FrmFaultFrequencyChart::TreeGearMesh_Init()
{
    QTreeWidget *widget = ui->tree_GearMesh;
    widget->clear();
    widget->setHeaderHidden(true);
    widget->blockSignals(true);
    //设置top Item
    QTreeWidgetItem *topItem = new QTreeWidgetItem(widget);
    topItem->setText(0,"啮合频率");

    //设置二级Item
    QTreeWidgetItem *GearFrequency = new QTreeWidgetItem(topItem);
    GearFrequency->setText(0,"齿轮");
    GearFrequency->setCheckState(0,Qt::Unchecked);
    GearFrequency->setData(0,Qt::UserRole,"child");

    //设置三级Item
    QTreeWidgetItem *FrequencyValue = new QTreeWidgetItem(GearFrequency);
    QString text = QString("(%1) %2Hz").arg(GearMeshFrequencyValues.at(0).first).arg(GearMeshFrequencyValues.at(0).second);
    FrequencyValue->setText(0,text);
    FrequencyValue->setData(0,Qt::UserRole,GearMeshFrequencyValues.at(0).first);
    FrequencyValue->setData(1,Qt::UserRole,GearMeshFrequencyValues.at(0).second);
    FrequencyValue->setFlags(FrequencyValue->flags() & ~Qt::ItemIsUserCheckable);
    GearMeshItems.append(FrequencyValue);

//    connect(widget,&QTreeWidget::itemChanged,this,&FrmFaultFrequencyChart::onItemChanged);
    widget->blockSignals(false);
}

void FrmFaultFrequencyChart::TreeBearing_Init()
{
    //每个轴承有5个故障
    QStringList faultStr;
    faultStr << "保外" << "保内" << "内环" << "外环" << "滚单";

    QTreeWidget *widget = ui->tree_Bearing;
    widget->clear();
    widget->setHeaderHidden(true);
    widget->blockSignals(true);
    //top
    QTreeWidgetItem *topItem = new QTreeWidgetItem(widget);
    topItem->setText(0,"轴承通过频率");

    int valueindex = 0;
    for(int i=0;i<BearingModelList.size();i++){
        //2级菜单，每个轴承名称
        QTreeWidgetItem *BearingModel = new QTreeWidgetItem(topItem);
        BearingModel->setText(0,BearingModelList.at(i));
        BearingModel->setCheckState(0,Qt::Unchecked);
        BearingModel->setData(0,Qt::UserRole,"father");

        //3级菜单，故障名称
        for(int j=0;j<faultStr.size();j++){
            QTreeWidgetItem *fault = new QTreeWidgetItem(BearingModel);
            fault->setText(0,faultStr.at(j));
            fault->setCheckState(0,Qt::Unchecked);
            fault->setData(0,Qt::UserRole,"child");

            //4级菜单
            QTreeWidgetItem *value = new QTreeWidgetItem(fault);
            //拼接text
            QString text = QString("(%1) %2Hz").arg(BearingFrequencyValue.at(valueindex).first).arg(BearingFrequencyValue.at(valueindex).second);
            value->setText(0,text);
            value->setFlags(fault->flags() & ~Qt::ItemIsUserCheckable);
            value->setData(0,Qt::UserRole,BearingFrequencyValue.at(valueindex).first);
            value->setData(1,Qt::UserRole,BearingFrequencyValue.at(valueindex).second);
            BearingItems.append(value);
            valueindex++;
        }
    }
//    connect(widget,&QTreeWidget::itemChanged,this,&FrmFaultFrequencyChart::onItemChanged);
    widget->blockSignals(false);
}

void FrmFaultFrequencyChart::TreeRotating_Init()
{
    QStringList typelist;
    typelist << "踏面" << "大齿轮" << "小齿轮";
    QTreeWidget *widget = ui->tree_Rotating;
    widget->clear();
    widget->setHeaderHidden(true);
    widget->blockSignals(true);
    //top
    QTreeWidgetItem *topItem = new QTreeWidgetItem(widget);
    topItem->setText(0,"转频");


    for(int i=0;i<typelist.size();i++){
        //二级菜单，类型
        QTreeWidgetItem *rotatingtype = new QTreeWidgetItem(topItem);
        rotatingtype->setText(0,typelist.at(i));
        rotatingtype->setCheckState(0,Qt::Unchecked);
        rotatingtype->setData(0,Qt::UserRole,"child");

        //三级菜单，值
        QTreeWidgetItem *value = new QTreeWidgetItem(rotatingtype);
        //拼接text
        QString text = QString("(%1) %2Hz").arg(BearingFrequencyValue.at(i).first).arg(BearingFrequencyValue.at(i).second);
        value->setText(0,text);
        value->setFlags(value->flags() & ~Qt::ItemIsUserCheckable);
        value->setData(0,Qt::UserRole,BearingFrequencyValue.at(i).first);
        value->setData(1,Qt::UserRole,BearingFrequencyValue.at(i).second);
        RotatingItems.append(value);
    }
    widget->blockSignals(false);

}

void FrmFaultFrequencyChart::DataInit()
{
    if(Wagon.isEmpty() || RawData.isEmpty() || BearingModelList.isEmpty()) return;

    double SpeedRatio = 0.00;//转速比
    int BullGear = 0;
    //找出车厢类型
    for(auto device : PreData::devicelist){
        if((device.Wagon == Wagon) && (device.id == ID) && (device.ch == CH)){
            this->wagonType = device.type;
            SpeedRatio = (double)device.drivenwheelTeethNum / device.capstanTeethNum;
            BullGear = device.drivenwheelTeethNum;
            break;
        }
    }
    if(this->wagonType.isEmpty()) return;
    //找出bearing对应参数
    QList<QVector<double>> BearingParameter;
    for(int i=0;i<BearingModelList.size();i++){
        QVector<double> parameter = QueryBearingParameters(BearingModelList.at(i));
        if(!parameter.isEmpty()){
            BearingParameter.append(parameter);
        }
    }
    if(BearingParameter.size() != BearingModelList.size()) return;

    //根据测点类型，计算出高速轴和低速轴转速
    double Lspeed = 0.00,Hspeed = 0.00;
    if(wagonType == "齿轮箱"){
        Lspeed = (double)SpeedRev/60;
        Hspeed = Lspeed * SpeedRatio;
    }else if(wagonType == "轴箱"){
        Lspeed = (double)SpeedRev/60;
        Hspeed = Lspeed;
    }else{
        Lspeed = (double)SpeedRev * SpeedRatio / 60;
        Hspeed = Lspeed;
    }
    //计算出轴承通过频率
    BearingFrequencyValue.clear();
    int index = 1;
    for(auto parameter : BearingParameter){
        if(parameter.size() != 4) continue;
        double ftemp = 0.0;
        double MidDia = parameter.at(0);    //中径
        double RollerDia = parameter.at(1); //滚径
        double RollerAmount = parameter.at(2); //滚子数
        double Angle = parameter.at(3);     //接触角
        ftemp = RollerAmount * cos(Angle);  //滚径*轴承接触角的余弦值

        double baowai = (MidDia - ftemp) / (2.0 * MidDia) * Lspeed;
        double baonei = (MidDia + ftemp) / (2.0 * MidDia) * Lspeed;
        double waihuan = baowai * RollerAmount;
        double neihuan  = baonei * RollerAmount;
        double gundan = (MidDia * MidDia) - (ftemp * ftemp) / (2.0 * MidDia * RollerDia) * Lspeed;
        BearingFrequencyValue.append(qMakePair(QString("BPFO.%1").arg(index),baowai));
        BearingFrequencyValue.append(qMakePair(QString("BPFI.%1").arg(index),baonei));
        BearingFrequencyValue.append(qMakePair(QString("BFS.%1").arg(index),waihuan));
        BearingFrequencyValue.append(qMakePair(QString("BFS.%1_1").arg(index),neihuan));
        BearingFrequencyValue.append(qMakePair(QString("TFT.%1").arg(index),gundan));
        index++;
    }

    //转频
    RotatingFrequencyValue.clear();
    RotatingFrequencyValue.append(qMakePair(QString("F.1"),Lspeed));    //踏面
    RotatingFrequencyValue.append(qMakePair(QString("B.1"),Lspeed));    //大齿轮
    RotatingFrequencyValue.append(qMakePair(QString("S.1"),Hspeed));    //小齿轮

    //啮合频率
    GearMeshFrequencyValues.clear();
    GearMeshFrequencyValues.append(qMakePair(QString("GMF.1"),Lspeed * BullGear));//大小齿轮啮合频率相同，所在轴转频*齿数（大齿轮）


}

QVector<double> FrmFaultFrequencyChart::QueryBearingParameters(QString model)
{
    //数据顺序：中径、滚径、滚子数、接触角
    QVector<double> retParameter;
    for(auto info : PreData::bearinglist){
        if((info.Wagon == Wagon) && (info.Model == model)){
            retParameter.append(info.PitchDiameter);
            retParameter.append(info.RollingDiameter);
            retParameter.append(info.RollerNum);
            retParameter.append(info.ContactAngle);
            break;
        }
    }
    return retParameter;
}

void FrmFaultFrequencyChart::flushData()
{
    for(int i=0;i<GearMeshItems.size();i++){//啮合频率
        QString text = QString("(%1) %2Hz").arg(GearMeshFrequencyValues.at(0).first).arg(GearMeshFrequencyValues.at(0).second);
        Qt::CheckState oldstate =  GearMeshItems.at(0)->parent()->checkState(0);
        if(oldstate == Qt::Checked){
            GearMeshItems.at(0)->parent()->setCheckState(0,Qt::Unchecked);
        }
        GearMeshItems.at(0)->setText(0,text);
        GearMeshItems.at(0)->setData(0,Qt::UserRole,GearMeshFrequencyValues.at(0).first);
        GearMeshItems.at(0)->setData(1,Qt::UserRole,GearMeshFrequencyValues.at(0).second);
        if(oldstate == Qt::Checked){
            GearMeshItems.at(0)->parent()->setCheckState(0,Qt::Checked);
        }
    }

    int len = BearingItems.size() >= BearingFrequencyValue.size() ? BearingFrequencyValue.size() : BearingItems.size();
    for(int i=0;i<len;i++){
        QString text = QString("(%1) %2Hz").arg(BearingFrequencyValue.at(i).first).arg(BearingFrequencyValue.at(i).second);
        Qt::CheckState oldstate =  BearingItems.at(0)->parent()->checkState(0);
        if(oldstate == Qt::Checked){
            BearingItems.at(i)->parent()->setCheckState(0,Qt::Unchecked);
        }
        BearingItems.at(i)->setText(0,text);
        BearingItems.at(i)->setData(0,Qt::UserRole,BearingFrequencyValue.at(i).first);
        BearingItems.at(i)->setData(1,Qt::UserRole,BearingFrequencyValue.at(i).second);
        if(oldstate == Qt::Checked){
            BearingItems.at(i)->parent()->setCheckState(0,Qt::Checked);
        }
    }

    len = RotatingItems.size() >= RotatingFrequencyValue.size() ? RotatingFrequencyValue.size() : RotatingItems.size();
    for(int i=0;i<len;i++){
        QString text = QString("(%1) %2Hz").arg(RotatingFrequencyValue.at(i).first).arg(RotatingFrequencyValue.at(i).second);
        Qt::CheckState oldstate =  RotatingItems.at(0)->parent()->checkState(0);
        if(oldstate == Qt::Checked){
            RotatingItems.at(i)->parent()->setCheckState(0,Qt::Unchecked);
        }
        RotatingItems.at(i)->setText(0,text);
        RotatingItems.at(i)->setData(0,Qt::UserRole,RotatingFrequencyValue.at(i).first);
        RotatingItems.at(i)->setData(1,Qt::UserRole,RotatingFrequencyValue.at(i).second);
        if(oldstate == Qt::Checked){
            RotatingItems.at(i)->parent()->setCheckState(0,Qt::Checked);
        }
    }
}

void FrmFaultFrequencyChart::SetDataInfo(const QString wagon, const quint8 id, const quint8 ch, const uint32_t speed, const QString bearingModels, const QVector<double> data)
{
    this->Wagon = wagon;
    this->RawData = data;
    this->BearingModelList = bearingModels.split("|");
    this->ID = id;
    this->CH = ch;
    this->SpeedRev = speed;

    ui->lineEdit->setText(QString("%1").arg(SpeedRev));
    DataInit();
    //图形界面更新
    treewidget_init();
}

void FrmFaultFrequencyChart::UpdateInfo(const uint32_t speed, const QVector<double> data)
{
    this->RawData = data;
    this->SpeedRev = speed;
    ui->lineEdit->setText(QString("%1").arg(SpeedRev));
    DataInit();
    flushData();
}

void FrmFaultFrequencyChart::onItemChanged(QTreeWidgetItem *item, int column)
{
    QString data = item->data(column,Qt::UserRole).toString();
    Qt::CheckState checkstate = item->checkState(column);
    bool isChecked = (checkstate == Qt::Checked);
    item->setExpanded(isChecked);
    if(data == "father"){
        //遍历出其下所有的child，选中并提取出其中的信号
       QTreeWidget *widget = item->treeWidget();
       widget->blockSignals(true);
        for(int i=0;i<item->childCount();i++){
            QTreeWidgetItem *childitem = item->child(i);
            childitem->setCheckState(0,checkstate);
            childitem->setExpanded(isChecked);
            //取出childitem之下的item的data，并触发信号
            QString key = childitem->child(0)->data(0,Qt::UserRole).toString();
            double value = childitem->child(0)->data(1,Qt::UserRole).toDouble();
            if (!std::isnan(value) && std::isfinite(value)) {
                Q_EMIT UserChecked(key, value, isChecked);
            }
        }
         widget->blockSignals(false);
    }else if(data == "child"){
        //下面只有一个child。取出并触发信号
        QString key = item->child(0)->data(0,Qt::UserRole).toString();
        double value = item->child(0)->data(1,Qt::UserRole).toDouble();
        if (!std::isnan(value) && std::isfinite(value)) {
            Q_EMIT UserChecked(key, value, isChecked);
        }
    }
}

void FrmFaultFrequencyChart::SetSimulateSpeedRev()
{
    ui->lineEdit->clearFocus();
    bool Isdouble;
    quint32 newspeed = ui->lineEdit->text().toUInt(&Isdouble);
    if(!Isdouble){
        QMessageBox::warning(nullptr,"模拟转速","请输入整数");
        return;
    }
    this->SpeedRev = newspeed;
    DataInit();
    flushData();
}

void FrmFaultFrequencyChart::on_checkBox_toggled(bool checked)
{
    ui->lineEdit->setEnabled(checked);
}
