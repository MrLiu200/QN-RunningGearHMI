#ifndef INDICATORLIGHT_H
#define INDICATORLIGHT_H

#include <QWidget>

class IndicatorLight : public QWidget
{
    Q_OBJECT
public:
    enum AlarmLevel{
        Normal = 0,
        Warning,
        Level_1,
        Level_2
    };

    explicit IndicatorLight(QWidget *parent = nullptr);

    void SetLevel(const AlarmLevel &newlevel);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    AlarmLevel level;

signals:

};

#endif // INDICATORLIGHT_H
