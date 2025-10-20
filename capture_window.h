#ifndef CAPTURE_WINDOW_H
#define CAPTURE_WINDOW_H

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <QtGlobal>
#  include <QDebug>
#  include <opencv2/core.hpp>

// 将给定 HWND 的客户区截图为 BGR cv::Mat，失败返回空 Mat
cv::Mat captureClientToMat(HWND hwnd);

#endif

#endif // CAPTURE_WINDOW_H


