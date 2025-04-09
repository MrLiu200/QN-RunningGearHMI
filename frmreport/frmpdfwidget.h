#ifndef FRMPDFWIDGET_H
#define FRMPDFWIDGET_H

#include <QWidget>
#include "reporthelp.h"
namespace Ui {
class frmPDFWidget;
}

class frmPDFWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmPDFWidget(QWidget *parent = nullptr);
    ~frmPDFWidget();

private slots:
    void on_btn_opendb_clicked();

    void on_btn_openconf_clicked();

    void on_btn_save_clicked();

    void on_btn_reporting_clicked();

private:
    Ui::frmPDFWidget *ui;
    QList<ReportHelp::PDFDEVICE> pdf_deviceList;
    QStringList pdf_wagonlist;
    QStringList FilesList;
    QString DBFileName;
    QString PDFSaveFile;
    QString CarNum;
    bool CanGenerate;

private:
    void InitFrm();
    void LoadConfig();
    void LoadSQL();
};

#endif // FRMPDFWIDGET_H
