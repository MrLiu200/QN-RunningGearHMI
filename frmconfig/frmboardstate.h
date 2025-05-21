#ifndef FRMBOARDSTATE_H
#define FRMBOARDSTATE_H

#include <QWidget>
#include <QLineEdit>
#include "udpmulticastapi.h"
namespace Ui {
class FrmBoardState;
}

class FrmBoardState : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString Wagon READ GetCurrentWagon WRITE SetCurrentWagon);

public:
    explicit FrmBoardState(QWidget *parent = nullptr);
    ~FrmBoardState();

private:
    Ui::FrmBoardState *ui;
    QList<QLineEdit *> txtCommState;
    QList<QLineEdit *> txtIOState;
    QList<QLineEdit *> txtSpeedState;
    QList<QLineEdit *> txtComputeState;
    QList<QLineEdit *> txtNetworkState;
    QList<QLineEdit *> txtLedState;

    QStringList AllStateList;
    QString Wagon;

    QMap<QString, QList<QLineEdit*>> stateMap;
    QMap<QString, int> expectedLengths;
private:
    void frmInit(void);
    void ReflushData(QStringList Data, QList<QLineEdit *> txtlist);
    void clearData();

public:
    void RedrawData();
    QString GetCurrentWagon() const;
    void SetCurrentWagon(const QString &wagon);

private slots:
    void UDPDataReturn(QString wagon, QStringList Statelist);
    void on_btn_fresh_clicked();
};

#endif // FRMBOARDSTATE_H
