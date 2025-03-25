#include "frmrmmeasurepoint.h"
#include "ui_frmrmmeasurepoint.h"
#include "columndelegate.h"
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "corehelper.h"
FrmRMMeasurePoint::FrmRMMeasurePoint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmRMMeasurePoint)
{
    ui->setupUi(this);
    Wagon = "";
    NeedSave = false;
    NeedInit = true;
}

FrmRMMeasurePoint::~FrmRMMeasurePoint()
{
    delete ui;
}

void FrmRMMeasurePoint::InitTable()
{
    NeedInit = false;
    columnNames.clear();
    columnWidths.clear();

    columnNames << "前置ID" << "通道号" << "测点名称" << "测点类型" << "灵敏度" << "轴径" << "轴承1名称"
                << "轴承1型号" << "轴承2名称" << "轴承2型号" << "轴承3名称" << "轴承3型号"
                << "轴承4名称" << "轴承4型号" << "从动轮名称" << "从动轮齿数" << "主动轮名称"
                << "主动轮齿数" << "软件版本" << "是否使能";
    columnWidths << 50 << 50 << 80 << 80 << 50 << 50 << 120 << 120
                 << 120 << 120 << 120 << 120 << 120 << 120 << 120 << 80
                 << 120 << 80 << 80 << 50;
//    columnWidths << 70 << 70 << 90 << 120 << 70 << 50 << 90 << 150
//                 << 90 << 150 << 90 << 150 << 90 << 150 << 90 << 90
//                 << 90 << 90 << 80 << 50;

    model = new QStandardItemModel(this);

    //获取前置处理器索引
    QList<int> preindexlist;
    for(int i=0;i<PreData::devicelist.size();i++){
        if(Wagon == PreData::devicelist.at(i).Wagon){
            preindexlist.append(i);
        }
    }

    //填充数据和表头
    for(int i=0;i<preindexlist.size();i++){
        auto device = PreData::devicelist.at(preindexlist.at(i));
        model->setItem(i, 0, new QStandardItem(QString::number(device.id)));
        model->setItem(i, 1, new QStandardItem(QString::number(device.ch)));
        model->setItem(i, 2, new QStandardItem(device.name));
        model->setItem(i, 3, new QStandardItem(device.type));
        model->setItem(i, 4, new QStandardItem(QString::number(device.sensitivity)));
        model->setItem(i, 5, new QStandardItem(QString::number(device.shaftDiameter)));
        model->setItem(i, 6, new QStandardItem(device.bearing1_name));
        model->setItem(i, 7, new QStandardItem(device.bearing1_model));
        model->setItem(i, 8, new QStandardItem(device.bearing2_name));
        model->setItem(i, 9, new QStandardItem(device.bearing2_model));
        model->setItem(i, 10, new QStandardItem(device.bearing3_name));
        model->setItem(i, 11, new QStandardItem(device.bearing3_model));
        model->setItem(i, 12, new QStandardItem(device.bearing4_name));
        model->setItem(i, 13, new QStandardItem(device.bearing4_model));
        model->setItem(i, 14, new QStandardItem(device.capstanname));
        model->setItem(i, 15, new QStandardItem(QString::number(device.capstanTeethNum)));
        model->setItem(i, 16, new QStandardItem(device.drivenwheelName));
        model->setItem(i, 17, new QStandardItem(QString::number(device.drivenwheelTeethNum)));
        model->setItem(i, 18, new QStandardItem(device.version));
        model->setItem(i, 19, new QStandardItem(device.IsEnable ? "Enabled" : "Disabled"));
    }
    ui->tablemain->setModel(model);
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

    //id 类型委托
    QStringList idlist;
    idlist << "1" << "2" << "3" << "4";
    ui->tablemain->setItemDelegateForColumn(0, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, idlist, this));
    //ch 委托
    QStringList chlist;
    chlist << "1" << "2" << "3" << "4" << "5";
    ui->tablemain->setItemDelegateForColumn(1, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, chlist, this));
    //测点类型委托
    QStringList pointtypelist;
    pointtypelist << "轴箱" << "电机传动端" << "电机非传动端" << "齿轮箱";
    ui->tablemain->setItemDelegateForColumn(3, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, pointtypelist, this));
    //轴承委托
    QStringList BearingList;
    for(int i=0;i<PreData::bearinglist.size();i++){
        if(Wagon == PreData::bearinglist.at(i).Wagon){
            BearingList.append(PreData::bearinglist.at(i).Model);
        }
    }
    ui->tablemain->setItemDelegateForColumn(7, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, BearingList, this));
    ui->tablemain->setItemDelegateForColumn(9, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, BearingList, this));
    ui->tablemain->setItemDelegateForColumn(11, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, BearingList, this));
    ui->tablemain->setItemDelegateForColumn(13, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, BearingList, this));

    //使能开关
    QStringList enablelist;
    enablelist << "Disable" << "Enable";
    ui->tablemain->setItemDelegateForColumn(19, new ColumnDelegate(ColumnDelegate::Delegate_ComboBox, enablelist, this));

    connect(model,&QStandardItemModel::dataChanged,this,&FrmRMMeasurePoint::DataChange);
}

