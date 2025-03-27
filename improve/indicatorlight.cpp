#include "indicatorlight.h"
#include <QPainter>
IndicatorLight::IndicatorLight(QWidget *parent) : QWidget(parent)
{
    level = AlarmLevel::Normal;
}

void IndicatorLight::SetLevel(const IndicatorLight::AlarmLevel &newlevel)
{
    this->level = newlevel;
    update();
}

void IndicatorLight::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // 计算圆的大小和位置
    int diameter = qMin(width(), height()) * 0.8;  // 80% 窗口大小
    int xOffset = (width() - diameter) / 2;
    int yOffset = (height() - diameter) / 2;
    QRect circleRect(xOffset, yOffset, diameter, diameter);

    // 根据报警等级选择颜色
    QColor color;
    switch (level) {
    case Warning: color = Qt::yellow; break;
    case Level_1: color = QColor(255, 165, 0); break;
    case Level_2: color = Qt::red; break;
    default: color = Qt::green; break;
    }

    // 画填充的圆
    painter.setBrush(color);
    painter.setPen(Qt::black);
    painter.drawEllipse(circleRect);

}
