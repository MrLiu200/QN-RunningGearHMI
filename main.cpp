#include "mainwindow.h"
#include <QApplication>
#include <QToolTip>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<PreData::EnumDeviceState>("PreData::EnumDeviceState");
//    qputenv("QT_QPA_PLATFORM", QByteArray("windows:fontengine=freetype"));
    MainWindow w;
//    w.show();
    w.showMaximized();
    return a.exec();
}
