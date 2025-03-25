#ifndef M_DELEGATE_H
#define M_DELEGATE_H
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QComboBox>
#include <QPainter>
#include <QPushButton>
#include <QDateTime>
#include <QAbstractItemView>
class ColumnDelegate  : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum DelegateType {
        Delegate_Text,       // 文本编辑器（QLineEdit）
        Delegate_ComboBox,   // 下拉框（QComboBox）
        Delegate_DateTime    // 日期时间编辑器（QDateTimeEdit）
    };
     // extraData 对于 Text: bool（是否密码模式）; ComboBox: QStringList（下拉选项）; DateTime: QString（显示格式）
    explicit ColumnDelegate (DelegateType type, const QVariant &extraData = QVariant(), QObject *parent = nullptr);


protected:
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
//    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override; //设置编辑器的几何位置

    bool eventFilter(QObject *object, QEvent *event) override;
private:
    DelegateType delegateType;      //委托类型
    QVariant delegateData;          //委托参数

public:
#if 0
    //设置委托类型
    void SetDelegateType(const QString &type);

    //设置下拉框是否可以编辑
    void SetDelegateEdit(bool Enable);

    //设置是否以密码显示
    void SetDelegatePwd(bool Enable);

    //设置委托参数
    void SetDelegateValue(QStringList Values);
#endif
};

#endif // M_DELEGATE_H
