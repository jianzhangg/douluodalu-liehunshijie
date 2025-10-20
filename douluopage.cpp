#include "douluopage.h"
#include "ui_douluopage.h"

DouluoPage::DouluoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DouluoPage)
{
    ui->setupUi(this);

    // 连接 UI 上的按钮
    connect(findChild<QPushButton*>("btnVirtualRealm"), &QPushButton::clicked, this, &DouluoPage::onVirtualRealmClicked);
}

DouluoPage::~DouluoPage()
{
    delete ui;
}

void DouluoPage::onVirtualRealmClicked()
{
    // 在另一个 cpp 文件中实现日志功能，这里只发出信号或直接调用函数
    extern void logVirtualRealm();
    logVirtualRealm();
}



