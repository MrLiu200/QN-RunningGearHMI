QT       += core gui widgets network sql printsupport concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 debug

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
TARGET              = HMI
TEMPLATE            = app
MOC_DIR             = temp/moc
RCC_DIR             = temp/rcc
UI_DIR              = temp/ui
OBJECTS_DIR         = temp/obj
DESTDIR             = bin
RC_FILE             = image/main.rc
#QT_QPA_PLATFORM=windows:fontengine=freetype
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

#开启控制台
#CONFIG += console

INCLUDEPATH         += $$PWD/frmchart
INCLUDEPATH         += $$PWD/improve
INCLUDEPATH         += $$PWD/frmhelp
INCLUDEPATH         += $$PWD/frmuser
INCLUDEPATH         += $$PWD/qcustomplot
INCLUDEPATH         += $$PWD/frmlog
INCLUDEPATH         += $$PWD/frmconfig
INCLUDEPATH         += $$PWD/networkapi
INCLUDEPATH         += $$PWD/api
INCLUDEPATH         += $$PWD/otherapi
INCLUDEPATH         += $$PWD/fftw
INCLUDEPATH         += $$PWD/eigen_3.4.0
INCLUDEPATH         += $$PWD/quazip/include

include             ($$PWD/frmchart/frmchart.pri)
include             ($$PWD/improve/improve.pri)
include             ($$PWD/frmhelp/frmhelp.pri)
include             ($$PWD/frmuser/frmuser.pri)
include             ($$PWD/qcustomplot/qcustomplot.pri)
include             ($$PWD/frmlog/frmlog.pri)
include             ($$PWD/frmconfig/frmconfig.pri)
include             ($$PWD/networkapi/networkapi.pri)
include             ($$PWD/api/api.pri)
include             ($$PWD/otherapi/otherapi.pri)
include             ($$PWD/fftw/fftw.pri)
include             ($$PWD/xlsx/qtxlsx.pri)
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Image.qrc
#QMAKE_CXXFLAGS += -fRTTI
LIBS += -L$$PWD/quazip/winlib/ -lquazipd
