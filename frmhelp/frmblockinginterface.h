#ifndef FRMBLOCKINGINTERFACE_H
#define FRMBLOCKINGINTERFACE_H

#include <QDialog>
#include <QTimer>
#include <QObject>
namespace Ui {
class frmBlockingInterface;
}

class frmBlockingInterface : public QDialog
{
    Q_OBJECT

public:
    static frmBlockingInterface *Instance();
    explicit frmBlockingInterface(QWidget *parent = nullptr);
    ~frmBlockingInterface();

    void StartShow();
    void SetLabelText(QString text);
    void Stop();
    void SetLabelColor(QString color);
    void SetFalse(QString colorHtml = "#ff0000",QString text = "失败");
private:
    Ui::frmBlockingInterface *ui;
    static frmBlockingInterface *self;
    QString lab_Text;
    QTimer *timerSetColor;
    QTimer *timerCount;
    QString FailColor;
    int Sec;
    void InitFrm();
    bool colorisfail;

private slots:
    void SwitchColor();
    void Updatetime();
    void on_btn_close_clicked();

signals:
    void closeview();
};

#endif // FRMBLOCKINGINTERFACE_H
