#include "frmdatalog.h"
#include "ui_frmdatalog.h"
//#include "excelapi.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include "xlsxdocument.h"
frmDataLog::frmDataLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmDataLog)
{
    ui->setupUi(this);
    whereSql = "";
    InitForm();
    LoadData();
}

frmDataLog::~frmDataLog()
{
    delete ui;
}


void frmDataLog::InitForm()
{
    ui->frameTop->layout()->setMargin(6);
    ui->frameTop->layout()->setSpacing(6);

    ui->cboxcarNum->addItems(PreData::wagonsList);

    ui->cboxLogType->addItem("运行信息");
    ui->cboxLogType->addItem("用户操作");
    ui->cboxLogType->addItem("报警信息");


    ui->cboxAlarmlevel->addItem("二级报警");
    ui->cboxAlarmlevel->addItem("一级报警");
    ui->cboxAlarmlevel->addItem("预警");
    ui->cboxAlarmlevel->addItem("温度报警");
    ui->cboxAlarmlevel->addItem("主机上报");

    ui->tableMain->setAlternatingRowColors(false);
    ui->tableMain->horizontalHeader()->setStretchLastSection(true);
    ui->tableMain->verticalHeader()->setDefaultSectionSize(15);

    QStringList pageCount;

    pageCount.clear();

    pageCount.clear();
    pageCount << "20" << "25" <<"30" << "35" << "40" << "45" << "50" << "55" << "60";
    ui->cboxPageCount->addItems(pageCount);
    ui->cboxPageCount->setCurrentIndex(2);
    PageSize = 30;

    ui->dateStart->setDate(QDate::currentDate());
    ui->dateEnd->setDate(QDate::currentDate().addDays(1));

    //设置样式，先暂时放这里，后期统一管理
    QList<QToolButton *> btns1;
    QStringList tips;
    tips << "第一页" << "上一页" << "下一页" << "最后一页";
    btns1 << ui->btn_first << ui->btn_previous << ui->btn_next << ui->btn_last;
    for(int i=0;i<btns1.size();i++){
        btns1.at(i)->setStyleSheet("QToolButton {"
                                   "border: none;"
                                   "background: none;"
                                   "padding: 1px;"
                                   "}"
                                   "QToolButton:hover {"
                                   "background-color: rgb(203, 241, 255);"
                                   "}"
                                   "QToolButton:pressed {"
                                   "background-color: gray;"
                                   "}");
        btns1.at(i)->setToolTip(tips.at(i));
    }
    ui->txt_totlapage->setReadOnly(true);
    ui->txt_totlapage->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    ui->txt_currentpage->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    connect(ui->txt_currentpage,&QLineEdit::editingFinished,this,&frmDataLog::txt_currentpage_Skip);

    ui->cboxAlarmlevel->setEnabled(false);
    ui->cboxLogType->setEnabled(false);
    ui->cboxcarNum->setEnabled(false);
    ui->dateStart->setEnabled(false);
    ui->dateEnd->setEnabled(false);
}

void frmDataLog::on_btnExcel_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,"new file",qApp->applicationDirPath(),tr("Excel Files (*.xlsx)"));
    if(fileName.isEmpty()) return;
    //查询符合条件的数据库内容
    QStringList content;
    QSqlQuery query(M_DbAPI::Instance()->GetDataDBHandle());
    QString sql = QString("select * from LogInfo %1").arg(whereSql);
    query.exec(sql);


    int columnCount = query.record().count();
    QList<QString> columnNames;
    columnNames << "车号" << "车厢号" << "设备ID" << "设备通道" << "设备名称" << "日志类型" << "报警等级" << "触发时间" << "描述";
    QList<int> columnwidths;
    columnwidths << 20 << 20 << 15 << 15 << 20 << 20 << 15 << 20 << 50;

    QXlsx::Document xlsx;

    // 创建格式对象
    QXlsx::Format format;

    // 设置居中对齐HorizontalAlignment
    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);  // 水平居中
    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);    // 垂直居中
    format.setFontSize(16);
    format.setFontBold(true);

    //设置表头和列宽
    for (int col = 0; col < columnCount; ++col) {
            xlsx.write(1, col + 1, columnNames.at(col));
            xlsx.setColumnWidth(col + 1, columnwidths.at(col));
        }

    //填充数据
    int rowNum = 2;
    format.setFontBold(false);
    format.setFontSize(12);
    while (query.next()) {
        for (int i = 0; i < columnCount; ++i) {
            QString temp = query.value(i).toString();
            xlsx.write(rowNum,i+1,temp);
        }
        rowNum++;
    }
    if(xlsx.saveAs(fileName)){
        QMessageBox::information(this,"提示","导出excel成功");
        SetPromptBox("导出excel成功");
        M_DbAPI::Instance()->AddLoaclLog("导出日志",DATETIME,whereSql);
    }else{
        QMessageBox::warning(this,"导出出错",QString("文件%1 写入出错").arg(fileName));
        SetPromptBox(QString("文件%1 写入出错").arg(fileName));
    }
}

