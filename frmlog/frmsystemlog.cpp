#include "frmsystemlog.h"
#include "ui_frmsystemlog.h"
#include "m_dbapi.h"
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QSqlRecord>
#include "xlsxdocument.h"
#include "xlsxformat.h"
frmSystemLog::frmSystemLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmSystemLog)
{
    ui->setupUi(this);
    InitFrom();
    initializePagination(PageSize);
}

frmSystemLog::~frmSystemLog()
{
    delete ui;
}

void frmSystemLog::InitFrom()
{
    QSqlQuery query(M_DbAPI::Instance()->GetLocalDBHandle());
    QString queryStr = "select User from UserInfo";
    if(query.exec(queryStr)){
        while(query.next()){
            ui->cboxuser->addItem(query.value(0).toString());
        }
    }


    QStringList pageCount;
    pageCount << "20" << "25" <<"30" << "35" << "40" << "45" << "50" << "55" << "60";
    ui->cboxPageCount->addItems(pageCount);
    ui->cboxPageCount->setCurrentIndex(2);
    PageSize = 30;

    ui->tableMain->setSelectionBehavior(QAbstractItemView::SelectRows);

    QList<QToolButton *> btns1;
    QStringList tips;
    tips << "第一页" << "上一页" << "下一页" << "最后一页";
    btns1 << ui->btn_first << ui->btn_previous << ui->btn_next << ui->btn_last;
    for(int i=0;i<btns1.size();i++){//203, 241, 255
        btns1.at(i)->setStyleSheet("QToolButton {"
                                   "border: none;"
                                   "background: none;"
                                   "padding: 1px;"
                                   "}"
                                   "QToolButton:hover {"
                                   "background-color: rgb(225, 225, 232);"
                                   "}"
                                   "QToolButton:pressed {"
                                   "background-color: gray;"
                                   "}");
        btns1.at(i)->setToolTip(tips.at(i));
    }

    ui->txt_totlapage->setReadOnly(true);
    ui->txt_totlapage->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    ui->txt_currentpage->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    connect(ui->txt_currentpage,&QLineEdit::editingFinished,this,&frmSystemLog::txt_currentpage_Skip);

    ui->cboxuser->setEnabled(false);
    ui->dateStart->setEnabled(false);
    ui->dateEnd->setEnabled(false);
}

void frmSystemLog::loadPage(int pageNumber, int pageSize)
{

    int offset = (pageNumber - 1) * pageSize;

//   创建分页查询
    QString queryStr = QString("SELECT * FROM LogInfo %1 LIMIT %2 OFFSET %3")
            .arg(whereSql)
            .arg(pageSize)
            .arg(offset);

    // 创建模型并加载数据
    QSqlQuery query(M_DbAPI::Instance()->GetLocalDBHandle());
    if(!query.exec(queryStr)){
        SetPromptBox("查询失败，请重试",true);
        return;
    }

    // 更新模型
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);
    // 手动设置列名
        model->setHeaderData(0, Qt::Horizontal, "操作员");
        model->setHeaderData(1, Qt::Horizontal, "内容");
        model->setHeaderData(2, Qt::Horizontal, "触发时间");
        model->setHeaderData(3, Qt::Horizontal, "描述");
    ui->tableMain->setModel(model);
    // 设置指定列宽
    ui->tableMain->setColumnWidth(0, 120);
    ui->tableMain->setColumnWidth(1, 120);
    ui->tableMain->setColumnWidth(2, 200);
    ui->tableMain->setColumnWidth(3, 250);
    // 使最后一列自适应宽度
    ui->tableMain->horizontalHeader()->setStretchLastSection(true);
    SetPromptBox("查询成功",false);
    // 更新分页信息
    updatePaginationInfo(pageSize);
}

void frmSystemLog::updatePaginationInfo(int pageSize)
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

