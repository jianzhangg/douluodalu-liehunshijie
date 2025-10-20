#include "douluopage.h"
#include "ui_douluopage.h"
#include "window_target.h"
#include "capture_window.h"
#include "ocr_simple.h"
#include "ocr_tesseract.h"
#include <QMessageBox>
#include <QLabel>
#include <QCoreApplication>
#include <opencv2/imgcodecs.hpp>

DouluoPage::DouluoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DouluoPage)
{
    ui->setupUi(this);

    // 连接 UI 上的按钮
    connect(findChild<QPushButton*>("btnVirtualRealm"), &QPushButton::clicked, this, &DouluoPage::onVirtualRealmClicked);
    auto *btnOk = findChild<QPushButton*>("btnConfirm");
    if (btnOk) {
        connect(btnOk, &QPushButton::clicked, this, [this]{
            auto *lbl = findChild<QLabel*>("lblInfo");
            HWND hwnd = getTargetWindow();
            if (!hwnd) { if (lbl) lbl->setText(QStringLiteral("未选择窗口，无法识别。")); qDebug() << "No target window selected"; return; }
            qDebug() << "Start capture target window";
            if (lbl) lbl->setText(QStringLiteral("开始截图..."));

#ifdef ENABLE_OPENCV
            cv::Mat screen = captureClientToMat(hwnd);
            if (screen.empty()) { if (lbl) lbl->setText(QStringLiteral("截图失败")); qDebug() << "Capture failed"; return; }
            qDebug() << "Capture done";
            if (lbl) lbl->setText(QStringLiteral("截图完成，开始识别‘魂兽幻境’..."));

#ifdef ENABLE_TESSERACT
            QRect bbox; double conf = 0.0; QString dbg;
            bool ok = tesseractFindTextBBox(screen, "chi_sim", QStringLiteral("魂兽幻境"), &bbox, &conf, &dbg);
            if (!ok) {
                if (lbl) lbl->setText(QStringLiteral("OCR 未识别到‘魂兽幻境’"));
                qDebug() << "Tesseract not found text. Debug:" << dbg;
                return;
            }
            qDebug() << "OCR found bbox at (" << bbox.x() << "," << bbox.y() << ") size (" << bbox.width() << "," << bbox.height() << ") conf:" << conf;
            if (lbl) lbl->setText(QStringLiteral("识别成功，坐标：(%1,%2)，大小：%3x%4，置信度：%5")
                                  .arg(bbox.x()).arg(bbox.y()).arg(bbox.width()).arg(bbox.height()).arg(conf, 0, 'f', 1));
#else
            // 退化为模板匹配（需要 hunshouhuanjing.png）
            QString templPath = QCoreApplication::applicationDirPath() + "/assets/hunshouhuanjing.png";
            cv::Mat templ = cv::imread(templPath.toStdString());
            if (templ.empty()) { if (lbl) lbl->setText(QStringLiteral("未启用 OCR 且缺少模板图")); qDebug() << "No OCR and no template"; return; }
            double score = 0.0; cv::Rect roi = findHunShouHuanJing(screen, templ, &score);
            if (roi.empty()) { if (lbl) lbl->setText(QStringLiteral("模板匹配未识别到")); qDebug() << "Template match not found"; return; }
            qDebug() << "Template match score:" << score << " at (" << roi.x << "," << roi.y << ") size (" << roi.width << "," << roi.height << ")";
            if (lbl) lbl->setText(QStringLiteral("识别成功，坐标：(%1,%2)，大小：%3x%4").arg(roi.x).arg(roi.y).arg(roi.width).arg(roi.height));
#endif
#else
            if (lbl) lbl->setText(QStringLiteral("未启用 OpenCV，无法识别。"));
            qDebug() << "OpenCV disabled";
#endif
        });
    }
}

DouluoPage::~DouluoPage()
{
    delete ui;
}

void DouluoPage::onVirtualRealmClicked()
{
    // 进入“选择窗口”模式：显示提示，等待用户点击任意窗口（使用 GetAsyncKeyState 轮询，无需全局钩子）
#ifdef _WIN32
    auto *lbl = findChild<QLabel*>("lblInfo");
    if (lbl) lbl->setText(QStringLiteral("请将鼠标移动到目标窗口上并单击左键以选择..."));

    const DWORD start = GetTickCount();
    SHORT prevState = GetAsyncKeyState(VK_LBUTTON);
    bool pressedDetected = false;
    HWND picked = nullptr;

    while (GetTickCount() - start < 15000) {
        SHORT state = GetAsyncKeyState(VK_LBUTTON);
        bool isDown = (state & 0x8000) != 0;
        bool wasDown = (prevState & 0x8000) != 0;

        if (!pressedDetected && isDown && !wasDown) {
            // 捕获按下瞬间的窗口
            POINT pt; GetCursorPos(&pt);
            HWND h = WindowFromPoint(pt);
            if (h) {
                HWND top = GetAncestor(h, GA_ROOT);
                picked = top ? top : h;
                pressedDetected = true;
            }
        }
        if (pressedDetected && !isDown && wasDown) {
            // 抬起后结束
            break;
        }

        prevState = state;
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(10);
    }

    if (picked) {
        setTargetWindow(picked);
        if (lbl) lbl->setText(describeWindow(picked));
        if (auto *btnOk = findChild<QPushButton*>("btnConfirm")) btnOk->setEnabled(true);
    } else {
        if (lbl) lbl->setText(QStringLiteral("未在限定时间内选择窗口。"));
    }
#else
    QMessageBox::warning(this, QStringLiteral("不支持"), QStringLiteral("当前平台不支持窗口选择。"));
#endif
}



