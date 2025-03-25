#include "columndelegate.h"
#include <QDateTimeEdit>
#include <QListView>
#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>
ColumnDelegate::ColumnDelegate(ColumnDelegate::DelegateType type, const QVariant &extraData, QObject *parent) :
    QStyledItemDelegate(parent),
    delegateType(type),
    delegateData(extraData)
{

}

QWidget *ColumnDelegate ::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    switch(delegateType){
    case Delegate_Text:
    {
        QLineEdit *editor = new QLineEdit(parent);
        if (delegateData.canConvert<bool>() && delegateData.toBool()) {
            editor->setEchoMode(QLineEdit::Password);
        }
        return editor;
    }
    case Delegate_ComboBox:
    {
        QComboBox *editor = new QComboBox(parent);
        if (delegateData.canConvert<QStringList>()) {
            editor->addItems(delegateData.toStringList());
        }

        // 获取 QComboBox 的列表视图
        QListView *listView = qobject_cast<QListView *>(editor->view());
        if (listView) {
            // 给 QListView 添加事件过滤器
            listView->installEventFilter(const_cast<ColumnDelegate *>(this));
        }
        return editor;
    }
    case Delegate_DateTime:
    {
        QDateTimeEdit  *editor = new QDateTimeEdit(parent);
        editor->setCalendarPopup(true);
        if (delegateData.canConvert<QString>() && !delegateData.toString().isEmpty()) {
            editor->setDisplayFormat(delegateData.toString());
        } else {
            editor->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
        }
        return editor;
    }
    default:
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void ColumnDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch(delegateType){
    case Delegate_Text:
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
        if (lineEdit)
            lineEdit->setText(index.model()->data(index, Qt::EditRole).toString());
    }
        break;
    case Delegate_ComboBox:
    {
        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
        if (comboBox)
            comboBox->setCurrentText(index.model()->data(index, Qt::EditRole).toString());
    }
        break;
    case Delegate_DateTime:
    {
        QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
        if (dateTimeEdit){
            if(!delegateData.toString().isEmpty()){
                dateTimeEdit->setDisplayFormat(delegateData.toString());
            }else{
                dateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
            }

            dateTimeEdit->setDateTime(index.model()->data(index, Qt::EditRole).toDateTime());
        }
    }
        break;
    default:
        QStyledItemDelegate::setEditorData(editor, index);
        break;
    }
}

void ColumnDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    switch(delegateType){
    case Delegate_Text:
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
        if (lineEdit)
            model->setData(index, lineEdit->text(), Qt::EditRole);
    }
        break;
    case Delegate_ComboBox:
    {
        QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
        if (comboBox)
            model->setData(index, comboBox->currentText(), Qt::EditRole);
    }
        break;
    case Delegate_DateTime:
    {
        QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
        if (dateTimeEdit){
            if(!delegateData.toString().isEmpty()){
                model->setData(index, dateTimeEdit->dateTime().toString(delegateData.toString()), Qt::EditRole);
            }else{
                model->setData(index, dateTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm:ss"), Qt::EditRole);
            }
        }
    }
        break;
    default:
        QStyledItemDelegate::setModelData(editor,model,index);
        break;
    }
}

void ColumnDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

bool ColumnDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QListView *view = qobject_cast<QListView *>(object);
        if (view) {
            QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
            QModelIndex index = view->indexAt(helpEvent->pos());
            if (index.isValid()) {
                // 获取完整的文本内容
                QString fullText = index.data(Qt::DisplayRole).toString();
                // 在鼠标位置显示 QToolTip
                QToolTip::showText(helpEvent->globalPos(), fullText, view);
            }
        }
        return true; // 表示事件已处理
    }
    return QStyledItemDelegate::eventFilter(object, event);
}
