#ifndef PRECONFIG_H
#define PRECONFIG_H

#include <QDialog>
#include <QLineEdit>
#include <QMessageBox>
namespace Ui {
class PreConfig;
}

class PreConfig : public QDialog
{
    Q_OBJECT

public:
    explicit PreConfig(QWidget *parent = nullptr, QString windowstitle = "前置处理器配置");
    ~PreConfig();

private:
    Ui::PreConfig *ui;
    QString title;


public:
    void SetPreDefault(QStringList values);
    void SetBearingDefault(QStringList bearinglist);
    QStringList GetPreInfo();

private slots:
    void on_btn_ok_clicked();
};

#endif // PRECONFIG_H
