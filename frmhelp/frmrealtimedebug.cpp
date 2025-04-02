#include "frmrealtimedebug.h"
#include "ui_frmrealtimedebug.h"
#include "udpmulticastapi.h"
#include "corehelper.h"
#include <QMenu>
frmRealTimeDebug::frmRealTimeDebug(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmRealTimeDebug)
{
    ui->setupUi(this);
    pauseecho = false;
    enablefilter = false;
    Init();
}

frmRealTimeDebug::~frmRealTimeDebug()
{
    disconnect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::ReturnEigenvalue,this,&frmRealTimeDebug::ReturnEigenvalue);
    delete ui;
}

void frmRealTimeDebug::Init()
{
    ui->txt_main->installEventFilter(this);
    O_Dimension = QVector<float>();
    D_Dimension = QVector<float>();

    temstate.clear();
    temstate.append(ui->state_temch1);
    temstate.append(ui->state_temch2);
    temstate.append(ui->state_temch3);
    temstate.append(ui->state_temch4);
    temstate.append(ui->state_temch5);

    txt_DDimension.clear();
    txt_ODimension.clear();

    txt_ODimension.append(ui->txt_Omax);
    txt_ODimension.append(ui->txt_Omin);
    txt_ODimension.append(ui->txt_Omean);
    txt_ODimension.append(ui->txt_Ovariance);
    txt_ODimension.append(ui->txt_Odeviation);
    txt_ODimension.append(ui->txt_Orms);
    txt_ODimension.append(ui->txt_Orectifiedmean);
    txt_ODimension.append(ui->txt_Okurtosis);
    txt_ODimension.append(ui->txt_Opeaktopeak);
    txt_ODimension.append(ui->txt_Ocrest_factor);
    txt_ODimension.append(ui->txt_Opeak_factor);
    txt_ODimension.append(ui->txt_Oimpulse_factor);
    txt_ODimension.append(ui->txt_Omargin_factor);

    txt_DDimension.append(ui->txt_Dmax);
    txt_DDimension.append(ui->txt_Dmin);
    txt_DDimension.append(ui->txt_Dmean);
    txt_DDimension.append(ui->txt_Dvariance);
    txt_DDimension.append(ui->txt_Ddeviation);
    txt_DDimension.append(ui->txt_Drms);
    txt_DDimension.append(ui->txt_Drectifiedmean);
    txt_DDimension.append(ui->txt_Dkurtosis);
    txt_DDimension.append(ui->txt_Dpeaktopeak);
    txt_DDimension.append(ui->txt_Dcrest_factor);
    txt_DDimension.append(ui->txt_Dpeak_factor);
    txt_DDimension.append(ui->txt_Dimpulse_factor);
    txt_DDimension.append(ui->txt_Dmargin_factor);

    for(QLineEdit *txt : txt_ODimension){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }

    for(QLineEdit *txt : txt_DDimension){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }


    ui->txt_realtimespeed->setReadOnly(true);
    ui->txt_wagon->setReadOnly(true);
    ui->txt_preid->setReadOnly(true);
    ui->txt_ch->setReadOnly(true);
    ui->txt_speed->setReadOnly(true);
    ui->txt_ambient->setReadOnly(true);
    ui->txt_point->setReadOnly(true);
    ui->txt_time->setReadOnly(true);
    ui->btn_filter->setCheckable(true);
    ui->btn_filter->setChecked(false);
    ui->txt_filter->setVisible(false);
    ui->txt_filter->setPlaceholderText("格式：车厢-前置id-通道,输入完成敲击回车生效。多个测点时用 , 隔开 例如：QN-t0021-1-1,QN-t0021-1-2");
    ui->txt_main->setReadOnly(true);

    for(auto state : temstate){
        state->setStyleSheet("background-color: #55ff00;");
    }


    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::ReturnEigenvalue,this,&frmRealTimeDebug::ReturnEigenvalue);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::RealtimeSpeedUpdate,this,&frmRealTimeDebug::RealtimeSpeedUpdate);
    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::ReturnTemValue,this,&frmRealTimeDebug::ReturnTemValue);


}

