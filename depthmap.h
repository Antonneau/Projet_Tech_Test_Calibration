#ifndef DEPTHMAP_H
#define DEPTHMAP_H

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


class depthmap
{
public:
    depthmap();
    static void Depthmap(QStringList sList, int numBoards,int numCornersH, int numCornersV, bool isVideo);
};

#endif // DEPTHMAP_H
