#ifndef OCR_SIMPLE_H
#define OCR_SIMPLE_H

#include <opencv2/core.hpp>
#include <QString>
#include <QDebug>

// 使用模板匹配寻找“魂兽幻境”四个字在大图中的位置。
// 参数：
//  - screenBgr: 屏幕截图（BGR）
//  - templBgr: 模板小图（BGR），需与截图文字样式接近
// 返回：匹配到的矩形区域，若未找到，返回空矩形
cv::Rect findHunShouHuanJing(const cv::Mat& screenBgr, const cv::Mat& templBgr, double* outScore = nullptr);

#endif // OCR_SIMPLE_H