void frmRealTimeDebug::UpdateDimensionChart(const QVector<double> &RawData, const QVector<double> &DemodulateData)
{
    QVector<double> O_Data = RawData;
    QVector<double> D_Data = DemodulateData;
    int len = D_Data.size();
    //求和
    double sum = std::accumulate(O_Data.begin(), O_Data.end(), 0.0);
    double sum1 = std::accumulate(D_Data.begin(), D_Data.end(), 0.0);
    //求平均值
    double ave = sum/len;
    double ave1 = sum1/len;

    for(int i=0;i<len;i++){
        O_Data[i] -= ave;//求直流分量
        D_Data[i] -= ave1;
    }

    O_Dimension.clear();
    D_Dimension.clear();

    O_Dimension.clear();
    D_Dimension.clear();

    O_Dimension = CoreHelper::CalculateDimension(O_Data);
    D_Dimension = CoreHelper::CalculateDimension(D_Data);
#if 0
    for(int i=0;i<txt_ODimension.size();i++){
        txt_ODimension[i]->setText(QString::number(O_Dimension.at(i)));
    }
    for(int i=0;i<txt_DDimension.size();i++){
        txt_DDimension[i]->setText(QString::number(D_Dimension.at(i)));
    }
#else
    for(int i=0;i<txt_ODimension.size();i++){
        txt_ODimension[i]->setText(QString::number(RawData.at(i)));
    }
    for(int i=0;i<txt_DDimension.size();i++){
        txt_DDimension[i]->setText(QString::number(DemodulateData.at(i)));
    }
#endif
}

void frmRealTimeDebug::UpdateAlarmInfo(QStringList alarmlist)
{
    if(alarmlist.isEmpty()){
        ui->lab_alarm->setText("无报警信息");
        ui->lab_alarm->setStyleSheet("color:green;");
    }else{
        ui->lab_alarm->setText(alarmlist.join(","));
        ui->lab_alarm->setStyleSheet("color:red;");
    }
}

bool frmRealTimeDebug::InFilterList(QString wagon, qint8 id, qint8 ch, bool temmodel)
{
    if(!enablefilter) return true;
    QString newinfo = QString("%1-%2-%3").arg(wagon).arg(id).arg(ch);
    for(auto str : filterlist){
        if(temmodel){//温度模式只匹配车厢和ID
            QStringList filter = str.split("-");
            if(filter.size() < 2) continue;
            if((filter.at(0) == wagon) && (filter.at(1).toInt() == id)){
                return true;
            }
        }else{
            if(str == newinfo) return true;
        }
    }
    return false;
}

void frmRealTimeDebug::ReturnEigenvalue(QString Wagon, qint8 id, qint8 ch, quint32 speed, double AmbientTem, double PointTem,
                                        QString time, QStringList Dimensional, QStringList Demodulated, QStringList alarmlist,
                                        quint8 rmsalarmgrade, quint8 ppalarmgrade)
{
    if(pauseecho) return;
    if(!InFilterList(Wagon,id,ch)) return;
    //更新状态栏
    ui->txt_wagon->setText(Wagon);
    ui->txt_preid->setText(QString::number(id));
    ui->txt_ch->setText(QString::number(ch));
    ui->txt_speed->setText(QString::number(speed));
    ui->txt_ambient->setText(QString::number(AmbientTem));
    ui->txt_point->setText(QString::number(PointTem));
    ui->txt_time->setText(time);

    //更新报警信息
    QStringList AlarmInfo;
    QStringList Contents;
    Contents << "温度" << "保外" << "保内" << "外环" << "内环" << "滚单" << "大齿轮" << "小齿轮" << "踏面";
    for(int i=0; i<alarmlist.size();i++){
        QString str = alarmlist.at(i);
        uint8_t state = str.toInt();
        if(state != 0xff){
            QString grade;
            if(state == 0){
                grade = "预警";
            }else if(state == 1){
                grade = "一级报警";
            }else if(state == 2){
                grade = "二级报警";
            }
            AlarmInfo.append(Contents.at(i) + grade);
        }
    }
    if(rmsalarmgrade > 0){
        AlarmInfo.append(QString("RMS值%1级报警").arg(rmsalarmgrade));
    }
    if(ppalarmgrade > 0){
        AlarmInfo.append(QString("PP值%1级报警").arg(ppalarmgrade));
    }
    UpdateAlarmInfo(AlarmInfo);

    //提取原始量纲
    QVector<double> DimensionValus;
    for(const QString str : Dimensional){
        DimensionValus.append(str.toFloat());
    }
    //提取包络量纲
    QVector<double> DemodulatedValus;
    for(const QString str : Demodulated){
        DemodulatedValus.append(str.toFloat());
    }
    //更新量纲
    UpdateDimensionChart(DimensionValus,DemodulatedValus);

    //添加内容
    QString str;
    str += time + " ";
    str += Wagon + "-";
    str += QString::number(id) + "-";
    str += QString::number(ch) + "： ";
    str += "转速(" + QString::number(speed) + ") ";
    str += "环境温度(" + QString::number(AmbientTem) + ") ";
    str += "测点温度(" + QString::number(PointTem) + ") ";
    str += "原始峰峰值(" + QString::number(DimensionValus.at(8)) + ") ";
    str += "包络峰峰值(" + QString::number(DemodulatedValus.at(8)) + "); ";

    if(!AlarmInfo.isEmpty()){
        str += "\n";
        str += AlarmInfo.join(",");
    }
    ui->txt_main->append(str);
}

