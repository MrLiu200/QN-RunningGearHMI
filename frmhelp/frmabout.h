#ifndef FRMABOUT_H
#define FRMABOUT_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

class frmabout : public QDialog {
    Q_OBJECT

public:
    static frmabout *Instance();
    explicit frmabout(QWidget *parent = nullptr);
private:
    static frmabout *self;
};

#endif // FRMABOUT_H
