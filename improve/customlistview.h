#ifndef CUSTOMLISTVIEW_H
#define CUSTOMLISTVIEW_H

#include <QListView>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>

class CustomListView : public QListView {
    Q_OBJECT

public:
    explicit CustomListView(QWidget *parent = nullptr);

protected:
    QMimeData* mimeData(const QList<QModelIndex>& indexes) const; // 拖动数据
    void dragEnterEvent(QDragEnterEvent *event) override; // 拖动进入事件
    void dropEvent(QDropEvent *event) override; // 拖放事件
    void startDrag(Qt::DropActions supportedActions) override;

private:
    QString SelectText;     //当前选中的文本
    bool EnableSelect;

public:
    QStringList onGetSelectedItems();

private slots:
    void showContextMenu(const QPoint &pos); // 显示上下文菜单
    void onItemDoubleClicked(const QModelIndex &index);     //双击选项
    void selectAllItems();                                  //全部选中
    void deselectAllItems();                                //全部取消选中


signals:
    void ContextMenuSelect(QString ItemText,int MenuType);  //右键上下文菜单选中 参数：项文本、菜单类型
    void TriggerItem(QString text);
    //打开趋势图
    void OpenTrend(QString deviceInfo,bool IsEnvelope);
    //打开波形图
    void OpenWaveform(QString deviceInfo,bool IsEnvelope);
    //清除报警
    void clearAlarm(QString text);

};

#endif // CUSTOMLISTVIEW_H