void frmRealTimeDebug::RealtimeSpeedUpdate(quint32 speed)
{
    ui->txt_realtimespeed->setText(QString::number(speed));

    double kmh =  static_cast<double>(speed) * 0.154566228;
    if(kmh > 20){
        ui->lab_runmode->setText("综合巡检模式");
    }else if(kmh > 5){
        ui->lab_runmode->setText("温度巡检模式");
    }else{
        ui->lab_runmode->setText("低功耗模式");
    }
}

void frmRealTimeDebug::ReturnTemValue(QString Wagon, qint8 id, QString time, qint8 ChannelCount, QStringList State)
{
    if(pauseecho) return;
    if(!InFilterList(Wagon,id,0,true)) return;
    ui->txt_wagon->setText(Wagon);
    ui->txt_temid->setText(QString::number(id));
    ui->txt_time->setText(time);

    if(ChannelCount > 5){
        ChannelCount = 5;
    }
    QStringList alarminfo;
    for(int i=0;i<ChannelCount;i++){
        quint8 state = State[i].toInt();
        if(state == 0xFF){
            temstate.at(i)->setStyleSheet("background-color: #55ff00;");
        }else if(state == 0x01){
            alarminfo.append(QString("通道%1预警").arg(i));
            temstate.at(i)->setStyleSheet("background-color: #ffff00;");
        }else if(state == 0x02){
            alarminfo.append(QString("通道%1一级报警").arg(i));
            temstate.at(i)->setStyleSheet("background-color: #ff0000;");
        }
    }
    if(alarminfo.isEmpty()){
        alarminfo.append("无报警");
    }

    //添加内容
    QString str;
    str += time + " ";
    str += Wagon + "-";
    str += QString::number(id) + "： ";
    str += alarminfo.join(",");
    ui->txt_main->append(str);
}
#if 1
void frmRealTimeDebug::contextMenuEvent(QContextMenuEvent *event)
{
    // 获取当前鼠标事件发生的控件
    QTextEdit *textEdit = qobject_cast<QTextEdit *>(childAt(event->pos()));
    if (textEdit) {
        QMenu menu(this); // 创建菜单

        // 获取默认的右键菜单（包括剪切、复制等）
        menu.addActions(textEdit->createStandardContextMenu()->actions());

        // 创建"清空"菜单项
        QAction *clearAction = new QAction("清空", this);
        connect(clearAction, &QAction::triggered, textEdit, &QTextEdit::clear);
        menu.addAction(clearAction);

        // 显示菜单
        menu.exec(event->globalPos());
    }
}
#else
bool frmRealTimeDebug::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu) {
        QTextEdit *textEdit = qobject_cast<QTextEdit *>(watched);
        if (textEdit) {
            QContextMenuEvent *contextEvent = static_cast<QContextMenuEvent *>(event);
            QMenu menu(textEdit);  // 创建右键菜单

            // 获取默认的右键菜单（包括剪切、复制、粘贴等）
            menu.addActions(textEdit->createStandardContextMenu()->actions());

            // 创建"清空"菜单项
            QAction *clearAction = new QAction("清空", textEdit);
            connect(clearAction, &QAction::triggered, textEdit, &QTextEdit::clear);
            menu.addAction(clearAction);

            // 显示菜单
            menu.exec(contextEvent->globalPos());

            return true;
        }
    }
    return QDialog::eventFilter(watched, event);
}
#endif

void frmRealTimeDebug::on_btn_pause_clicked()
{
    QString text = ui->btn_pause->text();
    if(text == "暂停刷新"){
        pauseecho = true;
        ui->btn_pause->setText("继续刷新");
    }else{
        pauseecho = false;
        ui->btn_pause->setText("暂停刷新");
    }
}

void frmRealTimeDebug::on_btn_clear_clicked()
{
    pauseecho = true;
    ui->txt_main->clear();
    QList <QLineEdit *> txts = findChildren<QLineEdit*>();
    for(auto txt : txts){
        txt->clear();
    }
    for(auto state : temstate){
        state->setStyleSheet("background-color: #55ff00;");
    }
    pauseecho = false;
}

void frmRealTimeDebug::on_btn_filter_clicked(bool checked)
{
    enablefilter = checked;
    ui->txt_filter->setVisible(checked);
}

void frmRealTimeDebug::on_txt_filter_returnPressed()
{
    filterlist.clear();
    QString text = ui->txt_filter->text();
    filterlist = text.split(",");
}
