#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QByteArray>
#include <QProcessEnvironment>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <algorithm>
#include <cwctype>
#endif

#include "douluopage.h"

#ifdef _WIN32
namespace {
    static HWND findLdWindow()
    {
        QByteArray hint = qgetenv("LDPLAYER_WINDOW_HINT");
        HWND targetWnd = nullptr;
        auto payload = std::make_pair(&targetWnd, std::wstring(hint.isEmpty() ? L"" : std::wstring(hint.begin(), hint.end())));
        EnumWindows(
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                if (!IsWindowVisible(hWnd)) return TRUE;
                wchar_t title[512] = {0}; GetWindowTextW(hWnd, title, 511);
                wchar_t cls[256] = {0};  GetClassNameW(hWnd, cls, 255);
                std::wstring t(title), c(cls);
                auto *p = reinterpret_cast<std::pair<HWND*, std::wstring>*>(lParam);
                const std::wstring &hintW = p->second;
                auto contains_ci = [](const std::wstring& hay, const std::wstring& needle) {
                    if (needle.empty()) return false;
                    std::wstring H = hay, N = needle;
                    std::transform(H.begin(), H.end(), H.begin(), ::towlower);
                    std::transform(N.begin(), N.end(), N.begin(), ::towlower);
                    return H.find(N) != std::wstring::npos;
                };
                bool match = false;
                if (!hintW.empty()) {
                    match = contains_ci(t, hintW) || contains_ci(c, hintW);
                } else {
                    match = (t.find(L"雷电") != std::wstring::npos) || (t.find(L"LDPlayer") != std::wstring::npos)
                         || (c.find(L"LDPlayer") != std::wstring::npos) || (c.find(L"leidian") != std::wstring::npos);
                }
                if (match) { *p->first = hWnd; return FALSE; }
                return TRUE;
            }, reinterpret_cast<LPARAM>(&payload));
        return targetWnd;
    }

    static std::pair<LONG,LONG> toAbsolute(int x, int y)
    {
        int sw = GetSystemMetrics(SM_CXSCREEN);
        int sh = GetSystemMetrics(SM_CYSCREEN);
        LONG ax = static_cast<LONG>((x * 65535.0) / (sw - 1));
        LONG ay = static_cast<LONG>((y * 65535.0) / (sh - 1));
        return {ax, ay};
    }

    static void moveMouseAbs(int x, int y)
    {
        auto absPos = toAbsolute(x, y);
        INPUT in; ZeroMemory(&in, sizeof(in));
        in.type = INPUT_MOUSE;
        in.mi.dx = absPos.first;
        in.mi.dy = absPos.second;
        in.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
        SendInput(1, &in, sizeof(INPUT));
    }

    static void mouseDown()
    {
        INPUT in; ZeroMemory(&in, sizeof(in));
        in.type = INPUT_MOUSE;
        in.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &in, sizeof(INPUT));
    }

    static void mouseUp()
    {
        INPUT in; ZeroMemory(&in, sizeof(in));
        in.type = INPUT_MOUSE;
        in.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &in, sizeof(INPUT));
    }
}
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btnUp, &QPushButton::clicked, this, &MainWindow::onUpClicked);
    connect(ui->btnDown, &QPushButton::clicked, this, &MainWindow::onDownClicked);
    connect(ui->btnLeft, &QPushButton::clicked, this, &MainWindow::onLeftClicked);
    connect(ui->btnRight, &QPushButton::clicked, this, &MainWindow::onRightClicked);
    connect(ui->btnDouluo, &QPushButton::clicked, this, &MainWindow::onDouluoClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onUpClicked()
{
    qDebug() << "Up button clicked";
#ifdef _WIN32
    HWND targetWnd = findLdWindow();

    if (!targetWnd) {
        qDebug() << "LDPlayer window not found. Set env LDPLAYER_WINDOW_HINT to a unique keyword (title/class).";
        return;
    }

    // 置前激活
    SetForegroundWindow(targetWnd);
    Sleep(200);

    // 在目标窗口客户区左下角滑动
    RECT rcClient{}; GetClientRect(targetWnd, &rcClient);
    POINT lt{ rcClient.left, rcClient.top };
    POINT rb{ rcClient.right, rcClient.bottom };
    ClientToScreen(targetWnd, &lt);
    ClientToScreen(targetWnd, &rb);

    const int margin = 150;
    const int startX = lt.x + margin;     // 统一起点：左下角 + margin
    const int startY = rb.y - margin;     // 统一起点：左下角 + margin
    const int endX = startX;
    const int endY = startY - ((rb.y - lt.y) / 8); // 上滑 1/8 客户区高度（更短）

    auto toAbsolute = [](int x, int y) -> std::pair<LONG, LONG> {
        int sw = GetSystemMetrics(SM_CXSCREEN);
        int sh = GetSystemMetrics(SM_CYSCREEN);
        LONG ax = static_cast<LONG>((x * 65535.0) / (sw - 1));
        LONG ay = static_cast<LONG>((y * 65535.0) / (sh - 1));
        return {ax, ay};
    };

    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    input.type = INPUT_MOUSE;

    // 移动到起点并按下
    {
        moveMouseAbs(startX, startY);
        mouseDown();
    }

    // 1s（保持较高步数以更平滑）
    const int steps = 200;
    const int totalMs = 1000;
    const int stepDelay = totalMs / steps;
    for (int i = 1; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        int curX = startX + static_cast<int>((endX - startX) * t);
        int curY = startY + static_cast<int>((endY - startY) * t);
        moveMouseAbs(curX, curY);
        Sleep(stepDelay);
    }

    // 抬起
    mouseUp();

    qDebug() << "Completed upward swipe in LDPlayer (1s)";
#else
    qDebug() << "Current platform does not support swipe operation";
#endif
}

void MainWindow::onDownClicked()
{
    qDebug() << "Down button clicked";
#ifdef _WIN32
    HWND targetWnd = findLdWindow();
    if (!targetWnd) { qDebug() << "LDPlayer window not found."; return; }
    SetForegroundWindow(targetWnd); Sleep(200);

    RECT rc{}; GetClientRect(targetWnd, &rc);
    POINT lt{ rc.left, rc.top }, rb{ rc.right, rc.bottom };
    ClientToScreen(targetWnd, &lt); ClientToScreen(targetWnd, &rb);

    const int margin = 150;
    const int startX = lt.x + margin;              // 统一：左下角 + margin
    const int startY = rb.y - margin;
    const int endX   = startX;
    const int endY   = startY + ((rb.y - lt.y) / 8); // 向下 1/8 客户区高度（更短）

    moveMouseAbs(startX, startY);
    mouseDown();
    const int steps = 60, totalMs = 1000, stepDelay = totalMs / steps;
    for (int i = 1; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        int curX = startX + static_cast<int>((endX - startX) * t);
        int curY = startY + static_cast<int>((endY - startY) * t);
        moveMouseAbs(curX, curY);
        Sleep(stepDelay);
    }
    mouseUp();
    qDebug() << "Completed downward swipe in LDPlayer (1s)";
#endif
}

void MainWindow::onLeftClicked()
{
    qDebug() << "Left button clicked";
#ifdef _WIN32
    HWND targetWnd = findLdWindow();
    if (!targetWnd) { qDebug() << "LDPlayer window not found."; return; }
    SetForegroundWindow(targetWnd); Sleep(200);

    RECT rc{}; GetClientRect(targetWnd, &rc);
    POINT lt{ rc.left, rc.top }, rb{ rc.right, rc.bottom };
    ClientToScreen(targetWnd, &lt); ClientToScreen(targetWnd, &rb);

    const int margin = 150;
    const int startX = lt.x + margin;               // 统一：左下角 + margin
    const int startY = rb.y - margin;
    const int endX   = startX - ((rb.x - lt.x) / 8); // 向左 1/8 客户区宽度（更短）
    const int endY   = startY;

    moveMouseAbs(startX, startY);
    mouseDown();
    const int steps = 60, totalMs = 1000, stepDelay = totalMs / steps;
    for (int i = 1; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        int curX = startX + static_cast<int>((endX - startX) * t);
        int curY = startY + static_cast<int>((endY - startY) * t);
        moveMouseAbs(curX, curY);
        Sleep(stepDelay);
    }
    mouseUp();
    qDebug() << "Completed leftward swipe in LDPlayer (1s)";
#endif
}

void MainWindow::onRightClicked()
{
    qDebug() << "Right button clicked";
#ifdef _WIN32
    HWND targetWnd = findLdWindow();
    if (!targetWnd) { qDebug() << "LDPlayer window not found."; return; }
    SetForegroundWindow(targetWnd); Sleep(200);

    RECT rc{}; GetClientRect(targetWnd, &rc);
    POINT lt{ rc.left, rc.top }, rb{ rc.right, rc.bottom };
    ClientToScreen(targetWnd, &lt); ClientToScreen(targetWnd, &rb);

    const int margin = 150;
    const int startX = lt.x + margin;               // 统一：左下角 + margin
    const int startY = rb.y - margin;
    const int endX   = startX + ((rb.x - lt.x) / 8); // 向右 1/8 客户区宽度（更短）
    const int endY   = startY;

    moveMouseAbs(startX, startY);
    mouseDown();
    const int steps = 60, totalMs = 1000, stepDelay = totalMs / steps;
    for (int i = 1; i <= steps; ++i) {
        double t = static_cast<double>(i) / steps;
        int curX = startX + static_cast<int>((endX - startX) * t);
        int curY = startY + static_cast<int>((endY - startY) * t);
        moveMouseAbs(curX, curY);
        Sleep(stepDelay);
    }
    mouseUp();
    qDebug() << "Completed rightward swipe in LDPlayer (1s)";
#endif
}

void MainWindow::onDouluoClicked()
{
    qDebug() << "Open DouluoPage";
    auto *page = new DouluoPage(this);
    page->setAttribute(Qt::WA_DeleteOnClose);
    page->setWindowTitle(QStringLiteral("斗罗大陆-猎魂世界"));
    page->resize(600, 400);
    page->show();
}