void frmDataLog::LoadData()
{
    if(!M_DbAPI::Instance()->GetDataConnectState()){
        QMessageBox::warning(nullptr,"连接错误","数据库未连接");
        ui->btnExcel->setEnabled(false);
        ui->btnSelect->setEnabled(false);
        return;
    }
    initializePagination(PageSize);
    // 默认加载第一页
//    loadPage(currentPage, PageSize);
}

void frmDataLog::on_btn_first_clicked()
{
    currentPage = 1;
    loadPage(currentPage, PageSize);
}

void frmDataLog::on_btn_previous_clicked()
{
    if (currentPage > 1) {
        currentPage--;
        loadPage(currentPage, PageSize);
    }
}

void frmDataLog::on_btn_next_clicked()
{
    if (currentPage < getTotalPages(PageSize)) {
        currentPage++;
        loadPage(currentPage, PageSize);
    }
}

void frmDataLog::on_btn_last_clicked()
{
    currentPage = getTotalPages(PageSize);
    loadPage(currentPage, PageSize);
}

void frmDataLog::loadPage(int pageNumber, int pageSize)
{
    int offset = (pageNumber - 1) * pageSize;

//    // 创建分页查询
    QString queryStr = QString("SELECT * FROM LogInfo %1 LIMIT %2 OFFSET %3")
            .arg(whereSql)
            .arg(pageSize)
            .arg(offset);
    // 创建模型并加载数据
    QSqlQuery query(M_DbAPI::Instance()->GetDataDBHandle());
    query.exec(queryStr);

    // 更新模型
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);
    // 手动设置列名
        model->setHeaderData(0, Qt::Horizontal, "车号");
        model->setHeaderData(1, Qt::Horizontal, "车厢号");
        model->setHeaderData(2, Qt::Horizontal, "设备ID");
        model->setHeaderData(3, Qt::Horizontal, "设备通道");
        model->setHeaderData(4, Qt::Horizontal, "设备名称");
        model->setHeaderData(5, Qt::Horizontal, "日志类型");
        model->setHeaderData(6, Qt::Horizontal, "报警等级");
        model->setHeaderData(7, Qt::Horizontal, "时间");
        model->setHeaderData(8, Qt::Horizontal, "描述");

    ui->tableMain->setModel(model);
    ui->tableMain->horizontalHeader()->setStretchLastSection(true);

    // 更新分页信息
    updatePaginationInfo(pageSize);
}

void frmDataLog::updatePaginationInfo(int pageSize)
{
    int totalPages = getTotalPages(pageSize);

    ui->txt_currentpage->setText(QString::number(currentPage));
    ui->txt_totlapage->setText(QString::number(totalPages));

    if(totalPages <= 1){
        ui->btn_first->setEnabled(false);
        ui->btn_last->setEnabled(false);
        ui->btn_next->setEnabled(false);
        ui->btn_previous->setEnabled(false);
    }else{
        ui->btn_first->setEnabled(true);
        ui->btn_last->setEnabled(true);
        ui->btn_next->setEnabled(true);
        ui->btn_previous->setEnabled(true);
    }
}

