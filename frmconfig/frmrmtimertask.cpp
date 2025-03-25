#include "frmrmtimertask.h"
#include "ui_frmrmtimertask.h"
#include "columndelegate.h"
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
FrmRMTimertask::FrmRMTimertask(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMTimertask)
{
    ui->setupUi(this);
    Wagon = "";
    NeedSave = false;
    NeedInit = true;
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::taskReturn,this,&FrmRMTimertask::UDPDataReturn);
}

FrmRMTimertask::~FrmRMTimertask()
{
    delete ui;
}

void FrmRMTimertask::InitTable()
{
    NeedInit = false;
    columnNames.clear();
    columnWidths.clear();

    columnNames << "任务名称" << "执行时间";
    columnWidths << 300 << 300;

    model = new QStandardItemModel(this);

    //填充数据
    for(int i=0;i<Tasks.size();i++){
        QStringList temp = Tasks.at(i).split("|");
        if(temp.size() != columnWidths.size()){
            continue;
        }
        model->setItem(i, 0, new QStandardItem(temp.at(0)));
        model->setItem(i, 1, new QStandardItem(temp.at(1)));
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

    //任务类型委托
    QStringList dotasklist;
    dotasklist << "重启" << "压缩";
    ui->tablemain->setItemDelegateForColumn(0, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, dotasklist, this));

    //时间委托
    QString timeformat = "yyyy-MM-dd HH:mm:ss";
    ui->tablemain->setItemDelegateForColumn(1, new ColumnDelegate(ColumnDelegate::Delegate_DateTime, timeformat, this));

    connect(model,&QStandardItemModel::dataChanged,this,&FrmRMTimertask::DataChange);
}

void FrmRMTimertask::RefreshData()
{
    if(NeedInit) return;
    disconnect(model,&QStandardItemModel::dataChanged,this,&FrmRMTimertask::DataChange);

    //清空
    if(model->rowCount() > 0){
        model->removeRows(0, model->rowCount());
    }

    //填充数据
    for(int i=0;i<Tasks.size();i++){
        QStringList temp = Tasks.at(i).split("|");
        if(temp.size() != columnWidths.size()){
            continue;
        }
        model->setItem(i, 0, new QStandardItem(temp.at(0)));
        model->setItem(i, 1, new QStandardItem(temp.at(1)));
    }

    connect(model,&QStandardItemModel::dataChanged,this,&FrmRMTimertask::DataChange);
}

QString FrmRMTimertask::GetCurrentWagon() const
{
    return this->Wagon;
}

void FrmRMTimertask::SetCurrentWagon(const QString &wagon)
{
    if(!NeedInit){
        if(model->rowCount() > 0){
            model->removeRows(0, model->rowCount());
        }
    }
    Tasks.clear();
    this->Wagon = wagon;
    UDPMulticastAPI::Instance()->GetTimerTask(Wagon);
}

void FrmRMTimertask::SaveData()
{
    if(!NeedSave) return;

    if(QMessageBox::question(this,"保存","是否将改变内容提交") == QMessageBox::No){
        NeedSave = false;
    }else{
        on_btn_save_clicked();
    }
}

void FrmRMTimertask::RedrawData()
{
    RefreshData();
}

void FrmRMTimertask::on_btn_add_clicked()
{
    int count = model->rowCount();
    model->insertRow(count);

    QString type = "重启";
    QString time = QDateTime::currentDateTime().addDays(1).toString("yyyy-MM-dd HH:mm:ss");

    //添加新行数据
    model->setData(model->index(count, 0), type, Qt::EditRole);
    model->setData(model->index(count, 1), time, Qt::EditRole);
}

void FrmRMTimertask::on_btn_delete_clicked()
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

void FrmRMTimertask::on_btn_clear_clicked()
{
    if(model->rowCount() <= 0){
        return;
    }
    if(QMessageBox::question(this,"清空","确认清空所有测点信息？") == QMessageBox::Yes){
        model->removeRows(0, model->rowCount());
    }
}

void FrmRMTimertask::on_btn_save_clicked()
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
    qDebug()<<"提交 定时任务 内容：" << jsonString;
#endif
    //先发送清空指令
    UDPMulticastAPI::Instance()->TestAddPre(Wagon,"TimedTasksInfo");
    //循环读取内容并发送添加指令
    QStringList rowlist;
    for(int row = 0;row < model->rowCount(); ++row){
        rowlist.clear();
        bool validRow = true;
        for(int col = 0; col < model->columnCount(); ++col){
            QVariant value = model->data(model->index(row, col), Qt::EditRole);

            if (!value.isValid() || value.toString().trimmed().isEmpty()) {
                QMessageBox::warning(this,"定时任务",QString("第%1行第%2列数据为空，跳过该行").arg(row).arg(col));
                validRow = false;
                break;
            }
            rowlist.append(value.toString());
        }
        if (!validRow) continue;
        UDPMulticastAPI::Instance()->UpdateTimerTask(Wagon,0,rowlist.at(0),rowlist.at(1));
    }

    NeedSave = false;
}

void FrmRMTimertask::DataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    NeedSave = true;
}

void FrmRMTimertask::UDPDataReturn(QString wagon, QStringList linklist)
{
    if(wagon == Wagon){
        Tasks.clear();
        Tasks = linklist;
        if(NeedInit){
            InitTable();
        }else{
            RefreshData();
        }
    }
}
