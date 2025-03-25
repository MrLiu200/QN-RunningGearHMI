#include "customlistview.h"
#include <QStandardItemModel>
#include <QDataStream>
#include <QMessageBox>
#include <QDebug>
#include <QDrag>
CustomListView::CustomListView(QWidget *parent)
    : QListView(parent)
{
    setUniformItemSizes(true);//设置项相同大小
    setDragEnabled(true); // 启用拖动
    setAcceptDrops(false); // 禁用接收拖动
    setDropIndicatorShown(true); // 不显示拖放指示器
    setSelectionMode(QAbstractItemView::ExtendedSelection);// 设置为多选模式 允许连续选择和不连续选择
    setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑
    setContextMenuPolicy(Qt::CustomContextMenu); // 设置上下文菜单策略

    setStyleSheet("QListView {  border: none; background-color: transparent; selection-background-color: lightgray; }");
    connect(this, &QListView::customContextMenuRequested, this, &CustomListView::showContextMenu);
    connect(this, &QListView::doubleClicked, this, &CustomListView::onItemDoubleClicked); // 连接双击信号
}

QMimeData* CustomListView::mimeData(const QList<QModelIndex>& indexes) const {
    QMimeData *mimeData = new QMimeData;
    QStringList itemTexts;

    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            itemTexts << index.data().toString();// 获取每项的文本
            qDebug()<<index.data().toString();
        }
    }

    mimeData->setData("application/x-item", itemTexts.join(";").toUtf8());// 设置数据
    return mimeData;
}

void CustomListView::dragEnterEvent(QDragEnterEvent *event) {
    // 不接收外部拖放，直接忽略
    event->ignore();
}

void CustomListView::dropEvent(QDropEvent *event) {
    // 不处理拖放事件
    event->ignore();
}

void CustomListView::startDrag(Qt::DropActions supportedActions)
{
    QModelIndexList indexes = selectionModel()->selectedIndexes();
           if (indexes.isEmpty())
               return; // 没有选中的项就不拖动

           // 收集所有选中的项
           QStringList selectedItems;
           for (const QModelIndex &index : indexes) {
               selectedItems.append(index.data(Qt::DisplayRole).toString());
           }

           // 设置 MIME 数据（文本格式，以换行符 `\n` 分隔）
           QMimeData *mimeData = new QMimeData();
           mimeData->setText(selectedItems.join("\n"));

           // 创建拖动对象
           QDrag *drag = new QDrag(this);
           drag->setMimeData(mimeData);
           drag->exec(supportedActions, Qt::MoveAction);
    #if 0
    QModelIndex index = currentIndex();
    if (!index.isValid()) return;

    QMimeData *mimeData = new QMimeData();
    mimeData->setText(model()->data(index, Qt::DisplayRole).toString());  // 拖动数据

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction);
#endif
}

void CustomListView::showContextMenu(const QPoint &pos)
{
    // 获取当前点击的索引
       QModelIndex index = indexAt(pos);
       if (!index.isValid()) {
           return; // 如果是点击空白区域，则不显示菜单
       }

       // 获取当前项
       QString itemText = index.data().toString();

       // 创建右键菜单
       QMenu menu(this);

       // 添加菜单项并连接到槽
       QAction *action1 = new QAction(QIcon(":/image/image/Icon/1.png"),"趋势图", this);
       QAction *action2 = new QAction(QIcon(":/image/image/Icon/2.png"),"波形图", this);
       QAction *action5 = new QAction(QIcon(":/image/image/Icon/demodulated.png"),"包络趋势图", this);
       QAction *action7 = new QAction(QIcon(":/image/image/Icon/livedemodulated.png"),"时域包络图",this);
       QAction *action3 = new QAction("全部选中",this);
       QAction *action4 = new QAction("全部取消",this);
       QAction *action6 = new QAction("清除报警",this);

       connect(action1, &QAction::triggered,[this,itemText]() {
//           qDebug() << "趋势图触发，项：" << itemText;
//           QMessageBox::information(nullptr, "趋势图", "趋势图被触发, 项: " + itemText);
           QStringList list;
           list.append(itemText);
           Q_EMIT OpenTrend(list.join(","),false);
       });

       connect(action2, &QAction::triggered,[this,itemText]() {
//           qDebug() << "波形图触发，项：" << itemText;
//           QMessageBox::information(nullptr, "波形图", "波形图被触发, 项: " + itemText);
           QStringList list;
           list.append(itemText);
           Q_EMIT OpenWaveform(list.join(","),false);
       });

       connect(action3,&QAction::triggered,this,&CustomListView::selectAllItems);
       connect(action4,&QAction::triggered,this,&CustomListView::deselectAllItems);

       connect(action5, &QAction::triggered,[this,itemText]() {
           QStringList list;
           list.append(itemText);
           Q_EMIT OpenTrend(list.join(","),true);
       });

       connect(action6, &QAction::triggered,[this,itemText]() {
           Q_EMIT clearAlarm(itemText);
       });

       connect(action7, &QAction::triggered,[this,itemText]() {
           QStringList list;
           list.append(itemText);
           Q_EMIT OpenWaveform(list.join(","),true);
       });
       menu.addAction(action1);
       menu.addAction(action2);
       menu.addAction(action5);
       menu.addAction(action7);
       menu.addAction(action3);
       menu.addAction(action4);
       menu.addAction(action6);
       // 显示菜单
       menu.exec(mapToGlobal(pos)); // 使用 mapToGlobal 将位置转化为全局坐标
}

// 双击项时触发
void CustomListView::onItemDoubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString itemText = index.data(Qt::DisplayRole).toString();
        QString text;
        QRegularExpression regex(R"(^([a-zA-Z0-9_]+)\()");  // 匹配括号前的字母、数字或下划线
        QRegularExpressionMatch match = regex.match(itemText);

           if (match.hasMatch()) {
               text =  match.captured(1);  // 返回第一个捕获组（括号前的部分）
           }else{
               text = itemText;
           }
        qDebug() << "双击的项文本: " << itemText;
        QMessageBox::information(this, "双击项", "双击的项是: " + itemText);
    }
}

void CustomListView::selectAllItems()
{
    // 获取模型
        QAbstractItemModel *model = this->model();

        // 获取总行数
        int rowCount = model->rowCount();

        // 获取选择模型
        QItemSelectionModel *selectionModel = this->selectionModel();

        // 为每一行创建选择范围
        for (int row = 0; row < rowCount; ++row) {
            QModelIndex index = model->index(row, 0); // 假设是单列列表，列索引为0
            selectionModel->select(index, QItemSelectionModel::Select); // 选择该项
        }
}

void CustomListView::deselectAllItems()
{
    selectionModel()->clearSelection();
}

// 获取被选中的项
QStringList CustomListView::onGetSelectedItems()
{
    QList<QModelIndex> selectedIndexes = selectionModel()->selectedIndexes();
    QStringList selectedItems;

    // 遍历选中的项
    for (const QModelIndex &index : selectedIndexes) {
        if (index.isValid()) {
            QString itemText = index.data(Qt::DisplayRole).toString();
            selectedItems << itemText;
        }
    }


    // 显示被选中的项
//    qDebug() << "选中的项: " << selectedItems;
//    QMessageBox::information(this, "选中的项", "选中的项: " + selectedItems.join(", "));
    return selectedItems;
}