int frmDataLog::getTotalCount()
{
    QSqlQuery query(M_DbAPI::Instance()->GetDataDBHandle());
    QString tempsql = QString("SELECT COUNT(*) FROM LogInfo %1").arg(whereSql);
    query.exec(tempsql);

    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int frmDataLog::getTotalPages(int pageSize)
{
    int totalCount = getTotalCount();
    return (totalCount + pageSize - 1) / pageSize;  // 向上取整
}

void frmDataLog::initializePagination(int pageSize)
{
    currentPage = 1;
    loadPage(currentPage, pageSize);
}

void frmDataLog::SetPromptBox(QString text)
{
    QFont font;
    // 设置字体类型
    font.setFamily("Arial");
    font.setPointSize(14);
    // 设置加粗
    font.setBold(true);

    QPalette palette = ui->lab_Info->palette();
    palette.setColor(QPalette::WindowText, QColor(255, 0, 0)); // 红色字体

    ui->lab_Info->setText(text);
    ui->lab_Info->setFont(font);
    ui->lab_Info->setPalette(palette);
}

void frmDataLog::on_cboxPageCount_currentTextChanged(const QString &arg1)
{
    PageSize = arg1.toInt();
}

void frmDataLog::txt_currentpage_Skip()
{
    QString text = ui->txt_currentpage->text();


    bool ok;
    int pageNumber = text.toInt(&ok);
    int totalPage = getTotalPages(PageSize);
    // 如果输入的页码有效且在允许的范围内
    if (ok && pageNumber >= 1 && pageNumber <= totalPage) {
        currentPage = pageNumber;
        loadPage(currentPage, PageSize);  // 加载指定页的数据
    } else {
        // 显示错误或恢复为当前页
        ui->txt_currentpage->setText(QString::number(currentPage));  // 恢复为当前页数
    }
}

void frmDataLog::on_btnSelect_clicked()
{
    QString wagonname = ui->cboxcarNum->currentText();
    QString logtype = ui->cboxLogType->currentText();
    QString alarmgrade = ui->cboxAlarmlevel->currentText();
    QDateTime dateStart = ui->dateStart->dateTime();
    QDateTime dateEnd = ui->dateEnd->dateTime();

    if (dateStart > dateEnd) {
        QMessageBox::warning(nullptr,"警告","开始时间不能大于结束时间!");
        return;
    }
    QString sql = "where 1=1";
    if(ui->ckcarNum->isChecked()){
        sql += " and WagonNumber = '" + wagonname + "'";
    }

    if(ui->ckLogType->isChecked()){
        sql += " and LogType = '" + logtype + "'";
    }

    if(ui->ckAlarmlevel->isChecked()){
        sql += " and AlarmGrade = '" + alarmgrade + "'";
    }

    if(ui->ckTriggerTimeStart->isChecked()){
        sql += " and TriggerTime >= '" + dateStart.toString("yyyy-MM-dd HH:mm:ss") + "'";
        sql += " and TriggerTime <= '" + dateEnd.toString("yyyy-MM-dd HH:mm:ss") + "'";
    }
    this->whereSql = sql;
    currentPage = 1;
    loadPage(currentPage,PageSize);
}

void frmDataLog::on_ckTriggerTimeStart_toggled(bool checked)
{
    ui->dateStart->setEnabled(checked);
    ui->dateEnd->setEnabled(checked);
    ui->ckTriggerTimeEnd->setChecked(checked);
}

void frmDataLog::on_ckTriggerTimeEnd_toggled(bool checked)
{
    ui->dateStart->setEnabled(checked);
    ui->dateEnd->setEnabled(checked);
    ui->ckTriggerTimeStart->setChecked(checked);
}

void frmDataLog::on_ckcarNum_toggled(bool checked)
{
    ui->cboxcarNum->setEnabled(checked);
}

void frmDataLog::on_ckLogType_toggled(bool checked)
{
    ui->cboxLogType->setEnabled(checked);
}

void frmDataLog::on_ckAlarmlevel_toggled(bool checked)
{
    ui->cboxAlarmlevel->setEnabled(checked);
}
