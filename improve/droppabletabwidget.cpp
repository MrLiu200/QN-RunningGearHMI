#include "droppabletabwidget.h"
#include "frmhistoricaltrends.h"
DroppableTabWidget::DroppableTabWidget(QWidget *parent) : QTabWidget(parent)
{
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    tabBar()->setMovable(true);  // 允许拖动标签换位置
    setTabsClosable(true);//显示关闭按钮
    connect(this, &QTabWidget::customContextMenuRequested, this, &DroppableTabWidget::showTabContextMenu);
    connect(this,&QTabWidget::tabCloseRequested,this,&DroppableTabWidget::deleteTab);
}

void DroppableTabWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void DroppableTabWidget::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasText() || !currentWidget() ||
            !(qobject_cast<frmLivePreview *>(currentWidget()) || qobject_cast<frmHistoricalTrends *>(currentWidget()))) {
        event->ignore();
        return;
    }

    QStringList tabTexts = event->mimeData()->text().split("\n", Qt::SkipEmptyParts);

    //判断当前的标签是趋势图还是包络图
    QStringList namelist = tabText(currentIndex()).split(")");
    if(namelist.size() < 2) return;
    QString type = namelist.at(1);

    if(type == "波形图" || type == "包络图"){//波形图只支持单个打开，所以需要循环打开
        bool isdemodulated = ((type == "包络图") ? true : false);
        for(const QString &tabtext : tabTexts){
            QString text = tabtext + type;
            if(!isTabExist(text)){
                QStringList list;
                list.append(tabtext);
                frmLivePreview *newTab = new frmLivePreview(list,isdemodulated);
                addTab(newTab, text);
                setCurrentWidget(newTab);
            }
        }
    }else if(type == "趋势图" || type == "包络趋势图"){
        bool isdemodulated = ((type == "包络趋势图") ? true : false);
        QString text = tabTexts.join(",");
        if(tabTexts.size() > 1){
            text += "对比趋势图";
        }else{
            text += "趋势图";
        }
        if(!isTabExist(text)){
            frmHistoricalTrends *history = new frmHistoricalTrends(tabTexts,isdemodulated);
            history->SetWidgettitle(text);
            addTab(history, text);
            setCurrentWidget(history);
        }
    }
    event->acceptProposedAction();
}

bool DroppableTabWidget::isTabExist(QString &tabName)
{
    for (int i = 0; i < count(); ++i) {
        if (tabText(i) == tabName) {
            setCurrentIndex(i);  // 切换到已有的标签
            return true;
        }
    }
    return false;
}

void DroppableTabWidget::showTabContextMenu(const QPoint &pos)
{
    int tabIndex = tabBar()->tabAt(pos);
    if (tabIndex == -1) return;  // 没有点击到标签时直接返回

    QMenu menu(this);

    QAction *closeTab = menu.addAction("关闭该标签");
    QAction *closeAllTabs = menu.addAction("关闭所有标签");
    QAction *closeOtherTabs = menu.addAction("关闭其他标签");

    QAction *closeLeftTabs = nullptr;
    QAction *closeRightTabs = nullptr;

    // 只有在中间标签才显示关闭左侧和右侧标签的选项
    if (tabIndex > 0) {
        closeLeftTabs = menu.addAction("关闭左侧标签");
    }
    if (tabIndex < count() - 1) {
        closeRightTabs = menu.addAction("关闭右侧标签");
    }

    QAction *selectedAction = menu.exec(tabBar()->mapToGlobal(pos));
    if (!selectedAction) return;

    if (selectedAction == closeTab) {
        removeTab(tabIndex);
    } else if (selectedAction == closeAllTabs) {
        clear();
    } else if (selectedAction == closeRightTabs) {
        for (int i = count() - 1; i > tabIndex; --i) {
            removeTab(i);
        }
    } else if (selectedAction == closeLeftTabs) {
        for (int i = tabIndex - 1; i >= 0; --i) {
            removeTab(i);
        }
    } else if (selectedAction == closeOtherTabs) {
        for (int i = count() - 1; i > tabIndex; --i) {
            removeTab(i);
        }
        for (int i = tabIndex - 1; i >= 0; --i) {
            removeTab(i);
        }
    }
}

void DroppableTabWidget::deleteTab(int tabindex)
{
    removeTab(tabindex);
}
