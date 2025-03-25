#include "predata.h"

QList<PreData::BEARINGINFO> PreData::bearinglist = QList<PreData::BEARINGINFO>();
QList<PreData::DEVICEINFO> PreData::devicelist = QList<PreData::DEVICEINFO>();
QList<QString> PreData::DeviceState = QList<QString>();
QList<QString> PreData::wagonsList = QList<QString>();
QList<PreData::PKDATASTRUCT> PreData::pkdatalist = QList<PreData::PKDATASTRUCT>();
int PreData::PKListSizeMax = 10;




int PreData::DeviceInfo_Count = 0;
QList<QString> PreData::DeviceInfo_WagonName = QList<QString>();
QList<int> PreData::DeviceInfo_DeviceID = QList<int>();
QList<int> PreData::DeviceInfo_DeviceCH = QList<int>();
QList<QString> PreData::DeviceInfo_DeviceName = QList<QString>();
QList<QString> PreData::DeviceInfo_DeviceType = QList<QString>();
QList<float> PreData::DeviceInfo_Sensitivity = QList<float>();
QList<float> PreData::DeviceInfo_ShaftDiameter = QList<float>();
QList<QString> PreData::DeviceInfo_bearing1Name = QList<QString>();
QList<QString> PreData::DeviceInfo_bearing1_model = QList<QString>();
QList<QString> PreData::DeviceInfo_bearing2Name = QList<QString>();
QList<QString> PreData::DeviceInfo_bearing2_model = QList<QString>();
QList<QString> PreData::DeviceInfo_bearing3Name = QList<QString>();
QList<QString> PreData::DeviceInfo_bearing3_model = QList<QString>();
QList<QString> PreData::DeviceInfo_bearing4Name = QList<QString>();
QList<QString> PreData::DeviceInfo_bearing4_model = QList<QString>();
QList<QString> PreData::DeviceInfo_capstanName = QList<QString>();
QList<int> PreData::DeviceInfo_capstanTeethNum = QList<int>();
QList<QString> PreData::DeviceInfo_DrivenwheelName = QList<QString>();
QList<int> PreData::DeviceInfo_DrivenwheelTeethNum = QList<int>();
QList<bool> PreData::DeviceInfo_IsEnable = QList<bool>();
QList<QString> PreData::DeviceInfo_version = QList<QString>();