void FrmRMMeasurePoint::RefreshData()
{
    disconnect(model,&QStandardItemModel::dataChanged,this,&FrmRMMeasurePoint::DataChange);
    //先清空所有内容
    if(model->rowCount() > 0){
        model->removeRows(0, model->rowCount());
    }

    //获取前置处理器索引
    QList<int> preindexlist;
    for(int i=0;i<PreData::devicelist.size();i++){
        if(Wagon == PreData::devicelist.at(i).Wagon){
            preindexlist.append(i);
        }
    }

    //填充数据
    for(int i=0;i<preindexlist.size();i++){
        auto device = PreData::devicelist.at(preindexlist.at(i));
        model->setItem(i, 0, new QStandardItem(QString::number(device.id)));
        model->setItem(i, 1, new QStandardItem(QString::number(device.ch)));
        model->setItem(i, 2, new QStandardItem(device.name));
        model->setItem(i, 3, new QStandardItem(device.type));
        model->setItem(i, 4, new QStandardItem(QString::number(device.sensitivity)));
        model->setItem(i, 5, new QStandardItem(QString::number(device.shaftDiameter)));
        model->setItem(i, 6, new QStandardItem(device.bearing1_name));
        model->setItem(i, 7, new QStandardItem(device.bearing1_model));
        model->setItem(i, 8, new QStandardItem(device.bearing2_name));
        model->setItem(i, 9, new QStandardItem(device.bearing2_model));
        model->setItem(i, 10, new QStandardItem(device.bearing3_name));
        model->setItem(i, 11, new QStandardItem(device.bearing3_model));
        model->setItem(i, 12, new QStandardItem(device.bearing4_name));
        model->setItem(i, 13, new QStandardItem(device.bearing4_model));
        model->setItem(i, 14, new QStandardItem(device.capstanname));
        model->setItem(i, 15, new QStandardItem(QString::number(device.capstanTeethNum)));
        model->setItem(i, 16, new QStandardItem(device.drivenwheelName));
        model->setItem(i, 17, new QStandardItem(QString::number(device.drivenwheelTeethNum)));
        model->setItem(i, 18, new QStandardItem(device.version));
        model->setItem(i, 19, new QStandardItem(device.IsEnable ? "Enabled" : "Disabled"));
    }
    connect(model,&QStandardItemModel::dataChanged,this,&FrmRMMeasurePoint::DataChange);
}

void FrmRMMeasurePoint::linshifun(QString str)
{
    // 将 QByteArray 转换为 QString
    QString jsonString = str;
    qDebug() << "Received JSON String:\n" << jsonString;

    // 解析 JSON 字符串
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!jsonDoc.isNull()) {
        if (jsonDoc.isArray()) {
            QJsonArray jsonArray = jsonDoc.array();
            qDebug() << "Received JSON Array:";

            for (const QJsonValue &value : jsonArray) {
                QJsonObject obj = value.toObject();
                qDebug() << "Parsed JSON Object:";

                for (QString key : obj.keys()) {
                    qDebug() << key << ":" << obj.value(key).toString();
                }
            }
        } else if (jsonDoc.isObject()) {
            QJsonObject jsonObject = jsonDoc.object();
            qDebug() << "Received JSON Object:";

            for (QString key : jsonObject.keys()) {
                qDebug() << key << ":" << jsonObject.value(key).toVariant();
            }
        }
    } else {
        qDebug() << "Invalid JSON received";
    }
}

QString FrmRMMeasurePoint::GetCurrentWagon() const
{
    return this->Wagon;
}

void FrmRMMeasurePoint::SetCurrentWagon(const QString &wagon)
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

void FrmRMMeasurePoint::SaveData()
{
    if(!NeedSave) return;

    if(QMessageBox::question(this,"保存","是否将改变内容提交") == QMessageBox::No){
        NeedSave = false;
    }else{
        on_btn_save_clicked();
    }
}

void FrmRMMeasurePoint::RedrawData()
{
    RefreshData();
}

