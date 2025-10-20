#include "window_target.h"

#ifdef _WIN32
static HWND g_target = nullptr;

HWND getTargetWindow()
{
    return g_target;
}

void setTargetWindow(HWND h)
{
    g_target = h;
}

QString describeWindow(HWND h)
{
    if (!h) return QStringLiteral("<null>");
    wchar_t title[512] = {0}; GetWindowTextW(h, title, 511);
    wchar_t cls[256] = {0};  GetClassNameW(h, cls, 255);
    RECT rc{}; GetWindowRect(h, &rc);
    return QStringLiteral("HWND=%1, title=%2, class=%3, rect=[%4,%5,%6,%7]")
        .arg((qulonglong)h)
        .arg(QString::fromWCharArray(title))
        .arg(QString::fromWCharArray(cls))
        .arg(rc.left).arg(rc.top).arg(rc.right).arg(rc.bottom);
}
#endif


