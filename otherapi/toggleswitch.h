#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPropertyAnimation>

class ToggleSwitch : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int sliderPos READ sliderPos WRITE setSliderPos)

public:
    explicit ToggleSwitch(QWidget *parent = nullptr)
        : QWidget(parent), m_checked(false), m_sliderPos(2) {
        setFixedSize(50, 25);  // 设定开关大小
    }

    bool isChecked() const { return m_checked; }

    // **新增外部设置 checked 状态的接口**
    void setChecked(bool checked) {
        if (m_checked != checked) {
            m_checked = checked;
//            emit toggled(m_checked);
            animateSlider();
        }
    }

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 绘制背景
        painter.setBrush(m_checked ? QColor(0, 188, 212) : QColor(200, 200, 200));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), height() / 2, height() / 2);

        // 绘制滑块
        painter.setBrush(Qt::white);
        painter.drawEllipse(m_sliderPos, 2, height() - 4, height() - 4);
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_checked = !m_checked;
            emit toggled(m_checked);
            animateSlider();
        }
    }

private:
    bool m_checked;
    int m_sliderPos;

    int sliderPos() const { return m_sliderPos; }
    void setSliderPos(int pos) {
        m_sliderPos = pos;
        update();
    }

    void animateSlider() {
        int endPos = m_checked ? width() - height() + 2 : 2;
        QPropertyAnimation *animation = new QPropertyAnimation(this, "sliderPos");
        animation->setDuration(200);
        animation->setStartValue(m_sliderPos);
        animation->setEndValue(endPos);
        animation->setEasingCurve(QEasingCurve::InOutCubic);
        connect(animation, &QPropertyAnimation::finished, animation, &QObject::deleteLater);
        animation->start();
    }
};

#endif // TOGGLESWITCH_H
