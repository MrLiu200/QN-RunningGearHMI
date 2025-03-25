#include "datadownload.h"
#include "ui_datadownload.h"
#include "m_networkapi.h"
#include <QMessageBox>
DataDownload::DataDownload(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataDownload)
{
    ui->setupUi(this);
    this->setWindowTitle("数据下载");
    InitFrm();
    StartDownLoad = false;

    connect(M_NetworkAPI::Instance(),&M_NetworkAPI::ReturnAllDataFile,this,&DataDownload::DownLoadFinish);
    connect(M_NetworkAPI::Instance(),&M_NetworkAPI::downloadprogress,this,&DataDownload::DownLoadProgress);
    connect(M_NetworkAPI::Instance(),&M_NetworkAPI::StartDownLoadAllData,[=](int count){
        ui->btn_download->setText("下载中,请稍后……");
        this->totalfilecount = count;
        this->currentfilecount = 0;
        ui->lab_info->setText(QString("%1/%2").arg(currentfilecount).arg(totalfilecount));
    });
    connect(M_NetworkAPI::Instance(),&M_NetworkAPI::ReturnDownLoadData,this,&DataDownload::ReturnDownLoadData);
    connect(M_NetworkAPI::Instance(),&M_NetworkAPI::ReturnCurrentDownLoadFinish,this,&DataDownload::ReturnCurrentDownLoadFinish);

}

DataDownload::~DataDownload()
{
    delete ui;
}

void DataDownload::InitFrm()
{
    //设置UI默认数据
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(0);
    ui->date_start->setDate(QDate::currentDate().addDays(-3));
    ui->date_end->setDate(QDate::currentDate().addDays(-1));
    setFixedSize(200,180);
}

void DataDownload::on_btn_download_clicked()
{
    if(StartDownLoad) return;
    if(ui->date_end->date() < ui->date_start->date()){
        QMessageBox::warning(this,"错误","结束时间不能小于开始时间");
        return;
    }
    QString starttime = ui->date_start->dateTime().toString("yyyy-MM-dd");
    QString endtime = ui->date_end->dateTime().toString("yyyy-MM-dd");
    bool DeleteFile = ui->check_enabledelete->isChecked();
    QString str = QString("起始时间:%1\n终止时间:%2\n%3")
            .arg(starttime).arg(endtime).arg(DeleteFile? "压缩后删除源文件" : "不删除源文件");
    if(QMessageBox::question(this,"下载","确认以该条件下载？\n*******************\n" + str + "\n*******************") == QMessageBox::Yes){
        QStringList tcplist;
        tcplist.append(starttime);
        tcplist.append(endtime);
        tcplist.append(QString::number(DeleteFile));
        M_TcpAPI::Instance()->GetAllData(tcplist.join(";"));
        ui->btn_download->setText("数据压缩中……");
        ui->btn_download->setEnabled(false);
        StartDownLoad = true;
    }
}

void DataDownload::DownLoadFinish(QString filename)
{
    QMessageBox::information(this,"下载成功","下载成功,保存为:\n" + filename);
    ui->btn_download->setText("下载数据");
    ui->btn_download->setEnabled(true);
    StartDownLoad = false;
    ui->progressBar->setValue(0);
    ui->lab_info->clear();
}

void DataDownload::DownLoadProgress(uint64_t current, uint64_t Total)
{
    if (Total == 0) {
        ui->progressBar->setValue(0);
        return;
    }

    int proportion = std::ceil(static_cast<double>(current*100) / Total);
    if(proportion >= 100){
        proportion = 100;
    }
    ui->progressBar->setValue(proportion);
}

void DataDownload::ReturnDownLoadData()
{
    QMessageBox::information(this,"下载","数据下载完成");
    ui->lab_info->setText(QString("%1/%2").arg(totalfilecount).arg(totalfilecount));
    ui->btn_download->setText("下载数据");
    ui->btn_download->setEnabled(true);
    currentfilecount = 0;
    totalfilecount = 0;
    StartDownLoad = false;
    ui->progressBar->setValue(0);
}

void DataDownload::ReturnCurrentDownLoadFinish()
{
    this->currentfilecount++;
    ui->lab_info->setText(QString("%1/%2").arg(currentfilecount).arg(totalfilecount));
}
