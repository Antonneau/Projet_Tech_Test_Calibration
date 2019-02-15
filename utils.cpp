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

    cv::Mat toDisparity(cv::Mat matL, cv::Mat matR, Convert::Mode mode) {
        cv::Mat gLeft, gRight, disp, disp8;

        cv::cvtColor(matL, gLeft, CV_BGR2GRAY);
        cv::cvtColor(matR, gRight, CV_BGR2GRAY);


        if(mode == Convert::Mode::SBM ){
            cv::Ptr<cv::StereoBM> sbm = cv::StereoBM::create(0,21);
            sbm->compute(gLeft, gRight, disp);
        } else if(mode == Convert::Mode::SGBM){
            cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(-64,192,5,600,2400,10,4,1,150,2,cv::StereoSGBM::MODE_SGBM);
            sgbm->compute(gLeft, gRight, disp);
        }


        cv::normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);

        return disp8;
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

