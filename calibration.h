#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QMainWindow>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

#include <mainwindow.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <vector>

class calibration
{
public:
    calibration();
    static void calib(MainWindow* win, QStringList sList, int numBoards,int numCornersH, int numCornersV, bool isVideo);
    static void undistort(QStringList sList, cv::Mat intrin, cv::Mat distco);
};

#endif // CALIBRATION_H

