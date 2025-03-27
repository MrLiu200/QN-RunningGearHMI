#include "frmalgorithmoffline.h"
#include "ui_frmalgorithmoffline.h"
#include "indicatorlight.h"
#include "app.h"
#include <QFileDialog>
#include <QFile>
#include "predata.h"
#include "corehelper.h"
#include "algorithm_v2.h"
#include <QMessageBox>
#include <QtConcurrent>
FrmAlgorithmOffline::FrmAlgorithmOffline(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmAlgorithmOffline)
{
    ui->setupUi(this);
    DiagnosticResults.clear();
    InitFrm();
}

FrmAlgorithmOffline::~FrmAlgorithmOffline()
{
    delete ui;
}

void FrmAlgorithmOffline::InitFrm()
{
    QFile file("D:/project/Qt_project/Example/MySrc/RunningGearHMI/HMI/image/other/style1.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }
    //table Init
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels({"文件名", "计算", "运行结果", "走行部结论","查看DB"});
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableWidget->horizontalHeader()->setStretchLastSection(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive); // 允许手动调整列宽

    // 第一列：文件名，自动调整
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    // 第二列："计算" 按钮，固定宽度
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableWidget->setColumnWidth(1, 100);

    // 第三列："结果"，填充剩余空间
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    // 第四列："走行部结论"，填充剩余空间
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    // 第五列："查看DB" 按钮，**固定宽度**
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableWidget->setColumnWidth(4, 100);

    //设置固定列宽
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(40);

    //设置默认路径 = 临时数据路径
    folderPath = App::DownLoadTempDataDir;

    //添加lineEdit
    //故障频率
    txt_frequency.clear();
    txt_frequency.append(ui->txt_baowai1frequency);
    txt_frequency.append(ui->txt_baonei1frequency);
    txt_frequency.append(ui->txt_waihuan1frequency);
    txt_frequency.append(ui->txt_neihuan1frequency);
    txt_frequency.append(ui->txt_gundong1frequency);

    txt_frequency.append(ui->txt_baowai2frequency);
    txt_frequency.append(ui->txt_baonei2frequency);
    txt_frequency.append(ui->txt_waihuan2frequency);
    txt_frequency.append(ui->txt_neihuan2frequency);
    txt_frequency.append(ui->txt_gundong2frequency);

    txt_frequency.append(ui->txt_baowai3frequency);
    txt_frequency.append(ui->txt_baonei3frequency);
    txt_frequency.append(ui->txt_waihuan3frequency);
    txt_frequency.append(ui->txt_neihuan3frequency);
    txt_frequency.append(ui->txt_gundong3frequency);

    txt_frequency.append(ui->txt_baowai4frequency);
    txt_frequency.append(ui->txt_baonei4frequency);
    txt_frequency.append(ui->txt_waihuan4frequency);
    txt_frequency.append(ui->txt_neihuan4frequency);
    txt_frequency.append(ui->txt_gundong4frequency);

    txt_frequency.append(ui->txt_tamianfrequency);
    txt_frequency.append(ui->txt_niehefrequency);
    txt_frequency.append(ui->txt_congdongfrequency);
    txt_frequency.append(ui->txt_zhudongfrequency);

    //包络DB
    txt_dbbaoluo.clear();
    txt_dbbaoluo.append(ui->txt_baowai1baoluo);
    txt_dbbaoluo.append(ui->txt_baonei1baoluo);
    txt_dbbaoluo.append(ui->txt_waihuan1baoluo);
    txt_dbbaoluo.append(ui->txt_neihuan1baoluo);
    txt_dbbaoluo.append(ui->txt_gundong1baoluo);

    txt_dbbaoluo.append(ui->txt_baowai2baoluo);
    txt_dbbaoluo.append(ui->txt_baonei2baoluo);
    txt_dbbaoluo.append(ui->txt_waihuan2baoluo);
    txt_dbbaoluo.append(ui->txt_neihuan2baoluo);
    txt_dbbaoluo.append(ui->txt_gundong2baoluo);

    txt_dbbaoluo.append(ui->txt_baowai3baoluo);
    txt_dbbaoluo.append(ui->txt_baonei3baoluo);
    txt_dbbaoluo.append(ui->txt_waihuan3baoluo);
    txt_dbbaoluo.append(ui->txt_neihuan3baoluo);
    txt_dbbaoluo.append(ui->txt_gundong3baoluo);

    txt_dbbaoluo.append(ui->txt_baowai4baoluo);
    txt_dbbaoluo.append(ui->txt_baonei4baoluo);
    txt_dbbaoluo.append(ui->txt_waihuan4baoluo);
    txt_dbbaoluo.append(ui->txt_neihuan4baoluo);
    txt_dbbaoluo.append(ui->txt_gundong4baoluo);

    //频谱DB
    txt_dbpinpu.clear();
    txt_dbpinpu.append(ui->txt_baowai1pinpu);
    txt_dbpinpu.append(ui->txt_baonei1pinpu);
    txt_dbpinpu.append(ui->txt_waihuan1pinpu);
    txt_dbpinpu.append(ui->txt_neihuan1pinpu);
    txt_dbpinpu.append(ui->txt_gundong1pinpu);

    txt_dbpinpu.append(ui->txt_baowai2pinpu);
    txt_dbpinpu.append(ui->txt_baonei2pinpu);
    txt_dbpinpu.append(ui->txt_waihuan2pinpu);
    txt_dbpinpu.append(ui->txt_neihuan2pinpu);
    txt_dbpinpu.append(ui->txt_gundong2pinpu);

    txt_dbpinpu.append(ui->txt_baowai3pinpu);
    txt_dbpinpu.append(ui->txt_baonei3pinpu);
    txt_dbpinpu.append(ui->txt_waihuan3pinpu);
    txt_dbpinpu.append(ui->txt_neihuan3pinpu);
    txt_dbpinpu.append(ui->txt_gundong3pinpu);

    txt_dbpinpu.append(ui->txt_baowai4pinpu);
    txt_dbpinpu.append(ui->txt_baonei4pinpu);
    txt_dbpinpu.append(ui->txt_waihuan4pinpu);
    txt_dbpinpu.append(ui->txt_neihuan4pinpu);
    txt_dbpinpu.append(ui->txt_gundong4pinpu);

    //其他db
    txt_dbother.clear();
    txt_dbother.append(ui->txt_tamiandb);
//    txt_dbother.append(ui->txt_niehedb);
    txt_dbother.append(ui->txt_congdongdb);
    txt_dbother.append(ui->txt_zhudongdb);

    //设置显示居中和不可选
    for(auto txt : txt_frequency){
        txt->setReadOnly(true);
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    for(auto txt : txt_dbbaoluo){
        txt->setReadOnly(true);
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    for(auto txt : txt_dbpinpu){
        txt->setReadOnly(true);
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    for(auto txt : txt_dbother){
        txt->setReadOnly(true);
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    ui->txt_allcount->setReadOnly(true);
    ui->txt_normalcount->setReadOnly(true);
    ui->txt_waringcount->setReadOnly(true);
    ui->txt_alarm1count->setReadOnly(true);
    ui->txt_alarm2count->setReadOnly(true);

    ui->txt_allcount->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->txt_normalcount->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->txt_waringcount->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->txt_alarm1count->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->txt_alarm2count->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    //轴承信息组
//    Groups_bearing.clear();
//    Groups_bearing.append(ui->gbox_bearing1);
//    Groups_bearing.append(ui->gbox_bearing2);
//    Groups_bearing.append(ui->gbox_bearing3);
//    Groups_bearing.append(ui->gbox_bearing4);

    LoadFile();
}

void FrmAlgorithmOffline::LoadFile()
{
    if (folderPath.isEmpty()) return;
    QDir dir(folderPath);
    QStringList binFiles = dir.entryList({"vib*.bin"}, QDir::Files | QDir::NoDotAndDotDot);
    ui->txt_allcount->setText(QString::number(binFiles.size()));

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(binFiles.size());

    for (int row = 0; row < binFiles.size(); ++row) {
        // 第一列：文件名
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(binFiles[row]));

        // 第二列：计算按钮
        QPushButton *calcButton = new QPushButton(QIcon(":/image/image/Icon/run.png"),"运行");
        //        calcButton->setFixedSize(80, 30);
        connect(calcButton, &QPushButton::clicked, this,&FrmAlgorithmOffline::on_RunbtnClicked);
        ui->tableWidget->setCellWidget(row, 1, calcButton);

        // 第三列：LineEdit（运行结果）
        QLineEdit *resultEdit = new QLineEdit();
        resultEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // 允许水平扩展
        ui->tableWidget->setCellWidget(row, 2, resultEdit);

        // 第四列：LineEdit（原始结果）
        QLineEdit *oldresultEdit = new QLineEdit();
        oldresultEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // 允许水平扩展
        ui->tableWidget->setCellWidget(row, 3, oldresultEdit);

        // 第五列：查看DB 按钮
        QPushButton *dbButton = new QPushButton("查看DB");
        //        dbButton->setFixedSize(80, 30);
        connect(dbButton, &QPushButton::clicked, this,&FrmAlgorithmOffline::on_EchoDBClichek);
        ui->tableWidget->setCellWidget(row, 4, dbButton);
    }
}

void FrmAlgorithmOffline::LoadingResults(QString fileName)
{
    if(DiagnosticResults.isEmpty()) return;

    int index = -1;
    for(int i=0;i<DiagnosticResults.size();i++){
        if(DiagnosticResults.at(i).filename == fileName){
            index = i;
            break;
        }
    }

    if(index == -1) return;
    DIAGNOSTICRESULTS tempresult = DiagnosticResults.at(index);

    //轴承名称
    QStringList bearingModel = tempresult.BearingInfo.split("|");
//    for(int i=0;i<bearingModel.size();i++){
//        Groups_bearing.at(i)->setTitle(bearingModel.at(i));
//    }
    ui->gbox_bearing1->setTitle(bearingModel.at(0));
    ui->gbox_bearing2->setTitle(bearingModel.at(1));
    ui->gbox_bearing3->setTitle(bearingModel.at(2));
    ui->gbox_bearing4->setTitle(bearingModel.at(3));

    //诊断结果

    //故障频率
    for(int i=0;i<txt_frequency.size();i++){
        txt_frequency.at(i)->setText(tempresult.faultFre.at(i));
    }

    //各阶DB
    int len = txt_dbbaoluo.size();
    for(int i=0;i<len;i++){
        txt_dbbaoluo.at(i)->setText(tempresult.DBValues.at(i));
    }
    for(int i=0;i<txt_dbpinpu.size();i++){
        txt_dbpinpu.at(i)->setText(tempresult.DBValues.at(i+len));
    }
    len += txt_dbpinpu.size();
    for(int i=0;i<txt_dbother.size();i++){
        txt_dbother.at(i)->setText(tempresult.DBValues.at(i+len));
    }
}

void FrmAlgorithmOffline::clearAll()
{
    for(auto txt : txt_frequency){
        txt->clear();
    }

    for(auto txt : txt_dbbaoluo){
        txt->clear();
    }

    for(auto txt : txt_dbpinpu){
        txt->clear();
    }

    for(auto txt : txt_dbother){
        txt->clear();
    }

    ui->txt_allcount->clear();
    ui->txt_normalcount->clear();
    ui->txt_waringcount->clear();
    ui->txt_alarm1count->clear();
    ui->txt_alarm2count->clear();

    ui->tableWidget->clearContents();
    DiagnosticResults.clear();
}

void FrmAlgorithmOffline::on_RunbtnClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender()); // 获取信号发送的按钮
    if (!button) return;

    // 在 tableWidget 中找到按钮所在的行
    int row = -1;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        if (ui->tableWidget->cellWidget(i, 1) == button) { // 按钮在第1列
            row = i;
            break;
        }
    }

    if(row == -1) return;
    QString filename = ui->tableWidget->item(row, 0)->text();
    QLineEdit *edit = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(row, 2));
    QLineEdit *edit_old = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(row, 3));
    QString str = RunAlgorithm(filename);
    QStringList list = str.split(",");
    if(list.size() == 2){
        edit_old->setText(list.at(1));
    }
    edit->setText(list.at(0));
    //更新其他视图
    LoadingResults(filename);
}

