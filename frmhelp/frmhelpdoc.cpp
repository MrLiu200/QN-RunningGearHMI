#include "frmhelpdoc.h"
#include <QTextEdit>
#include <QFile>
#include <QVBoxLayout>

frmhelpdoc::frmhelpdoc(QWidget *parent) : QWidget(parent)
{
    QTextEdit *textedit = new QTextEdit(this);
    textedit->setReadOnly(true);
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addWidget(textedit);

    this->setLayout(mainlayout);

    QFile file(":/other/image/other/help.txt");//---LMG
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString str = file.readAll();
        textedit->setText(str);
        file.close();
    }

}
