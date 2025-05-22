#include "frmabout.h"
#include <QPixmap>
#include <QMutex>
frmabout *frmabout::self = 0;
frmabout *frmabout::Instance()
{
    if (!self) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new frmabout;
        }
    }
    return self;
}

frmabout::frmabout(QWidget *parent) : QDialog(parent) {
    setWindowTitle("关于");
    setFixedSize(380, 280);  // 设置固定窗口大小

    // Logo
    QLabel *logoLabel = new QLabel(this);
    logoLabel->setPixmap(QPixmap(":/image/image/Icon/Qnode.png")); // 请确保提供正确的路径
    logoLabel->setAlignment(Qt::AlignCenter);

    // Text content
    QLabel *textLabel = new QLabel(this);
    textLabel->setText(
                "<h2>走行部管理软件</h2>"
                "<p>版本信息: v1.0.0</p>"
                "<p>版权所有: 上海诠诺物联网技术有限公司</p>"
                "<p>联系我们: 021-1234567</p>"
                "<p>开发者: Mr.Liu</p>"
                "<p>Email: liumengguangboy@163.com</p>"
                );
    textLabel->setWordWrap(true);  // 使文本自动换行

    // Close button
    QPushButton *closeButton = new QPushButton("Close",this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    // Layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(logoLabel);
    mainLayout->addWidget(textLabel);
    mainLayout->addWidget(closeButton, 0, Qt::AlignRight);

    setLayout(mainLayout);
}