void FrmAlgorithmOffline::on_EchoDBClichek()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    // 在 tableWidget 中找到按钮所在的行
    int row = -1;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        if (ui->tableWidget->cellWidget(i, 4) == button) {
            row = i;
            break;
        }
    }
    if(row == -1) return;
    QString filename = ui->tableWidget->item(row, 0)->text();
    LoadingResults(filename);
}

QString FrmAlgorithmOffline::RunAlgorithm(QString fileName)
{
    //1、判断文件是否为空
    QString ret;
    if(fileName.isEmpty()){
        ret = "文件为空";
        return ret;
    }
    QString name = folderPath + "/" + fileName;

    //2、需要根据文件名找到对应的id、ch、wagon
    QFileInfo fileinfo(name);
    QStringList list = fileinfo.baseName().split("_");
    if(list.size() != 5){
        ret = "文件错误，无法解析";
        return ret;
    }
    QString wagon = list.at(1);
    quint8 id = list.at(2).toInt();
    quint8 ch = list.at(3).toInt();
    float sensitivity = 0.0;//找出灵敏度
    int Devicetype = -1;//设备类型
    int capstanTeethNum = 0,drivenwheelTeethNum = 0;
    for(auto pre : PreData::devicelist){
        if((pre.Wagon == wagon) && (pre.id == id) && (pre.ch == ch)){
            sensitivity = pre.sensitivity;

            capstanTeethNum = pre.capstanTeethNum;
            drivenwheelTeethNum = pre.drivenwheelTeethNum;

            QString typestr = pre.type;
            if(typestr == "齿轮箱"){
                Devicetype = 1;
            }else if(typestr == "电机" || typestr == "电机传动端" || typestr == "电机非传动端"){
                Devicetype = 2;
            }else if(typestr.contains("轴箱")){
                Devicetype = 0;
            }


            break;
        }
    }
    if((sensitivity == 0.0) || (Devicetype == -1)){
        ret = "测点配置未找到";
        return ret;
    }

    //3、打开文件
    QFile file(name);
    if(!file.open(QIODevice::ReadOnly)){
        ret = "无法打开文件";
        return ret;
    }

    //4、按格式读取数据
    QDataStream in(&file);
    int VIBDataLen = 262144;
    //振动数据
    QByteArray vibData;
    vibData.resize(VIBDataLen);
    in.readRawData(vibData.data(), VIBDataLen);

    //解析出数据 格式：前一半是原始数据，后一半是滤波后的数据,低位在前，高位在后

    float Rawsum = 0,Demsum=0,Rawave=0,Demave = 0,tempdouble = 0;
    uint16_t temp = 0;
    QVector <double> RawData;                       //原始数据
    QVector<double> DemodulatedData;                //解调数据
    for(int i=0;i<VIBDataLen/2;i+=2){
        temp = ((quint8)(vibData.at(i+1)) << 8) | (quint8)(vibData.at(i));
        tempdouble = ((((double)temp*3.3*1000)/65535)-1650)/sensitivity;//换算出电压值(double)(double)/灵敏度
        RawData.append(tempdouble);
    }
    for(int i=VIBDataLen/2;i<VIBDataLen;i+=2){
        temp = (static_cast<quint8>(vibData.at(i+1)) << 8) | static_cast<quint8>(vibData.at(i));
        tempdouble = ((((double)temp*3.3*1000)/65535)-1650)/sensitivity;//换算出电压值(double)(double)/灵敏度
        DemodulatedData.append(tempdouble);
    }

    //求和并找出原始数据最大值
    Rawsum = std::accumulate(RawData.begin(), RawData.end(), 0.0);
    Demsum = std::accumulate(DemodulatedData.begin(), DemodulatedData.end(), 0.0);
    qint32 datelen = RawData.size();
    //求出平均值
    Rawave = Rawsum/datelen;
    Demave = Demsum/datelen;

    //去直流分量
    for(int i=0;i<datelen;i++){
        RawData[i] -= Rawave;
        DemodulatedData[i] -= Demave;
        DemodulatedData[i] = DemodulatedData[i] * 166;
    }
    RawData[datelen-2] = 0;
    RawData[datelen-1] = 0;
    DemodulatedData[datelen-2] = 0;
    DemodulatedData[datelen-1] = 0;
    //量纲和RMS
    QVector <float> Dimension = CoreHelper::CalculateDimension(RawData);
    float rms = Dimension.at(5);

    QByteArray remainingData = file.readAll();
    QDataStream out(&remainingData,QIODevice::ReadOnly);
    //转速
    uint32_t currentspeed = 0;
    QString label;
    out >> label;
    if(label == "speedAve:"){
        uint32_t speed;
        out >> speed;
        currentspeed = speed;
    }
    label.clear();
    //区间转速容器
    QVector<uint32_t> SpeedVector;
    out >> label;
    if(label == "SpeedVector:"){
        int len = 0;
        out >> len;
        for(int i=0;i<len;i++){
            uint32_t value;
            out >> value;
            SpeedVector.append(value);
        }
    }
    label.clear();
    //诊断结果
    QStringList OldDiagnosticResults;
    out >> label;
    if(label == "result:"){
        int len = 0;
        out >> len;
        for(int i=0;i<len;i++){
            QString str;
            out >> str;
            OldDiagnosticResults.append(str);
        }
    }
    //轴承型号
    QString BearingType;
    label.clear();
    out >> label;
    if(label == "bearingInfo:"){
        QString str;
        out >> str;
        BearingType = str;
    }
    file.close();

    //5、从数据库或者配置文件中导出轴承参数及灵敏度
    QVector<float> list1, list2, list3, list4;

    QStringList bearinglist = BearingType.split("|");
    if(bearinglist.size() != 4){
        ret = "文件内容不完整";
        return ret;
    }
    for(auto bearing : PreData::bearinglist){
        if(bearing.Wagon == wagon){
            if(bearinglist.at(0) == bearing.Model){
                list1 << bearing.PitchDiameter << bearing.RollingDiameter << bearing.RollerNum << bearing.ContactAngle;
            }
            if(bearinglist.at(1) == bearing.Model){
                list2 << bearing.PitchDiameter << bearing.RollingDiameter << bearing.RollerNum << bearing.ContactAngle;
            }
            if(bearinglist.at(2) == bearing.Model){
                list3 << bearing.PitchDiameter << bearing.RollingDiameter << bearing.RollerNum << bearing.ContactAngle;
            }
            if(bearinglist.at(3) == bearing.Model){
                list4 << bearing.PitchDiameter << bearing.RollingDiameter << bearing.RollerNum << bearing.ContactAngle;
            }
        }
    }
    if((list1.isEmpty()) || (list2.isEmpty()) || (list3.isEmpty()) || (list4.isEmpty())){
        ret = "轴承信息未找到";
        return ret;
    }

    //6、开始对数据进行计算
    //hanning window
    for(int i=0;i<datelen;i++){
        RawData[i] *= 0.5 - 0.5 * cos(2 * M_PI * i / 65535);
        DemodulatedData[i] *= 0.5 - 0.5 * cos(2 * M_PI * i / 65535);
    }

    //傅里叶变换
    CoreHelper::FFT(RawData);
    CoreHelper::FFT(DemodulatedData);
    //(2/65536)傅里叶变换后 幅值会*数据长度，所以在这里除回来，*2是hanning window幅值补阵
    for (int i=0;i<datelen;i++) {
        RawData[i] = (RawData[i]*2/65536)*2;
        DemodulatedData[i] = (DemodulatedData[i]*2/65536)*2;
    }

    //求FFT后平均值
    Rawsum = std::accumulate(RawData.begin(), RawData.end(), 0.0);
    Demsum = std::accumulate(DemodulatedData.begin(), DemodulatedData.end(), 0.0);
    Rawave = Rawsum/datelen;
    Demave = Demsum/datelen;

    Algorithm_v2::Argument_Init(list1.at(3),list1.at(0),list1.at(1),list1.at(2),
                                list2.at(3),list2.at(0),list2.at(1),list2.at(2),
                                list3.at(3),list3.at(0),list3.at(1),list3.at(2),
                                list4.at(3),list4.at(0),list4.at(1),list4.at(2),
                                50.0,50.0,currentspeed,capstanTeethNum,drivenwheelTeethNum,
                                Devicetype);
    QStringList result = Algorithm_v2::Algoruthm_Start(QVector<float>(RawData.begin(), RawData.end()),QVector<float>(DemodulatedData.begin(), DemodulatedData.end()),
                                  RawData.size(),DemodulatedData.size(),20480,
                                  rms,Rawave,Demave);

    ret.clear();
    int alarmlevel = -1;
    QStringList shunxu;
    shunxu << "保外" << "保内" << "外环" << "内环" << "滚单" << "大齿轮" << "小齿轮" << "踏面";
    //分析result，应该是 报警结果 + 故障频率 + 包络DB + 频谱 DB
    if(result.size() != 3){
        ret = "诊断失败";
        return ret;
    }else{
        //对结果进行分析
        QStringList AlarmResult = result.at(0).split("|");
        for(int i=0;i<shunxu.size();i++){
            quint8 grade = AlarmResult.at(i).toInt();
            if(grade != 255){
                QString alarmstr = "故障";
                switch (grade) {
                case 0:
                    alarmstr = "预警";
                    alarmlevel = 0;
                    break;
                case 1:
                    alarmstr = "一级报警";
                    alarmlevel = 1;
                    break;
                case 2:
                    alarmstr = "二级报警";
                    alarmlevel = 2;
                    break;
                }
                ret += shunxu.at(i) + " " + alarmstr;
            }
        }
        DIAGNOSTICRESULTS results;
        results.filename = fileName;
        results.results = ret;
        results.faultFre = result.at(1).split("|");
        results.DBValues = result.at(2).split("|");
        results.Oldresults = OldDiagnosticResults.at(0);
        results.BearingInfo = BearingType;
        DiagnosticResults.append(results);
    }

    switch (alarmlevel) {
    case -1:
    {
        QString text = ui->txt_normalcount->text();
        if(text.isEmpty()) ui->txt_normalcount->setText("1");
        else{
            int count = text.toInt() + 1;
            ui->txt_normalcount->setText(QString::number(count));
        }
    }
        break;
    case 0:
    {
        QString text = ui->txt_waringcount->text();
        if(text.isEmpty()) ui->txt_waringcount->setText("1");
        else{
            int count = text.toInt() + 1;
            ui->txt_waringcount->setText(QString::number(count));
        }
    }
        break;
    case 1:
    {
        QString text = ui->txt_alarm1count->text();
        if(text.isEmpty()) ui->txt_alarm1count->setText("1");
        else{
            int count = text.toInt() + 1;
            ui->txt_alarm1count->setText(QString::number(count));
        }
    }
        break;
    case 2:
    {
        QString text = ui->txt_alarm2count->text();
        if(text.isEmpty()) ui->txt_alarm2count->setText("1");
        else{
            int count = text.toInt() + 1;
            ui->txt_alarm2count->setText(QString::number(count));
        }
    }
        break;
    }

    //对原始诊断结果进行分析
    QStringList oldresult = OldDiagnosticResults.at(0).split("|");
    QString oldstate;
    for(int i=0;i<shunxu.size();i++){
        quint8 grade = oldresult.at(i).toInt();
        if(grade != 255){
            QString alarmstr = "故障";
            switch (grade) {
            case 0:
                alarmstr = "预警";
                alarmlevel = 0;
                break;
            case 1:
                alarmstr = "一级报警";
                alarmlevel = 1;
                break;
            case 2:
                alarmstr = "二级报警";
                alarmlevel = 2;
                break;
            }
            oldstate += shunxu.at(i) + " " + alarmstr;
        }
    }
    if(oldstate.isEmpty()) oldstate = "无报警";



    if(ret.isEmpty()){
        ret = "无报警";
    }
    ret = ret + "," +oldstate;
    return ret;
}

