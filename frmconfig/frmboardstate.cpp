#include "frmboardstate.h"
#include "ui_frmboardstate.h"
FrmBoardState::FrmBoardState(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmBoardState)
{
    ui->setupUi(this);
    frmInit();
    Wagon = QString();
}

FrmBoardState::~FrmBoardState()
{
    delete ui;
}

void FrmBoardState::frmInit()
{
    QList<QLabel *> labels = findChildren<QLabel *>();

    for(auto lab : labels){
        lab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    txtCommState.append(ui->txt_commID);
    txtCommState.append(ui->txt_commstate);
    txtCommState.append(ui->txt_commselfstate);
    txtCommState.append(ui->txt_commversion);

    txtIOState.append(ui->txt_ioID);
    txtIOState.append(ui->txt_iostate);
    txtIOState.append(ui->txt_ioselfstate);
    txtIOState.append(ui->txt_ioversion);

    txtSpeedState.append(ui->txt_speedID);
    txtSpeedState.append(ui->txt_speedstate);
    txtSpeedState.append(ui->txt_speedselfstate);
    txtSpeedState.append(ui->txt_speedversion);

    txtComputeState.append(ui->txt_computeID);
    txtComputeState.append(ui->txt_computestate);
    txtComputeState.append(ui->txt_computeversion);

    txtNetworkState.append(ui->txt_eth0state);
    txtNetworkState.append(ui->txt_eth1state);

    txtLedState.append(ui->txt_ledrun);
    txtLedState.append(ui->txt_ledpre);
    txtLedState.append(ui->txt_ledone);
    txtLedState.append(ui->txt_ledtwo);
    txtLedState.append(ui->txt_ledpis);

    for(auto txt : txtCommState){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }
    for(auto txt : txtIOState){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }
    for(auto txt : txtSpeedState){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }
    for(auto txt : txtComputeState){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }
    for(auto txt : txtNetworkState){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }
    for(auto txt : txtLedState){
        txt->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        txt->setReadOnly(true);
    }

    stateMap = {
        {"COMM", txtCommState},
        {"IO", txtIOState},
        {"SPEED", txtSpeedState},
        {"COMPUTE", txtComputeState},
        {"NETWORK", txtNetworkState},
        {"LED", txtLedState}
    };

    expectedLengths = {
        {"COMM", 5},
        {"IO", 5},
        {"SPEED", 5},
        {"COMPUTE", 4},
        {"NETWORK", 3},
        {"LED", 6}
    };

    connect(UDPMulticastAPI::Instance(),&UDPMulticastAPI::AllBoardStatereturn,this,&FrmBoardState::UDPDataReturn);
}

void FrmBoardState::ReflushData(QStringList Data, QList<QLineEdit *> txtlist)
{
    if(Data.size() != (txtlist.size() + 1)) return;
    for(int i=1;i<Data.size();i++){
        txtlist.at(i - 1)->setText(Data.at(i));
    }
}

void FrmBoardState::clearData()
{
    for(auto txt : txtCommState){
        txt->clear();
    }
    for(auto txt : txtIOState){
        txt->clear();
    }
    for(auto txt : txtSpeedState){
        txt->clear();
    }
    for(auto txt : txtComputeState){
        txt->clear();
    }
    for(auto txt : txtNetworkState){
        txt->clear();
    }
    for(auto txt : txtLedState){
        txt->clear();
    }
}

void FrmBoardState::RedrawData()
{
    if(AllStateList.isEmpty()) return;
    clearData();
    for(auto Str : AllStateList){
        QStringList list = Str.split("|");
        if(list.isEmpty()) continue;

        const QString& hand = list.at(0);

        if (!expectedLengths.contains(hand)) continue;
        if (list.size() != expectedLengths.value(hand)) continue;

        if (stateMap.contains(hand)) {
            if((hand != "NETWORK") && (hand != "LED")){
                //替换在线状态。0-在线 1-离线
                QString state = list.at(2);
                if(state == "0"){
                    list.replace(2,"在线");
                }else if(state == "1"){
                    list.replace(2,"离线");
                }
            }
            ReflushData(list, stateMap.value(hand));
        }
    }
}

QString FrmBoardState::GetCurrentWagon() const
{
    return this->Wagon;
}

void FrmBoardState::SetCurrentWagon(const QString &wagon)
{
    clearData();
    this->Wagon = wagon;
    UDPMulticastAPI::Instance()->GetAllBoardState(Wagon);
}

void FrmBoardState::UDPDataReturn(QString wagon, QStringList Statelist)
{
    if(wagon != this->Wagon) return;
    AllStateList = Statelist;
    clearData();
    for(auto Str : Statelist){
        QStringList list = Str.split("|");
        if(list.isEmpty()) continue;

        const QString& hand = list.at(0);

        if (!expectedLengths.contains(hand)) continue;
        if (list.size() != expectedLengths.value(hand)) continue;

        if (stateMap.contains(hand)) {
            if((hand != "NETWORK") && (hand != "LED")){
                //替换在线状态。0-在线 1-离线
                QString state = list.at(2);
                if(state == "0"){
                    list.replace(2,"在线");
                }else if(state == "1"){
                    list.replace(2,"离线");
                }
            }

            ReflushData(list, stateMap.value(hand));
        }
    }
}

void FrmBoardState::on_btn_fresh_clicked()
{
    UDPMulticastAPI::Instance()->GetAllBoardState(Wagon);
}
