#include "frmdbchart.h"
#include "ui_frmdbchart.h"

frmDBChart::frmDBChart(const QStringList &DBResultsList, const QString BearingType, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmDBChart),
    dblist(DBResultsList),
    BearingModel(BearingType)
{
    ui->setupUi(this);

    setFixedSize(700,800);
    setWindowTitle("数据DB值");
    frmInit();
    UpdateChart();

}

frmDBChart::~frmDBChart()
{
    delete ui;
}

void frmDBChart::closeEvent(QCloseEvent *event)
{
    Q_EMIT closedSignal();
    QDialog::closeEvent(event);
}

void frmDBChart::frmInit()
{

    txt_baowai.clear();
    txt_baowai.append(ui->txt_baowai1jie);
    txt_baowai.append(ui->txt_baowai2jie);
    txt_baowai.append(ui->txt_baowai3jie);
    txt_baowai.append(ui->txt_baowai4jie);
    txt_baowai.append(ui->txt_baowai5jie);

    txt_baonei.clear();
    txt_baonei.append(ui->txt_baonei1jie);
    txt_baonei.append(ui->txt_baonei2jie);
    txt_baonei.append(ui->txt_baonei3jie);
    txt_baonei.append(ui->txt_baonei4jie);
    txt_baonei.append(ui->txt_baonei5jie);

    txt_waihuan.clear();
    txt_waihuan.append(ui->txt_waihuan1jie);
    txt_waihuan.append(ui->txt_waihuan2jie);
    txt_waihuan.append(ui->txt_waihuan3jie);
    txt_waihuan.append(ui->txt_waihuan4jie);
    txt_waihuan.append(ui->txt_waihuan5jie);

    txt_neihuan.clear();
    txt_neihuan.append(ui->txt_neihuan1jie);
    txt_neihuan.append(ui->txt_neihuan2jie);
    txt_neihuan.append(ui->txt_neihuan3jie);
    txt_neihuan.append(ui->txt_neihuan4jie);
    txt_neihuan.append(ui->txt_neihuan5jie);

    txt_gundan.clear();
    txt_gundan.append(ui->txt_gundan1jie);
    txt_gundan.append(ui->txt_gundan2jie);
    txt_gundan.append(ui->txt_gundan3jie);
    txt_gundan.append(ui->txt_gundan4jie);
    txt_gundan.append(ui->txt_gundan5jie);

    txt_baowai_2.clear();
    txt_baowai_2.append(ui->txt_baowai1jie_2);
    txt_baowai_2.append(ui->txt_baowai2jie_2);
    txt_baowai_2.append(ui->txt_baowai3jie_2);
    txt_baowai_2.append(ui->txt_baowai4jie_2);
    txt_baowai_2.append(ui->txt_baowai5jie_2);

    txt_baonei_2.clear();
    txt_baonei_2.append(ui->txt_baonei1jie_2);
    txt_baonei_2.append(ui->txt_baonei2jie_2);
    txt_baonei_2.append(ui->txt_baonei3jie_2);
    txt_baonei_2.append(ui->txt_baonei4jie_2);
    txt_baonei_2.append(ui->txt_baonei5jie_2);

    txt_waihuan_2.clear();
    txt_waihuan_2.append(ui->txt_waihuan1jie_2);
    txt_waihuan_2.append(ui->txt_waihuan2jie_2);
    txt_waihuan_2.append(ui->txt_waihuan3jie_2);
    txt_waihuan_2.append(ui->txt_waihuan4jie_2);
    txt_waihuan_2.append(ui->txt_waihuan5jie_2);

    txt_neihuan_2.clear();
    txt_neihuan_2.append(ui->txt_neihuan1jie_2);
    txt_neihuan_2.append(ui->txt_neihuan2jie_2);
    txt_neihuan_2.append(ui->txt_neihuan3jie_2);
    txt_neihuan_2.append(ui->txt_neihuan4jie_2);
    txt_neihuan_2.append(ui->txt_neihuan5jie_2);

    txt_gundan_2.clear();
    txt_gundan_2.append(ui->txt_gundan1jie_2);
    txt_gundan_2.append(ui->txt_gundan2jie_2);
    txt_gundan_2.append(ui->txt_gundan3jie_2);
    txt_gundan_2.append(ui->txt_gundan4jie_2);
    txt_gundan_2.append(ui->txt_gundan5jie_2);

    txt_baowai_3.clear();
    txt_baowai_3.append(ui->txt_baowai1jie_3);
    txt_baowai_3.append(ui->txt_baowai2jie_3);
    txt_baowai_3.append(ui->txt_baowai3jie_3);
    txt_baowai_3.append(ui->txt_baowai4jie_3);
    txt_baowai_3.append(ui->txt_baowai5jie_3);

    txt_baonei_3.clear();
    txt_baonei_3.append(ui->txt_baonei1jie_3);
    txt_baonei_3.append(ui->txt_baonei2jie_3);
    txt_baonei_3.append(ui->txt_baonei3jie_3);
    txt_baonei_3.append(ui->txt_baonei4jie_3);
    txt_baonei_3.append(ui->txt_baonei5jie_3);

    txt_waihuan_3.clear();
    txt_waihuan_3.append(ui->txt_waihuan1jie_3);
    txt_waihuan_3.append(ui->txt_waihuan2jie_3);
    txt_waihuan_3.append(ui->txt_waihuan3jie_3);
    txt_waihuan_3.append(ui->txt_waihuan4jie_3);
    txt_waihuan_3.append(ui->txt_waihuan5jie_3);

    txt_neihuan_3.clear();
    txt_neihuan_3.append(ui->txt_neihuan1jie_3);
    txt_neihuan_3.append(ui->txt_neihuan2jie_3);
    txt_neihuan_3.append(ui->txt_neihuan3jie_3);
    txt_neihuan_3.append(ui->txt_neihuan4jie_3);
    txt_neihuan_3.append(ui->txt_neihuan5jie_3);

    txt_gundan_3.clear();
    txt_gundan_3.append(ui->txt_gundan1jie_3);
    txt_gundan_3.append(ui->txt_gundan2jie_3);
    txt_gundan_3.append(ui->txt_gundan3jie_3);
    txt_gundan_3.append(ui->txt_gundan4jie_3);
    txt_gundan_3.append(ui->txt_gundan5jie_3);

    txt_baowai_4.clear();
    txt_baowai_4.append(ui->txt_baowai1jie_4);
    txt_baowai_4.append(ui->txt_baowai2jie_4);
    txt_baowai_4.append(ui->txt_baowai3jie_4);
    txt_baowai_4.append(ui->txt_baowai4jie_4);
    txt_baowai_4.append(ui->txt_baowai5jie_4);

    txt_baonei_4.clear();
    txt_baonei_4.append(ui->txt_baonei1jie_4);
    txt_baonei_4.append(ui->txt_baonei2jie_4);
    txt_baonei_4.append(ui->txt_baonei3jie_4);
    txt_baonei_4.append(ui->txt_baonei4jie_4);
    txt_baonei_4.append(ui->txt_baonei5jie_4);

    txt_waihuan_4.clear();
    txt_waihuan_4.append(ui->txt_waihuan1jie_4);
    txt_waihuan_4.append(ui->txt_waihuan2jie_4);
    txt_waihuan_4.append(ui->txt_waihuan3jie_4);
    txt_waihuan_4.append(ui->txt_waihuan4jie_4);
    txt_waihuan_4.append(ui->txt_waihuan5jie_4);

    txt_neihuan_4.clear();
    txt_neihuan_4.append(ui->txt_neihuan1jie_4);
    txt_neihuan_4.append(ui->txt_neihuan2jie_4);
    txt_neihuan_4.append(ui->txt_neihuan3jie_4);
    txt_neihuan_4.append(ui->txt_neihuan4jie_4);
    txt_neihuan_4.append(ui->txt_neihuan5jie_4);

    txt_gundan_4.clear();
    txt_gundan_4.append(ui->txt_gundan1jie_4);
    txt_gundan_4.append(ui->txt_gundan2jie_4);
    txt_gundan_4.append(ui->txt_gundan3jie_4);
    txt_gundan_4.append(ui->txt_gundan4jie_4);
    txt_gundan_4.append(ui->txt_gundan5jie_4);

    txt_dachilun.clear();
    txt_dachilun.append(ui->txt_dachilun1jie);
    txt_dachilun.append(ui->txt_dachilun2jie);
    txt_dachilun.append(ui->txt_dachilun3jie);
    txt_dachilun.append(ui->txt_dachilun4jie);
    txt_dachilun.append(ui->txt_dachilun5jie);

    txt_xiaochilun.clear();
    txt_xiaochilun.append(ui->txt_xiaochilun1jie);
    txt_xiaochilun.append(ui->txt_xiaochilun2jie);
    txt_xiaochilun.append(ui->txt_xiaochilun3jie);
    txt_xiaochilun.append(ui->txt_xiaochilun4jie);
    txt_xiaochilun.append(ui->txt_xiaochilun5jie);

    txt_tamian.clear();
    txt_tamian.append(ui->txt_tamian1jie);
    txt_tamian.append(ui->txt_tamian2jie);
    txt_tamian.append(ui->txt_tamian3jie);
    txt_tamian.append(ui->txt_tamian4jie);
    txt_tamian.append(ui->txt_tamian5jie);

    Fault_Type.clear();
    Fault_Type << "保外" << "保内" << "外环" << "内环" << "滚单" << "大齿轮" << "小齿轮" << "踏面";

    txt_aggregate.clear();
    txt_aggregate.append(txt_baowai);
    txt_aggregate.append(txt_baonei);
    txt_aggregate.append(txt_waihuan);
    txt_aggregate.append(txt_neihuan);
    txt_aggregate.append(txt_gundan);

    txt_aggregate.append(txt_baowai_2);
    txt_aggregate.append(txt_baonei_2);
    txt_aggregate.append(txt_waihuan_2);
    txt_aggregate.append(txt_neihuan_2);
    txt_aggregate.append(txt_gundan_2);

    txt_aggregate.append(txt_baowai_3);
    txt_aggregate.append(txt_baonei_3);
    txt_aggregate.append(txt_waihuan_3);
    txt_aggregate.append(txt_neihuan_3);
    txt_aggregate.append(txt_gundan_3);

    txt_aggregate.append(txt_baowai_4);
    txt_aggregate.append(txt_baonei_4);
    txt_aggregate.append(txt_waihuan_4);
    txt_aggregate.append(txt_neihuan_4);
    txt_aggregate.append(txt_gundan_4);

    txt_aggregate.append(txt_dachilun);
    txt_aggregate.append(txt_xiaochilun);
    txt_aggregate.append(txt_tamian);

    groupList.clear();
    groupList.append(ui->group_bearing1);
    groupList.append(ui->group_bearing2);
    groupList.append(ui->group_bearing3);
    groupList.append(ui->group_bearing4);

    for(auto txt : txt_aggregate){
        for(auto lab : txt){
            lab->setReadOnly(true);
        }
    }
}

