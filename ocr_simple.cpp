#include "ocr_simple.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

cv::Rect findHunShouHuanJing(const cv::Mat& screenBgr, const cv::Mat& templBgr, double* outScore)
{
    if (outScore) *outScore = 0.0;
    if (screenBgr.empty() || templBgr.empty()) return {};
    cv::Mat screenGray, templGray;
    cv::cvtColor(screenBgr, screenGray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(templBgr, templGray, cv::COLOR_BGR2GRAY);

    int resultCols = screenGray.cols - templGray.cols + 1;
    int resultRows = screenGray.rows - templGray.rows + 1;
    if (resultCols <= 0 || resultRows <= 0) return {};
    cv::Mat result(resultRows, resultCols, CV_32FC1);
    cv::matchTemplate(screenGray, templGray, result, cv::TM_CCOEFF_NORMED);
    double minVal, maxVal; cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    if (outScore) *outScore = maxVal;
    if (maxVal < 0.7) { // 阈值可调
        return {};
    }
    return cv::Rect(maxLoc.x, maxLoc.y, templGray.cols, templGray.rows);
}


