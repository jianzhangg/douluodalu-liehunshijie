#ifndef WINDOW_TARGET_H
#define WINDOW_TARGET_H

#include <QtGlobal>
#include <QDebug>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
HWND getTargetWindow();
void setTargetWindow(HWND h);
QString describeWindow(HWND h);
#else
inline void* getTargetWindow() { return nullptr; }
inline void setTargetWindow(void*) {}
inline QString describeWindow(void*) { return QStringLiteral("Unsupported platform"); }
#endif

#endif // WINDOW_TARGET_H


