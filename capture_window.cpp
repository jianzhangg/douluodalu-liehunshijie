#include "capture_window.h"

#ifdef _WIN32
#include <opencv2/imgproc.hpp>

cv::Mat captureClientToMat(HWND hwnd)
{
    if (!hwnd) return cv::Mat();
    RECT rc{}; if (!GetClientRect(hwnd, &rc)) return cv::Mat();
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return cv::Mat();

    HDC hdc = GetDC(hwnd);
    HDC mem = CreateCompatibleDC(hdc);
    BITMAPINFO bmi{}; bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ old = SelectObject(mem, hbmp);

    POINT pt{0,0}; ClientToScreen(hwnd, &pt);
    BitBlt(mem, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

    cv::Mat mat(h, w, CV_8UC3, bits);
    cv::Mat bgr; mat.copyTo(bgr);

    SelectObject(mem, old);
    DeleteObject(hbmp);
    DeleteDC(mem);
    ReleaseDC(hwnd, hdc);
    return bgr;
}
#endif


