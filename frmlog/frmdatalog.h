#ifndef FRMDATALOG_H
#define FRMDATALOG_H

#include <QWidget>
#include <QFileDialog>
#include "m_dbapi.h"
#include <QSqlTableModel>
namespace Ui {
class frmDataLog;
}

class frmDataLog : public QWidget
{
    Q_OBJECT

public:
    explicit frmDataLog(QWidget *parent = nullptr);
    ~frmDataLog();
private:
    Ui::frmDataLog *ui;
    int currentPage;
    int PageSize;
    QString whereSql;
    void InitForm();

private slots:
    void on_btnExcel_clicked();

    void LoadData();

    void on_btn_first_clicked();

    void on_btn_previous_clicked();

    void on_btn_next_clicked();

    void on_btn_last_clicked();

    void on_cboxPageCount_currentTextChanged(const QString &arg1);

    void txt_currentpage_Skip();

    void on_btnSelect_clicked();

    void on_ckTriggerTimeStart_toggled(bool checked);

    void on_ckTriggerTimeEnd_toggled(bool checked);

    void on_ckcarNum_toggled(bool checked);

    void on_ckLogType_toggled(bool checked);

    void on_ckAlarmlevel_toggled(bool checked);

private:
    //加载页数
    void loadPage(int pageNumber, int pageSize);
    //更新当前页码和总页数
    void updatePaginationInfo(int pageSize);
    //获取总行数
    int getTotalCount();
    //获取总页数
    int getTotalPages(int pageSize);
    //初始化第一页
    void initializePagination(int pageSize);
    //设置提示框
    void SetPromptBox(QString text);
};

#endif // FRMDATALOG_H