void FrmRMMeasurePoint::on_btn_add_clicked()
{
    int count = model->rowCount();
    model->insertRow(count);

    int id = 1;
    int ch = 1;
    QString name = "ID1CH1";
    QString type = "轴箱";
    double sensitivity = 10;
    double shaftDiameter = 10;
    QString bearing1_name = "轴承1";
    QString bearing1_model = "QN-Null";
    QString bearing2_name = "轴承2";
    QString bearing2_model = "QN-Null";
    QString bearing3_name = "轴承3";
    QString bearing3_model = "QN-Null";
    QString bearing4_name = "轴承4";
    QString bearing4_model = "QN-Null";
    QString capstanname = "从动轮";
    int capstanTeethNum = 20;
    QString drivenwheelName = "主动轮";
    int drivenwheelTeethNum = 20;
    QString version = "1.0.0";
    bool IsEnable = true;

    //取出上一行的部分数据
    if(count > 0){
        id = model->data(model->index(count-1,0)).toInt();
        ch = model->data(model->index(count-1,1)).toInt() + 1;
        if(ch > 5){
            id ++;
            ch = 1;
        }
        name = QString("ID%1CH%2").arg(id).arg(ch);
        sensitivity = model->data(model->index(count-1,4)).toDouble();
        shaftDiameter = model->data(model->index(count-1,5)).toDouble();

        bearing1_model = model->data(model->index(count-1,7), Qt::EditRole).toString();
        bearing2_model = model->data(model->index(count-1,8), Qt::EditRole).toString();
        bearing3_model = model->data(model->index(count-1,11), Qt::EditRole).toString();
        bearing4_model = model->data(model->index(count-1,13), Qt::EditRole).toString();
        capstanTeethNum = model->data(model->index(count-1,15)).toInt();
        drivenwheelTeethNum = model->data(model->index(count-1,17)).toInt();
        version = model->data(model->index(count-1,18)).toString();
    }

    //添加新行数据
    model->setData(model->index(count, 0), QString::number(id), Qt::EditRole);
    model->setData(model->index(count, 1), QString::number(ch), Qt::EditRole);
    model->setData(model->index(count, 2), name);
    model->setData(model->index(count, 3), type, Qt::EditRole);
    model->setData(model->index(count, 4), QString::number(sensitivity));
    model->setData(model->index(count, 5), QString::number(shaftDiameter));
    model->setData(model->index(count, 6), bearing1_name);
    model->setData(model->index(count, 7), bearing1_model, Qt::EditRole);
    model->setData(model->index(count, 8), bearing2_name);
    model->setData(model->index(count, 9), bearing2_model, Qt::EditRole);
    model->setData(model->index(count, 10), bearing3_name);
    model->setData(model->index(count, 11), bearing3_model, Qt::EditRole);
    model->setData(model->index(count, 12), bearing4_name);
    model->setData(model->index(count, 13), bearing4_model, Qt::EditRole);
    model->setData(model->index(count, 14), capstanname);
    model->setData(model->index(count, 15), QString::number(capstanTeethNum));
    model->setData(model->index(count, 16), drivenwheelName);
    model->setData(model->index(count, 17), QString::number(drivenwheelTeethNum));
    model->setData(model->index(count, 18), version);
    model->setData(model->index(count, 19), IsEnable ? "Enabled" : "Disabled", Qt::EditRole);
}

void FrmRMMeasurePoint::on_btn_delete_clicked()
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

void FrmRMMeasurePoint::on_btn_clear_clicked()
{
    if(model->rowCount() <= 0){
        return;
    }
    if(QMessageBox::question(this,"清空","确认清空所有测点信息？") == QMessageBox::Yes){
        model->removeRows(0, model->rowCount());
    }
}

