#include "utils.h"

namespace Utils {

namespace Convert {

namespace CvMat {

    QImage toQImage(cv::Mat *mat, bool copy) {
        QImage::Format format;
        switch(mat->type()) {
            case CV_8U :{
                format = QImage::Format_Indexed8;
                break;
            }

            case CV_8UC4 :{
               format = QImage::Format_ARGB32;
               break;
            }
        }

        QImage img = QImage(mat->data, mat->cols, mat->rows, mat->step, format);
        return ( copy ) ? img.copy() : img ;
    }

} // end namespace CvMat

namespace qImage {
    cv::Mat toCvMat(QImage *img, bool copy) {
        cv::Mat mat = cv::Mat(img->height(), img->width(), CV_8UC4, const_cast<uchar*>(img->bits()), img->bytesPerLine());
        return ( copy ) ? mat.clone() : mat ;
    }
} // end namespace qImage

} // end namespace Converter

} // end namespace utils

