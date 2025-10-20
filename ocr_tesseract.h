#ifndef OCR_TESSERACT_H
#define OCR_TESSERACT_H

#ifdef ENABLE_TESSERACT
#include <opencv2/core.hpp>
#include <QString>

// 使用 Tesseract 在图像中查找包含 query 文本的矩形（像素坐标，基于输入图像左上角）。
// lang: 例如 "chi_sim"。返回是否找到；若找到，填充 outRect（x,y,w,h）与 outConf（0..100）。
bool tesseractFindTextBBox(const cv::Mat& bgrImage, const char* lang, const QString& query,
                           QRect* outRect, double* outConf, QString* debugText);
#endif

#endif // OCR_TESSERACT_H


