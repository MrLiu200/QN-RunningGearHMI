#ifndef FRMSYSTEMLOG_H
#define FRMSYSTEMLOG_H

#include <QWidget>

namespace Ui {
class frmSystemLog;
}

class frmSystemLog : public QWidget
{
    Q_OBJECT

public:
    explicit frmSystemLog(QWidget *parent = nullptr);
    ~frmSystemLog();

private slots:
    void on_btn_first_clicked();

    void on_btn_previous_clicked();

    void on_btn_next_clicked();

    void on_btn_last_clicked();

    void on_btnSelect_clicked();

    void on_ckuser_toggled(bool checked);

    void on_ckTriggerTimeStart_toggled(bool checked);

    void on_ckTriggerTimeEnd_toggled(bool checked);

    void txt_currentpage_Skip();
    void on_cboxPageCount_currentTextChanged(const QString &arg1);

    void on_btnExcel_clicked();

    void on_btnClear_clicked();

    void on_btnDelete_clicked();

private:
    Ui::frmSystemLog *ui;
    void InitFrom();
    int PageSize;
    int currentPage;
    QString whereSql;
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
    void SetPromptBox(QString text,bool alarm);
};

#endif // FRMSYSTEMLOG_H
