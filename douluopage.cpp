#include "douluopage.h"
#include "ui_douluopage.h"
#include <QPushButton>

DouluoPage::DouluoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DouluoPage)
{
    ui->setupUi(this);

    // 创建一个按钮：虚拟境地副本
    QPushButton *btn = new QPushButton(QStringLiteral("虚拟境地副本"), this);
    btn->setObjectName("btnVirtualRealm");
    btn->setMinimumSize(180, 48);
    QFont f = btn->font(); f.setPointSize(12); f.setBold(true); btn->setFont(f);
    btn->move(10, 10); // 左上角

    connect(btn, &QPushButton::clicked, this, &DouluoPage::onVirtualRealmClicked);
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



