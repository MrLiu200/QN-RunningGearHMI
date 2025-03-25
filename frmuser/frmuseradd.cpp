#include "frmuseradd.h"
#include "ui_frmuseradd.h"
#include <QMessageBox>
#include "m_dbapi.h"
frmUserAdd::frmUserAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmUserAdd)
{
    ui->setupUi(this);
    setWindowTitle("添加用户");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->txt_password->setEchoMode(QLineEdit::Password);
    connect(ui->btn_cancel,&QPushButton::clicked,this,&QDialog::reject);
    connect(ui->btn_ok,&QPushButton::clicked,this,&frmUserAdd::AddUsers);
    this->resize(250,150);
}

frmUserAdd::~frmUserAdd()
{
    delete ui;
}

void frmUserAdd::AddUsers()
{
    QString user = ui->txt_user->text();
    QString password = ui->txt_password->text();
    if(user.isEmpty()){
        QMessageBox::warning(this,"错误","用户名不能为空!");
        return;
    }

    if(password.isEmpty()){
        QMessageBox::warning(this,"错误","密码不能为空!");
        return;
    }

    //往数据库里添加内容
    QSqlQuery query(M_DbAPI::Instance()->GetLocalDBHandle());
    QString addstr = "INSERT INTO UserInfo (User, Password) VALUES (?, ?)";
    query.prepare(addstr);

    query.bindValue(0, user);   // 绑定用户名
    query.bindValue(1, password);   // 绑定密码
    if(query.exec()){
        if(QMessageBox::information(this,"新增用户","添加新用户成功！") == QMessageBox::Ok){
            this->accept();
        }
    }else{
        QMessageBox::warning(this,"新增用户","添加失败！");
    }
}