void FrmRMMeasurePoint::on_btn_setsametype_clicked()
{
    int currentrow = ui->tablemain->currentIndex().row();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","未选中任何行");
        return;
    }

    //读取选中行的类型
    QString type = model->data(model->index(currentrow,3)).toString();
    if(QMessageBox::question(this,"二次确认",QString("确认将选中行参数更新至所有 %1 测点？").arg(type)) == QMessageBox::Yes){
        // 创建提示框
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("更新中，请稍后...");
        msgBox.setStandardButtons(QMessageBox::NoButton); // 不显示按钮
        msgBox.show();
        //获取其他信息
        double sensitivity = model->data(model->index(currentrow,4)).toDouble();
        double shaftDiameter = model->data(model->index(currentrow,5)).toDouble();

        QString bearing1_model = model->data(model->index(currentrow,7), Qt::EditRole).toString();
        QString bearing2_model = model->data(model->index(currentrow,9), Qt::EditRole).toString();
        QString bearing3_model = model->data(model->index(currentrow,11), Qt::EditRole).toString();
        QString bearing4_model = model->data(model->index(currentrow,13), Qt::EditRole).toString();
        int capstanTeethNum = model->data(model->index(currentrow,15)).toInt();
        int drivenwheelTeethNum = model->data(model->index(currentrow,17)).toInt();

        //应用至同类型
        for (int row = 0; row < model->rowCount(); ++row)
        {
            if(model->data(model->index(row,3)).toString() == type){
                model->setData(model->index(row,4),sensitivity);
                model->setData(model->index(row,5),shaftDiameter);
                model->setData(model->index(row,7),bearing1_model, Qt::EditRole);
                model->setData(model->index(row,8),bearing2_model, Qt::EditRole);
                model->setData(model->index(row,11),bearing3_model, Qt::EditRole);
                model->setData(model->index(row,13),bearing4_model, Qt::EditRole);
                model->setData(model->index(row,14),capstanTeethNum);
                model->setData(model->index(row,17),drivenwheelTeethNum);
            }
        }
        msgBox.close();
    }
}

void FrmRMMeasurePoint::on_btn_setall_clicked()
{
    int currentrow = ui->tablemain->currentIndex().row();
    if(currentrow == -1){
        QMessageBox::warning(this,"提示","未选中任何行");
        return;
    }

    if(QMessageBox::question(this,"二次确认",QString("确认将选中行参数更新至所有测点？")) == QMessageBox::Yes){
        // 创建提示框
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("更新中，请稍后...");
        msgBox.setStandardButtons(QMessageBox::NoButton); // 不显示按钮
        msgBox.show();

        //获取其他信息
        double sensitivity = model->data(model->index(currentrow,4)).toDouble();
        double shaftDiameter = model->data(model->index(currentrow,5)).toDouble();

        QString bearing1_model = model->data(model->index(currentrow,7), Qt::EditRole).toString();
        QString bearing2_model = model->data(model->index(currentrow,9), Qt::EditRole).toString();
        QString bearing3_model = model->data(model->index(currentrow,11), Qt::EditRole).toString();
        QString bearing4_model = model->data(model->index(currentrow,13), Qt::EditRole).toString();
        int capstanTeethNum = model->data(model->index(currentrow,15)).toInt();
        int drivenwheelTeethNum = model->data(model->index(currentrow,17)).toInt();

        //应用至所有类型
        for (int row = 0; row < model->rowCount(); ++row)
        {
            model->setData(model->index(row,4),sensitivity);
            model->setData(model->index(row,5),shaftDiameter);
            model->setData(model->index(row,7),bearing1_model, Qt::EditRole);
            model->setData(model->index(row,8),bearing2_model, Qt::EditRole);
            model->setData(model->index(row,11),bearing3_model, Qt::EditRole);
            model->setData(model->index(row,13),bearing4_model, Qt::EditRole);
            model->setData(model->index(row,14),capstanTeethNum);
            model->setData(model->index(row,17),drivenwheelTeethNum);
        }
        msgBox.close();
    }
}

void FrmRMMeasurePoint::on_btn_save_clicked()
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
    UDPMulticastAPI::Instance()->TestAddPre(Wagon,"DeviceInfo");
    //循环读取内容并发送添加指令
    QStringList rowlist;
    for(int row = 0;row < model->rowCount(); ++row){
        rowlist.clear();
        bool validRow = true;
        for(int col = 0; col < model->columnCount(); ++col){
            QVariant value = model->data(model->index(row, col), Qt::EditRole);

            if (!value.isValid() || value.toString().trimmed().isEmpty()) {
                QMessageBox::warning(this,"测点配置",QString("第%1行第%2列数据为空，跳过该行").arg(row).arg(col));
                validRow = false;  // 处理方式：跳过整行
                break;
            }
            rowlist.append(value.toString());

            QString date = model->data(model->index(row,col),Qt::EditRole).toString();
            if(col == model->columnCount()-1){
                rowlist.append(date == "Enable" ? "1" : "0");
            }else{
                rowlist.append(date);
            }
        }
       if (!validRow) continue;
        UDPMulticastAPI::Instance()->UpdatePre(Wagon,0,rowlist);
    }
    NeedSave = false;
    CoreHelper::showFadingMessage(this,"已发送提交");
//    linshifun(jsonString);
}

void FrmRMMeasurePoint::DataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    NeedSave = true;
}
