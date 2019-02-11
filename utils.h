#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/traits.hpp>

#include <QImage>

namespace Utils {

namespace Convert {

enum Mode { SBM, SGBM };

namespace CvMat {

    QImage toQImage(cv::Mat *mat, bool copy);

    cv::Mat toLaplacian(cv::Mat mat, bool reduceNoise);

    cv::Mat toSobel(cv::Mat mat, bool reduceNoise);

    cv::Mat toDisparity(cv::Mat mat, Convert::Mode mode);

    void reconstructChess(cv::Mat mat, int columns, int lines);

} // end namespace CvMat

namespace qImage {

    cv::Mat toCvMat(QImage *img, bool copy);

} // end namesapce QImage

} // end namespace Converter

} // end namespace utils

#endif // UTILS_H
