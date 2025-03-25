#ifndef M_SCREENSHOT_H
#define M_SCREENSHOT_H

#include <QWidget>

class M_Screenshot : public QWidget
{
    Q_OBJECT
public:
    explicit M_Screenshot(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QPoint startPoint, endPoint;

signals:
    void areaSelected(const QRect &rect);
};

class ScreenshotWindow : public QWidget {
    Q_OBJECT
public:
    ScreenshotWindow(const QPixmap &pixmap,QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
private:
    QPixmap originalPixmap;
    bool dragging = false;
    QPoint lastMousePos;
    qreal scaleFactor;
    QPointF offset;

private slots:
    void SaveImage();
};


class ScreenshotApp : public QObject{
    Q_OBJECT
public:
    ScreenshotApp(QObject *parent = nullptr);
    void startCapture();

private:
    M_Screenshot *overlay;

private slots:
     void captureScreen(const QRect &selectedArea);
};



#endif // M_SCREENSHOT_H
