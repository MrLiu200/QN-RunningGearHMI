#include "frmrmassociation.h"
#include "ui_frmrmassociation.h"
#include "columndelegate.h"
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "corehelper.h"
FrmRMAssociation::FrmRMAssociation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMAssociation)
{
    ui->setupUi(this);
    Wagon = "";
    NeedSave = false;
    NeedInit = true;
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::linkconfigrereturn,this,&FrmRMAssociation::UDPDataReturn);
}

FrmRMAssociation::~FrmRMAssociation()
{
    delete ui;
}

void FrmRMAssociation::InitTable()
{
    NeedInit = false;
    columnNames.clear();
    columnWidths.clear();

    columnNames << "车厢类型" << "ip地址" << "Tcp监听端口" << "车厢号";
    columnWidths << 150 << 150 << 120 << 120;

    model = new QStandardItemModel(this);

    //填充数据
    for(int i=0;i<AssociationDevice.size();i++){
        QStringList temp = AssociationDevice.at(i).split("|");
        if(temp.size() != columnWidths.size()){
            continue;
        }
        //将主机的命名进行更改
        QString oldtype = temp.at(0);
        QString newtype = "错误类型";
        if(oldtype == "Host"){
            newtype = "主机";
        }else if(oldtype == "StandbyHost"){
            newtype = "备用主机";
        }else if(oldtype == "Slave"){
            newtype = "从机";
        }
        model->setItem(i, 0, new QStandardItem(newtype));
        model->setItem(i, 1, new QStandardItem(temp.at(1)));
        model->setItem(i, 2, new QStandardItem(temp.at(2)));
        model->setItem(i, 3, new QStandardItem(temp.at(3)));
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

    //车厢类型委托
    QStringList wagontypelist;
    wagontypelist << "主机" << "备用主机" << "从机";
    ui->tablemain->setItemDelegateForColumn(0, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, wagontypelist, this));

    connect(model,&QStandardItemModel::dataChanged,this,&FrmRMAssociation::DataChange);
}

void FrmRMAssociation::RefreshData()
{
    if(NeedInit) return;
    disconnect(model,&QStandardItemModel::dataChanged,this,&FrmRMAssociation::DataChange);

    //清空
    if(model->rowCount() > 0){
        model->removeRows(0, model->rowCount());
    }

    //填充数据
    for(int i=0;i<AssociationDevice.size();i++){
        QStringList temp = AssociationDevice.at(i).split("|");
        if(temp.size() != columnWidths.size()){
            continue;
        }
        //将主机的命名进行更改
        QString oldtype = temp.at(0);
        QString newtype = "错误类型";
        if(oldtype == "Host"){
            newtype = "主机";
        }else if(oldtype == "StandbyHost"){
            newtype = "备用主机";
        }else if(oldtype == "Slave"){
            newtype = "从机";
        }
        model->setItem(i, 0, new QStandardItem(newtype));
        model->setItem(i, 1, new QStandardItem(temp.at(1)));
        model->setItem(i, 2, new QStandardItem(temp.at(2)));
        model->setItem(i, 3, new QStandardItem(temp.at(3)));
    }

    connect(model,&QStandardItemModel::dataChanged,this,&FrmRMAssociation::DataChange);
}

QString FrmRMAssociation::GetCurrentWagon() const
{
    return this->Wagon;
}

void FrmRMAssociation::SetCurrentWagon(const QString &wagon)
{
    if(!NeedInit){
        if(model->rowCount() > 0){
            model->removeRows(0, model->rowCount());
        }
    }
    AssociationDevice.clear();
    this->Wagon = wagon;
    UDPMulticastAPI::Instance()->GetLinkDeviceInfo(Wagon);
}

void FrmRMAssociation::SaveData()
{
    if(!NeedSave) return;

    if(QMessageBox::question(this,"保存","是否将改变内容提交") == QMessageBox::No){
        NeedSave = false;
    }else{
        on_btn_save_clicked();
    }
}

void FrmRMAssociation::RedrawData()
{
    RefreshData();
}

void FrmRMAssociation::on_btn_add_clicked()
{
    int count = model->rowCount();
    model->insertRow(count);

    QString type = "从机";
    QString ip = "192.168.xxx.xxx";
    int port = 6000;
    QString wagonname = "xxxx";

    //获取端口信息，因为端口一般是一致的
    if(count > 0){
        port = model->data(model->index(count-1,1)).toInt();
    }

    //添加新行数据
    model->setData(model->index(count, 0), type);
    model->setData(model->index(count, 1), ip);
    model->setData(model->index(count, 2), QString::number(port));
    model->setData(model->index(count, 3), wagonname);
}

void FrmRMAssociation::on_btn_delete_clicked()
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

void FrmRMAssociation::on_btn_clear_clicked()
{
    if(model->rowCount() <= 0){
        return;
    }
    if(QMessageBox::question(this,"清空","确认清空所有测点信息？") == QMessageBox::Yes){
        model->removeRows(0, model->rowCount());
    }
}

void FrmRMAssociation::on_btn_save_clicked()
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
    UDPMulticastAPI::Instance()->TestAddPre(Wagon,"LinkDeviceInfo");
    //循环读取内容并发送添加指令
    QStringList rowlist;
    for(int row = 0;row < model->rowCount(); ++row){
        rowlist.clear();
        bool validRow = true;
        for(int col = 0; col < model->columnCount(); ++col){
            QVariant value = model->data(model->index(row, col), Qt::EditRole);

            if (!value.isValid() || value.toString().trimmed().isEmpty()) {
                QMessageBox::warning(this,"关联设备",QString("第%1行第%2列数据为空，跳过该行").arg(row).arg(col));
                validRow = false;
                break;
            }
            rowlist.append(value.toString());
        }
        if (!validRow) continue;
        UDPMulticastAPI::Instance()->UpdateLinkDevice(Wagon,0,rowlist.at(0),
                                                   rowlist.at(1),rowlist.at(2).toInt(),
                                                   rowlist.at(3));
    }
    NeedSave = false;
    CoreHelper::showFadingMessage(this,"已发送提交");

}

void FrmRMAssociation::DataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    NeedSave = true;
}

void FrmRMAssociation::UDPDataReturn(QString wagon, QStringList linklist)
{
    if(wagon == Wagon){
        AssociationDevice.clear();
        AssociationDevice = linklist;
        if(NeedInit){
            InitTable();
        }else{
            RefreshData();
        }
    }
}
