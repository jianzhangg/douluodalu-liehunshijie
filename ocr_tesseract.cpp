#include "ocr_tesseract.h"

#ifdef ENABLE_TESSERACT
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/imgproc.hpp>

static Pix* cvMatBgrToPix(const cv::Mat& bgr)
{
    cv::Mat rgb; cv::cvtColor(bgr, rgb, cv::COLOR_BGR2RGB);
    return pixCreate(rgb.cols, rgb.rows, 32);
}

bool tesseractFindTextBBox(const cv::Mat& bgrImage, const char* lang, const QString& query,
                           QRect* outRect, double* outConf, QString* debugText)
{
    if (outRect) *outRect = QRect();
    if (outConf) *outConf = 0.0;
    if (bgrImage.empty()) return false;

    tesseract::TessBaseAPI api;
    if (api.Init(nullptr, lang)) {
        if (debugText) *debugText = QStringLiteral("Tesseract init failed");
        return false;
    }
    api.SetPageSegMode(tesseract::PSM_AUTO);
    api.SetVariable("user_defined_dpi", "300");

    // 转 Pix
    cv::Mat gray; cv::cvtColor(bgrImage, gray, cv::COLOR_BGR2GRAY);
    cv::Mat bin; cv::threshold(gray, bin, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY);
    cv::Mat rgb; cv::cvtColor(bin, rgb, cv::COLOR_GRAY2RGB);
    Pix* pix = pixCreate(rgb.cols, rgb.rows, 32);
    // 写入像素
    for (int y = 0; y < rgb.rows; ++y) {
        const uchar* row = rgb.ptr<uchar>(y);
        for (int x = 0; x < rgb.cols; ++x) {
            int idx = (y * rgb.cols + x) * 3;
            l_uint32 val = (row[idx] << 24) | (row[idx+1] << 16) | (row[idx+2] << 8) | 0xFF;
            pixSetPixel(pix, x, y, val);
        }
    }

    api.SetImage(pix);
    api.Recognize(nullptr);
    tesseract::ResultIterator* ri = api.GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
    bool found = false;
    if (ri) {
        do {
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);
            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);
            QString w = QString::fromUtf8(word ? word : "");
            delete [] word;
            if (w.contains(query, Qt::CaseSensitive)) {
                if (outRect) *outRect = QRect(x1, y1, x2 - x1, y2 - y1);
                if (outConf) *outConf = conf;
                found = true;
                break;
            }
        } while (ri->Next(level));
    }
    pixDestroy(&pix);
    api.End();
    if (debugText && !found) *debugText = QStringLiteral("Text not found");
    return found;
}
#endif


