#ifndef DATADOWNLOAD_H
#define DATADOWNLOAD_H

#include <QDialog>
#include <QtCore>
namespace Ui {
class DataDownload;
}

class DataDownload : public QDialog
{
    Q_OBJECT

public:
    explicit DataDownload(QWidget *parent = nullptr);
    ~DataDownload();

private slots:

    void on_btn_download_clicked();

    void DownLoadFinish(QString filename);

    void DownLoadProgress(uint64_t current, uint64_t Total);

    void ReturnDownLoadData();

    void ReturnCurrentDownLoadFinish();
private:
    Ui::DataDownload *ui;
    QString title;
    bool StartDownLoad;
    int totalfilecount;
    int currentfilecount;
    void InitFrm();
};

#endif // DATADOWNLOAD_H
