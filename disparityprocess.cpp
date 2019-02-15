#include "disparityprocess.h"
#include "mainwindow.h"

DisparityProcess::DisparityProcess(MainWindow* parent)
    : _mode(0), _SBMnumDisparity(0), _SBMblockSize(21), _minDisparity(-64), _SGBMnumDisparity(192),
      _SGBMblockSize(5), _p1(600), _p2(2400), _disp12MaxDiff(10), _preFilter(4), _UniquenessRatio(1),
      _speckleWindowsSize(150), _speckleRange(2), _SGBMmode(cv::StereoSGBM::MODE_SGBM)
{
    this->_parent = parent;
}

void DisparityProcess::process() {
    cv::Mat mat, gLeft, gRight, disp, disp8;

    mat = Utils::Convert::qImage::toCvMat(this->_parent->getOriginalPicture(), true);
    //Division de l'image source en deux images gauche et droite
    cv::Mat left = mat.colRange(0, mat.cols/2); //Création de l'image gauche
    int cropping = 0;
    //Si la largeur de l'image (en pixels) est impaire on retire un pixel de largeur sur l'image droite
    if(mat.cols%2 != 0) cropping = 1;
    cv::Mat right = mat.colRange(mat.cols/2+cropping, mat.cols); //Création de l'image droite

    cv::cvtColor(left, gLeft, CV_BGR2GRAY);
    cv::cvtColor(right, gRight, CV_BGR2GRAY);


    if(this->_mode == 0) {
        cv::Ptr<cv::StereoBM> sbm = cv::StereoBM::create(this->_SBMnumDisparity, this->_SBMblockSize);
        sbm->compute(gLeft, gRight, disp);
    } else if(this->_mode == 1) {
        // default param : -64,192,5,600,2400,10,4,1,150,2,cv::StereoSGBM::MODE_SGBM
        cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(
                    this->_minDisparity ,
                    this->_SGBMnumDisparity,
                    this->_SGBMblockSize,
                    this->_p1,
                    this->_p2,
                    this->_disp12MaxDiff,
                    this->_preFilter,
                    this->_UniquenessRatio,
                    this->_speckleWindowsSize,
                    this->_speckleRange,
                    this->_SGBMmode
                    );
        sgbm->compute(gLeft, gRight, disp);
    }


    cv::normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);
    QImage pic = Utils::Convert::CvMat::toQImage(&disp8, true);
    this->_parent->setPicture(pic);
}

void DisparityProcess::updatePicture() {
    std::cout << "Update de l'image" << std::endl;
    this->process();
    this->_parent->updateImage();
}

void DisparityProcess::setMode(int value) {
    _mode = value;
    this->updatePicture();
}

// value must be positive and divisible by 16
void DisparityProcess::setSBMnumDisp(int value) {
    if (value > 0 && value % 16 == 0) {
        _SBMnumDisparity = value;
        this->updatePicture();
    }
}

// value must be odd
void DisparityProcess::setSBMblockSize(int value) {
    if (value & 1) {
        _SBMblockSize = value;
        this->updatePicture();
    }
}

void DisparityProcess::setMinDisp(int value) {
    _minDisparity = value;
    this->updatePicture();
}

// value need to be greater than 0 and divisible by 16
void DisparityProcess::setSGBMnumDisp(int value) {
    if (value > 0 && value % 16 == 0) {
        _SGBMnumDisparity = value;
        this->updatePicture();
    }
}

// value must be odd and in the range 3-11
void DisparityProcess::setSGBMblockSize(int value) {
    if (value & 1 && value >= 3 && value <= 11) {
        _SGBMblockSize = value;
        this->updatePicture();
    }
}

void DisparityProcess::setP1(int value) {
    _p1 = value;
    this->updatePicture();
}

// value must be > to _p1
void DisparityProcess::setP2(int value) {
    if (value > this->_p1) {
        _p2 = value;
        this->updatePicture();
    }
}

void DisparityProcess::setDisp12MaxDiffChange(int value) {
    _disp12MaxDiff = value;
    this->updatePicture();
}

void DisparityProcess::setPrefilter(int value) {
    _preFilter = value;
    this->updatePicture();
}

// value betwen 5-15 is good
void DisparityProcess::setUniquenessRatio(int value) {
    _UniquenessRatio = value;
    this->updatePicture();
}

// 0 disable this feature otherwise 50-200
void DisparityProcess::setSpeckleWindowsSize(int value) {
    if (value < 50 || value > 200){
        _speckleWindowsSize = 0;
    } else {
        _speckleWindowsSize = value;
    }
    this->updatePicture();
}

// value must be divisible by 16 or be 1 or 2
void DisparityProcess::setSpeckleRange(int value) {
    _speckleRange = value;
    this->updatePicture();
}

// 0 or 1 or 2
void DisparityProcess::setSGBMmode(int value) {
    _SGBMmode = value;
    this->updatePicture();
}