void FrmAlgorithmOffline::on_btn_reimport_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择文件夹");
    if(dir.isEmpty()) return;
    this->folderPath = dir;
    clearAll();
    LoadFile();
}

void FrmAlgorithmOffline::on_btn_allrun_clicked()
{
#if 0
    if(QMessageBox::question(this,"全部运行","确认全部运行？将耗费很长时间") == QMessageBox::Yes){
        for(int i=0;i<ui->tableWidget->rowCount();i++){
            QString filename = ui->tableWidget->item(i, 0)->text();
            QLineEdit *edit = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(i, 2));
            QLineEdit *edit_old = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(i, 3));
            QString str = RunAlgorithm(filename);
            QStringList list = str.split(",");
            if(list.size() == 2){
                edit_old->setText(list.at(1));
            }
            edit->setText(list.at(0));
        }
    }
#else
    if (QMessageBox::question(this, "全部运行", "确认全部运行？将耗费很长时间") == QMessageBox::Yes) {
            QtConcurrent::run([this]() {
                for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
                    QString filename = ui->tableWidget->item(i, 0)->text();
                    QLineEdit *edit = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(i, 2));
                    QLineEdit *edit_old = qobject_cast<QLineEdit*>(ui->tableWidget->cellWidget(i, 3));

                    QString str = RunAlgorithm(filename);
                    QStringList list = str.split(",");

                    // UI 更新需要在主线程进行
                    QMetaObject::invokeMethod(this, [=]() {
                        if (list.size() == 2) {
                            edit_old->setText(list.at(1));
                        }
                        edit->setText(list.at(0));
                    }, Qt::QueuedConnection);
                }
            });
        }
#endif
}
