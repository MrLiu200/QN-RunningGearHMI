#ifndef DROPPABLETABWIDGET_H
#define DROPPABLETABWIDGET_H
#include <QTabWidget>
#include <QDragEnterEvent>
#include <QMimeData>
#include <frmlivepreview.h>
class DroppableTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    DroppableTabWidget(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    bool isTabExist(QString &tabName);

private slots:
    void showTabContextMenu(const QPoint &pos);
    void deleteTab(int tabindex);
};

#endif // DROPPABLETABWIDGET_H
