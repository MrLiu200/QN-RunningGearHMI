#include "frmpdfwidget.h"
#include "ui_frmpdfwidget.h"
#include <QFileDialog>
#include <QTextStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include "km4template.h"
frmPDFWidget::frmPDFWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmPDFWidget)
{
    ui->setupUi(this);
    InitFrm();
}

frmPDFWidget::~frmPDFWidget()
{
    delete ui;
}

void frmPDFWidget::InitFrm()
{
    setWindowTitle("诊断报告生成");
    ui->cbox_template->addItem("KM4");
    ui->lab_instructions->setText(
                "<h2>自动诊断报告使用步骤</h2>"
                "<p>1、选择相应的报告模板</p>"
                "<p>2、加载该车辆所有车厢的配置文件</p>"
                "<p>3、加载该车辆的数据库文件</p>"
                "<p>4、调整诊断日期及文件保存位置</p>"
                "<p>5、点击生成报告按钮</p>"
                );
    ui->date_start->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->date_end->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->date_start->setCalendarPopup(true);
    ui->date_end->setCalendarPopup(true);
    ui->date_start->setDateTime(QDateTime::currentDateTime().addMonths(-1));
    ui->date_end->setDateTime(QDateTime::currentDateTime());
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    CanGenerate = true;
}

void frmPDFWidget::LoadConfig()
{
    if(FilesList.isEmpty()) return;
    pdf_deviceList.clear();
    pdf_wagonlist.clear();
    CarNum.clear();
    for(QString filename : FilesList){
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            continue;
        }
        //加载数据
        QTextStream in(&file);
        //取出车厢号
        QString wagon = "";
        QFileInfo fileinfo(filename);
        QString basename = fileinfo.baseName();
        QStringList list = basename.split("_");
        if(list.size() > 3){
            wagon= list.at(3);
        }
        if(CarNum.isEmpty()){
            CarNum = list.at(2);
        }else{
            if(CarNum != list.at(2)){
                CanGenerate = false;
                QMessageBox::warning(this,"配置信息","配置信息中车辆不一致");
                return;
            }
        }
        //判断，如果车厢已经存在，则跳过
        if(pdf_wagonlist.contains(wagon)){
            continue;
        }else{
            pdf_wagonlist.append(wagon);
        }
        QString infotype;
        int infolen = 0;
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            //判断这一行是什么内容
            if(line == "PreInfo"){
                infotype = line;
                if(!in.atEnd()){
                    line = in.readLine().trimmed();
                    infolen = line.toInt();
                }
            }else if(infolen > 0){
                QStringList dataList = line.split(",");
                if(infotype == "PreInfo"){
                    if(dataList.size() < 20) continue;
                    ReportHelp::PDFDEVICE device;
                    device.Wagon = wagon;
                    device.id = dataList.at(0).trimmed().toInt();
                    device.ch = dataList.at(1).trimmed().toInt();
                    device.name = dataList.at(2).trimmed();
                    device.type = dataList.at(3).trimmed();
                    device.AxisPosition = dataList.at(5).trimmed().toInt();
                    device.alarmgrade = -1;
                    pdf_deviceList.append(device);
                }
                infolen--;
            }
        }
        file.close();
    }
}

void frmPDFWidget::LoadSQL()
{
    if(DBFileName.isEmpty()) return;
    //连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","TemporaryDB");
    db.setDatabaseName(DBFileName);
    bool ok = db.open();
    if(!ok){
        QMessageBox::warning(this,"数据库","数据库打开失败");
        CanGenerate = false;
        return;
    }

    //根据车厢号、轴位、id、ch来判断是否有过报警
    for(auto &device : pdf_deviceList){
        QSqlQuery query(db);
        query.prepare(R"(
                      SELECT MAX(AlarmGrade)
                      FROM LogInfo
                      WHERE DeviceID = :id AND DeviceCH = :ch
                      AND WagonNumber = :wagon
                      AND LogType = '报警信息'
                      AND TriggerTime BETWEEN :start AND :end
                      )");
        query.bindValue(":id", device.id);
        query.bindValue(":ch", device.ch);
        query.bindValue(":wagon",device.Wagon);
        query.bindValue(":start", "2023-04-02 16:19:25");
        query.bindValue(":end", "2025-04-08 16:36:28");
        if (query.exec() && query.next()){
            device.alarmgrade = query.value(0).toInt();
        }else{
            device.alarmgrade = -1;
        }
    }
    db.close();
}

void frmPDFWidget::on_btn_opendb_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"导入数据库文件","",
                                                    tr("db文件(*.db)"));
    if(filename.isEmpty()) return;
    DBFileName = filename;
}

void frmPDFWidget::on_btn_openconf_clicked()
{
    QStringList list = QFileDialog::getOpenFileNames(this,"选择导入的配置文件","",
                                                     tr("配置文件(*.csv)"));
    if(list.isEmpty()) return;
    FilesList.clear();
    FilesList = list;

    ui->listWidget->clear();
    for(QString name : FilesList){
        ui->listWidget->addItem(QFileInfo(name).fileName());
    }
}

void frmPDFWidget::on_btn_save_clicked()
{
    QString str = QFileDialog::getSaveFileName(this,"保存路径","",tr("pdf文件(*.pdf)"));
    if(str.isEmpty()) return;
    if (!str.endsWith(".pdf", Qt::CaseInsensitive)) {
        str += ".pdf";
    }
    PDFSaveFile = str;
    ui->txt_savepath->setText(PDFSaveFile);
}

void frmPDFWidget::on_btn_reporting_clicked()
{
    if(FilesList.isEmpty() || DBFileName.isEmpty() || PDFSaveFile.isEmpty()){
        QMessageBox::warning(this,"条件错误","配置信息不完善");
        return;
    }
    if(QMessageBox::question(this,"报告生成","确认以当前选项生成报告？") == QMessageBox::Yes){
        LoadConfig();
        LoadSQL();
        if(!CanGenerate) return;
        if(pdf_wagonlist.isEmpty() || pdf_deviceList.isEmpty() || CarNum.isEmpty()) return;
        QString Templatestr = ui->cbox_template->currentText();
        QString StartTime = ui->date_start->dateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString EndTime = ui->date_end->dateTime().toString("yyyy-MM-dd HH:mm:ss");
        if(Templatestr == "KM4"){
            KM4Template pdfapi;
            pdfapi.SetData(CarNum,StartTime,EndTime,PDFSaveFile,pdf_wagonlist,pdf_deviceList);
            if(pdfapi.drawReport()){
                QMessageBox::information(this,"报告生成","报告已生成");
            }
        }
    }
}