void frmDBChart::UpdateChart()
{
    if(BearingModel.isEmpty()) return;
    QStringList list = BearingModel.split("|");
    int len = list.size();
    if(len > 4){
        len = 4;
    }
    for(int i=0;i<len;i++){
        QString str = list.at(i);
        groupList.at(i)->setTitle(str);
    }
    FlushData();
}

void frmDBChart::FlushData()
{
    ClearChart();
    int len = dblist.size();
    if(len < 2) return;
    for(int i=1;i<len;i++){
        //格式：轴承编号(1,2,3,4)|故障位置|报警等级|db值(5阶用,分割)
        QStringList list = dblist.at(i).split("|");
        if(list.size() < 4) continue;
        int bearingindex = list.at(0).toInt();
        if(bearingindex > 4) continue;
        QString Faulttype = list.at(1);
        QStringList newdb = list.at(3).split(",");
        //如果是轴承
        if(bearingindex!=0){
            int txt_index = (bearingindex-1) * 5;//找到txt的索引 0-4 轴承1 ，5-9 轴承2 10-14 轴承3 15-19 轴承5
            //找出对应类型
            int fault_index = Fault_Type.indexOf(Faulttype);
            //解析db
            int len = newdb.size();
            if(len > 5) len = 5;
            for(int i=0;i<len;i++){
                QString db = newdb.at(i);
                txt_aggregate.at(txt_index+fault_index).at(i)->setText(db);
            }
        }else{//大齿轮、小齿轮、踏面
            //找出对应类型
            int fault_index = Fault_Type.indexOf(Faulttype);
            //解析db
            int len = newdb.size();
            if(len > 5) len = 5;
            for(int i=0;i<len;i++){
                QString db = newdb.at(i);
                txt_aggregate.at(15+fault_index).at(i)->setText(db);
            }
        }
    }
}

void frmDBChart::ClearChart()
{
    for(auto txt : txt_aggregate){
        for(auto lab : txt){
//            lab->clear();
            lab->setText("00.00");
        }
    }
}

void frmDBChart::UpdateData(const QStringList &DBResultsList, const QString BearingType)
{
    this->dblist.clear();
    dblist = DBResultsList;
    this->BearingModel.clear();
    BearingModel = BearingType;
    UpdateChart();
}
