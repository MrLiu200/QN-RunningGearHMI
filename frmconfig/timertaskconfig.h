#ifndef TIMERTASKCONFIG_H
#define TIMERTASKCONFIG_H

#include <QDialog>

namespace Ui {
class TimerTaskConfig;
}

class TimerTaskConfig : public QDialog
{
    Q_OBJECT

public:
    explicit TimerTaskConfig(QWidget *parent = nullptr, QString windowstitle = "定时任务配置");
    ~TimerTaskConfig();

private:
    Ui::TimerTaskConfig *ui;
    QString title;
public:
    QStringList GetTaskInfo();
    void SetDefaultInfo(QString action);
};

#endif // TIMERTASKCONFIG_H
