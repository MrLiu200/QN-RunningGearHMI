#ifndef FRMUSERADD_H
#define FRMUSERADD_H

#include <QDialog>

namespace Ui {
class frmUserAdd;
}

class frmUserAdd : public QDialog
{
    Q_OBJECT

public:
    explicit frmUserAdd(QWidget *parent = nullptr);
    ~frmUserAdd();

private:
    Ui::frmUserAdd *ui;

private slots:
    void AddUsers();
};

#endif // FRMUSERADD_H
