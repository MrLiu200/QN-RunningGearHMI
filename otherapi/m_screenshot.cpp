#include "m_screenshot.h"
#include <QMouseEvent>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include <QAction>
#include <QMenu>

M_Screenshot::M_Screenshot(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setWindowOpacity(0.3);
    setCursor(Qt::CrossCursor);
    setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    showFullScreen();
}

void M_Screenshot::mouseMoveEvent(QMouseEvent *event)
{
    endPoint = event->pos();
    update();
}

void M_Screenshot::mousePressEvent(QMouseEvent *event)
{
    startPoint = event->pos();
    endPoint = startPoint;
    update();
}

void M_Screenshot::mouseReleaseEvent(QMouseEvent *event)
{
    endPoint = event->pos();
    update();
    QRect selectedArea = QRect(startPoint, endPoint).normalized();
    hide();
    emit areaSelected(selectedArea);
}

void M_Screenshot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(Qt::red);
    painter.drawRect(QRect(startPoint, endPoint));
}

ScreenshotWindow::ScreenshotWindow(const QPixmap &pixmap, QWidget *parent) : QWidget(parent),
    originalPixmap(pixmap)
{
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    setWindowTitle("截图窗口(右键保存图片)");
    scaleFactor = 1.0;
    offset = QPoint(0, 0);
    setFixedSize(pixmap.size());
    setMouseTracking(true);
}

void ScreenshotWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);  // 背景填充黑色

    QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPointF drawPoint = offset + (rect().center() - scaledPixmap.rect().center());
    painter.drawPixmap(drawPoint, scaledPixmap);

    // 绘制窗口边框
    painter.setPen(Qt::black);
    painter.drawRect(0, 0, width() - 1, height() - 1);
}

void ScreenshotWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *actionScreenshot = menu.addAction("Save");
    connect(actionScreenshot, &QAction::triggered, this, &ScreenshotWindow::SaveImage);

    menu.exec(event->globalPos());  // 右键菜单在鼠标位置显示
}

void ScreenshotWindow::SaveImage()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存截图"), "", tr("Images (*.png *.jpg *.bmp *.jpeg)"));
    if (!filePath.isEmpty()) {
        QString selectedFormat = QFileInfo(filePath).suffix();
        if (selectedFormat.isEmpty()) {//若没有手输，则默认png
                filePath += ".png";
                selectedFormat = "png";
            }
        // 保存图片到指定路径
        if (!originalPixmap.save(filePath, selectedFormat.toUtf8().constData(), 100)) {
            QMessageBox::warning(this, tr("保存失败"), tr("无法保存图片。"));
        }
    }
}

ScreenshotApp::ScreenshotApp(QObject *parent) : QObject(parent)
{

}

void ScreenshotApp::startCapture()
{
    overlay = new M_Screenshot();
    connect(overlay, &M_Screenshot::areaSelected, this, &ScreenshotApp::captureScreen);
}

void ScreenshotApp::captureScreen(const QRect &selectedArea)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen || selectedArea.isEmpty()) return;

    QPixmap screenshot = screen->grabWindow(0, selectedArea.x(), selectedArea.y(), selectedArea.width(), selectedArea.height());

    // 显示带边框的截图窗口
    ScreenshotWindow *screenshotWindow = new ScreenshotWindow(screenshot);
    screenshotWindow->show();
}
