#ifndef DISPARITYPROCESS_H
#define DISPARITYPROCESS_H

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <QImage>

class MainWindow;

class DisparityProcess
{
public:
    DisparityProcess(MainWindow* parent);
    void setMode(int value);
    void setSBMnumDisp(int value);
    void setSBMblockSize(int value);
    void setMinDisp(int value);
    void setSGBMnumDisp(int value);
    void setSGBMblockSize(int value);
    void setP1(int value);
    void setP2(int value);
    void setDisp12MaxDiffChange(int value);
    void setPrefilter(int value);
    void setUniquenessRatio(int value);
    void setSpeckleWindowsSize(int value);
    void setSpeckleRange(int value);
    void setSGBMmode(int value);

private:
    void updatePicture(void);
    void process(void);

    MainWindow* _parent;

    int _mode;

    int _SBMnumDisparity;
    int _SBMblockSize;

    int _minDisparity;
    int _SGBMnumDisparity;
    int _SGBMblockSize;
    int _p1;
    int _p2;
    int _disp12MaxDiff;
    int _preFilter;
    int _UniquenessRatio;
    int _speckleWindowsSize;
    int _speckleRange;
    int _SGBMmode;


};

#endif // DISPARITYPROCESS_H
