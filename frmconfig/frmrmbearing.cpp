#include "frmrmbearing.h"
#include "ui_frmrmbearing.h"
#include "columndelegate.h"
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "corehelper.h"
FrmRMBearing::FrmRMBearing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMBearing)
{
    ui->setupUi(this);
    Wagon = "";
    NeedSave = false;
    NeedInit = true;
}

FrmRMBearing::~FrmRMBearing()
{
    delete ui;
}

void FrmRMBearing::InitTable()
{
    NeedInit = false;
    columnNames.clear();
    columnWidths.clear();

    columnNames << "型号" << "中径" << "滚径" << "滚子数" << "接触角";
    columnWidths << 150 << 120 << 120 << 120 << 120;

    model = new QStandardItemModel(this);

    //获取轴承列表
    QList<int> bearingindexlist;
    for(int i=0;i<PreData::bearinglist.size();i++){
        if(Wagon == PreData::bearinglist.at(i).Wagon){
            bearingindexlist.append(i);
        }
    }

    //填充数据
    for(int i=0;i<bearingindexlist.size();i++){
        auto bearing = PreData::bearinglist.at(bearingindexlist.at(i));
        model->setItem(i, 0, new QStandardItem(bearing.Model));
        model->setItem(i, 1, new QStandardItem(QString::number(bearing.PitchDiameter)));
        model->setItem(i, 2, new QStandardItem(QString::number(bearing.RollingDiameter)));
        model->setItem(i, 3, new QStandardItem(QString::number(bearing.RollerNum)));
        model->setItem(i, 4, new QStandardItem(QString::number(bearing.ContactAngle)));
    }
    ui->tablemain->setModel(model);
    //填充表头和列宽
    for(int i=0;i<columnNames.size();i++){
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tablemain->setColumnWidth(i,columnWidths.at(i));
    }

    ui->tablemain->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选中
    ui->tablemain->setSelectionMode(QAbstractItemView::SingleSelection);//单行选择
    ui->tablemain->setEditTriggers(QAbstractItemView::DoubleClicked);//QAbstractItemView::CurrentChanged |
    ui->tablemain->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //    ui->tablemain->horizontalHeader()->setStretchLastSection(true); // 最后一列填充完所有表格
    //    ui->tablemain->verticalHeader()->setDefaultSectionSize(30);//设置默认行高

    connect(model,&QStandardItemModel::dataChanged,this,&FrmRMBearing::DataChange);
}

void FrmRMBearing::RefreshData()
{
    disconnect(model,&QStandardItemModel::dataChanged,this,&FrmRMBearing::DataChange);
    //先清空所有内容
    if(model->rowCount() > 0){
        model->removeRows(0, model->rowCount());
    }

    //获取轴承列表
    QList<int> bearingindexlist;
    for(int i=0;i<PreData::bearinglist.size();i++){
        if(Wagon == PreData::bearinglist.at(i).Wagon){
            bearingindexlist.append(i);
        }
    }

    //填充数据
    for(int i=0;i<bearingindexlist.size();i++){
        auto bearing = PreData::bearinglist.at(bearingindexlist.at(i));
        model->setItem(i, 0, new QStandardItem(bearing.Model));
        model->setItem(i, 1, new QStandardItem(QString::number(bearing.PitchDiameter)));
        model->setItem(i, 2, new QStandardItem(QString::number(bearing.RollingDiameter)));
        model->setItem(i, 3, new QStandardItem(QString::number(bearing.RollerNum)));
        model->setItem(i, 4, new QStandardItem(QString::number(bearing.ContactAngle)));
    }

    connect(model,&QStandardItemModel::dataChanged,this,&FrmRMBearing::DataChange);
}

QString FrmRMBearing::GetCurrentWagon() const
{
    return this->Wagon;
}

void FrmRMBearing::SetCurrentWagon(const QString &wagon)
{
    if(!NeedInit){
        if(model->rowCount() > 0){
            model->removeRows(0, model->rowCount());
        }
    }
    if(Wagon.isEmpty()){
        this->Wagon = wagon;
        InitTable();
    }else{
        this->Wagon = wagon;
        RefreshData();
    }
}

void FrmRMBearing::SaveData()
{
    if(!NeedSave) return;

    if(QMessageBox::question(this,"保存","是否将改变内容提交") == QMessageBox::No){
        NeedSave = false;
    }else{
        on_btn_save_clicked();
    }
}

void FrmRMBearing::RedrawData()
{
    RefreshData();
}

void FrmRMBearing::on_btn_add_clicked()
{
    int count = model->rowCount();
    model->insertRow(count);

    QString name = "NewBearing";
    float PitchDiameter = 8.88;
    float RollingDiameter = 8.88;
    int RollerNum = 0;
    float ContactAngle = 8.88;

    //添加新行数据
    model->setData(model->index(count, 0), name);
    model->setData(model->index(count, 1), QString::number(PitchDiameter));
    model->setData(model->index(count, 2), QString::number(RollingDiameter));
    model->setData(model->index(count, 3), QString::number(RollerNum));
    model->setData(model->index(count, 4), QString::number(ContactAngle));
}

void FrmRMBearing::on_btn_delete_clicked()
{
    int row = ui->tablemain->currentIndex().row();
    if(row < 0){
        QMessageBox::warning(this,"删除","请选择删除的行");
        return;
    }

    model->removeRow(row);
    model->submit();
    ui->tablemain->setCurrentIndex(model->index(model->rowCount() - 1, 0));
}

void FrmRMBearing::on_btn_clear_clicked()
{
    if(model->rowCount() <= 0){
        return;
    }
    if(QMessageBox::question(this,"清空","确认清空所有测点信息？") == QMessageBox::Yes){
        model->removeRows(0, model->rowCount());
    }
}

void FrmRMBearing::on_btn_save_clicked()
{
#if 0
    // JSON数组
    QJsonArray jsonArray;

    // 遍历所有行
    for (int row = 0; row < model->rowCount(); ++row) {
        QJsonObject rowObject;

        // 遍历所有列
        for (int col = 0; col < model->columnCount(); ++col) {
            QVariant cellData = model->data(model->index(row, col), Qt::EditRole);
            rowObject[columnNames[col]] = QJsonValue::fromVariant(cellData);
        }

        jsonArray.append(rowObject);
    }

    // 封装成 JSON 文档
    QJsonDocument jsonDoc(jsonArray);
    QString jsonString = jsonDoc.toJson(QJsonDocument::Indented);
#endif
    //先发送清空指令
    UDPMulticastAPI::Instance()->TestAddPre(Wagon,"BearingInfo");
    //循环读取内容并发送添加指令
    QStringList rowlist;
    for(int row = 0;row < model->rowCount(); ++row){
        rowlist.clear();
        bool validRow = true;
        for(int col = 0; col < model->columnCount(); ++col){
            QVariant value = model->data(model->index(row, col), Qt::EditRole);

            if (!value.isValid() || value.toString().trimmed().isEmpty()) {
                QMessageBox::warning(this,"轴承配置",QString("第%1行第%2列数据为空，跳过该行").arg(row).arg(col));
                validRow = false;
                break;
            }
            rowlist.append(value.toString());
        }
        if (!validRow) continue;
        UDPMulticastAPI::Instance()->UpdateBearing(Wagon,0,rowlist.at(0),
                                                   rowlist.at(1).toFloat(),rowlist.at(2).toFloat(),
                                                   rowlist.at(3).toInt(),rowlist.at(4).toFloat());
    }
    NeedSave = false;
    CoreHelper::showFadingMessage(this,"已发送提交");
}

void FrmRMBearing::DataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    NeedSave = true;
}