int frmSystemLog::getTotalCount()
{
    QSqlQuery query(M_DbAPI::Instance()->GetLocalDBHandle());
    QString tempsql = QString("SELECT COUNT(*) FROM LogInfo %1").arg(whereSql);
    query.exec(tempsql);

    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int frmSystemLog::getTotalPages(int pageSize)
{
    int totalCount = getTotalCount();
    return (totalCount + pageSize - 1) / pageSize;
}

void frmSystemLog::initializePagination(int pageSize)
{
    currentPage = 1;
    loadPage(currentPage, pageSize);
}

void frmSystemLog::SetPromptBox(QString text, bool alarm)
{
    QFont font;
    // 设置字体类型
    font.setFamily("Arial");
    font.setPointSize(14);
    // 设置加粗
    font.setBold(true);

    QPalette palette = ui->lab_Info->palette();
    if(alarm){
        palette.setColor(QPalette::WindowText, QColor(255, 0, 0)); // 红色字体
    }else{
        palette.setColor(QPalette::WindowText, QColor(0, 255, 0)); // 绿色字体
    }


    ui->lab_Info->setText(text);
    ui->lab_Info->setFont(font);
    ui->lab_Info->setPalette(palette);
}

void frmSystemLog::on_btn_first_clicked()
{
    currentPage = 1;
    loadPage(currentPage, PageSize);
}

void frmSystemLog::on_btn_previous_clicked()
{
    if (currentPage > 1) {
        currentPage--;
        loadPage(currentPage, PageSize);
    }
}

void frmSystemLog::on_btn_next_clicked()
{
    if (currentPage < getTotalPages(PageSize)) {
        currentPage++;
        loadPage(currentPage, PageSize);
    }
}

void frmSystemLog::on_btn_last_clicked()
{
    currentPage = getTotalPages(PageSize);
    loadPage(currentPage, PageSize);
}

void frmSystemLog::on_btnSelect_clicked()
{
    QDateTime dateStart = ui->dateStart->dateTime();
    QDateTime dateEnd = ui->dateEnd->dateTime();
    QString user = ui->cboxuser->currentText();

    if (dateStart > dateEnd) {
        QMessageBox::warning(nullptr,"警告","开始时间不能大于结束时间!");
        return;
    }

    QString sql = "where 1=1";
    if(ui->ckuser->isChecked()){
        sql += " and User = '" + user + "'";
    }

    if(ui->ckTriggerTimeStart->isChecked()){
        sql += " and TriggerTime >= '" + dateStart.toString("yyyy-MM-dd HH:mm:ss") + "'";
        sql += " and TriggerTime <= '" + dateEnd.toString("yyyy-MM-dd HH:mm:ss") + "'";
    }

    this->whereSql = sql;
    currentPage = 1;
    loadPage(currentPage,PageSize);
}

void frmSystemLog::on_ckuser_toggled(bool checked)
{
    ui->cboxuser->setEnabled(checked);
}

void frmSystemLog::on_ckTriggerTimeStart_toggled(bool checked)
{
    ui->dateStart->setEnabled(checked);
    ui->dateEnd->setEnabled(checked);
    ui->ckTriggerTimeEnd->setEnabled(checked);
}

void frmSystemLog::on_ckTriggerTimeEnd_toggled(bool checked)
{
    ui->dateStart->setEnabled(checked);
    ui->dateEnd->setEnabled(checked);
    ui->ckTriggerTimeStart->setEnabled(checked);
}

void frmSystemLog::txt_currentpage_Skip()
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

void frmSystemLog::on_cboxPageCount_currentTextChanged(const QString &arg1)
{
    PageSize = arg1.toInt();
}

void frmSystemLog::on_btnExcel_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,"new file",qApp->applicationDirPath(),tr("Excel Files (*.xlsx)"));
    if(fileName.isEmpty()) return;
    //查询符合条件的数据库内容
    QStringList content;
    QSqlQuery query(M_DbAPI::Instance()->GetLocalDBHandle());
    QString sql = QString("select * from LogInfo %1").arg(whereSql);
    query.exec(sql);


    int columnCount = query.record().count();
    QList<QString> columnNames;
    columnNames << "操作员" << "内容" << "触发时间" << "描述";
    QList<int> columnwidths;
    columnwidths << 20 << 20 << 20 << 50;

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
            xlsx.write(1, col + 1, columnNames.at(col),format);
            xlsx.setColumnWidth(col + 1, columnwidths.at(col));
        }

    //填充数据
    int rowNum = 2;
    format.setFontBold(false);
    format.setFontSize(12);
    while (query.next()) {
        for (int i = 0; i < columnCount; ++i) {
            QString temp = query.value(i).toString();
            xlsx.write(rowNum,i+1,temp,format);
        }
        rowNum++;
    }
    if(xlsx.saveAs(fileName)){
        QMessageBox::information(this,"提示","导出excel成功");
        SetPromptBox("导出excel成功",false);
        M_DbAPI::Instance()->AddLoaclLog("导出日志",DATETIME,whereSql);
    }else{
        QMessageBox::warning(this,"导出出错",QString("文件%1 写入出错").arg(fileName));
        SetPromptBox(QString("文件%1 写入出错").arg(fileName),true);
    }
}

void frmSystemLog::on_btnClear_clicked()
{
    if(QMessageBox::question(nullptr,"确认","确认清空所查询内容？") == QMessageBox::No){
        return;
    }
    QSqlQuery query(M_DbAPI::Instance()->GetLocalDBHandle());
    QString deletesql = QString("DELETE FROM LogInfo %1").arg(whereSql);
    if(query.exec(deletesql)){
        SetPromptBox("清空成功",false);
        M_DbAPI::Instance()->AddLoaclLog("删除日志",DATETIME,"");
    }else{
        SetPromptBox("清空失败",true);
    }
    on_btnSelect_clicked();
}

void frmSystemLog::on_btnDelete_clicked()
{
    QModelIndexList selectedIndexes = ui->tableMain->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        qDebug() << "没有选中任何行";
        return;  // 如果没有选中任何行，则返回
    }else{
        if(QMessageBox::question(nullptr,"本地日志","确认删除所选日志？") == QMessageBox::No){
            return;
        }
    }

    QSqlQuery query(M_DbAPI::Instance()->GetLocalDBHandle());
    query.prepare("DELETE FROM LogInfo WHERE User = :user and TriggerTime = :time");
    for(int i=0;i<selectedIndexes.size();i++){
        int row = selectedIndexes.at(i).row();
        // 获取选中行的数据
        QSqlQueryModel *model = qobject_cast<QSqlQueryModel *>(ui->tableMain->model());
        QString user = model->data(model->index(row, 0)).toString();
        QString time = model->data(model->index(row, 2)).toString();
        query.bindValue(":user", user);
        query.bindValue(":time", time);
        if(!query.exec()){
            SetPromptBox(query.lastError().text(),true);
            return;
        }
    }
    SetPromptBox("删除成功",false);
    on_btnSelect_clicked();
}
