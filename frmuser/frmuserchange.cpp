#include "frmuserchange.h"
#include "ui_frmuserchange.h"
#include <QMessageBox>
#include "m_dbapi.h"
frmUserChange::frmUserChange(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmUserChange)
{
    ui->setupUi(this);
    setWindowTitle("用户切换");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->txt_password->setEchoMode(QLineEdit::Password);
    ui->lab_Currentuser->setText(QString("当前用户:%1").arg(App::CurrentUser));
    connect(ui->btn_cancel,&QPushButton::clicked,this,&QDialog::reject);
    connect(ui->btn_ok,&QPushButton::clicked,this,&frmUserChange::verify);
    this->resize(250,150);
}

frmUserChange::~frmUserChange()
{
    delete ui;
}

void frmUserChange::verify()
{
    QString user = ui->txt_user->text();
    QString password = ui->txt_password->text();
    if(user.isEmpty()){
        QMessageBox::warning(this,"切换用户","用户名不能为空!");
        return;
    }

    if(password.isEmpty()){
        QMessageBox::warning(this,"切换用户","密码不能为空!");
        return;
    }

    //根据数据库去判断，如果用户名不存在或密码错误则弹窗提示
    QStringList namelist;
    QStringList passwordlist;
    QSqlQuery query(M_DbAPI::Instance()->GetLocalDBHandle());
    QString sqlstr = "SELECT * FROM UserInfo";
    if(query.exec(sqlstr)){
        while (query.next()) {
            namelist.append(query.value(0).toString());
            passwordlist.append(query.value(1).toString());
        }
    }
    if(!namelist.contains(user)){
        QMessageBox::warning(this,"切换用户","用户名不存在");
        return;
    }

    int index = namelist.indexOf(user);
    if(password != passwordlist.at(index)){
        QMessageBox::warning(this,"切换用户","密码错误");
        return;
    }else{
        QMessageBox::information(this,"切换用户","用户切换成功");
        App::CurrentUser = user;
        App::CurrentPassword = password;
        App::writeConfig();
        this->accept();
    }
}
