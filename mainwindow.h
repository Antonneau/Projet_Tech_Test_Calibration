#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    void setIntrinsic(cv::Mat Intrinsic){
        intrinsic = Intrinsic;
    }

    void setDistcoeffs(cv::Mat Distcoeff){
        distcoeffs = Distcoeff;
    }

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void open(void);
    void about(void);
    void close(void);
    void getCalibrationParam(void);
    void getCalibrationParamVid(void);
    void applyUndistort(void);

private:
    void createMenu(void);
    void createAction(void);
    void createTab(void);
    //char* argval[];
    cv::Mat toCvMat(QImage *img, bool copy);

    QStringList* fileList;
    QMenu* fileMenu;
    QMenu* aboutMenu;

    QAction* openFileAct;
    QAction* exitAppAct;
    QAction* aboutAct;
    QAction* calibrAct;
    QAction* calibrActVid;
    QAction* undistAct;

    QTabWidget* tabWidget;

    QLabel* label;
    QImage* picture;

    QWidget* sliderWidget;
    QLabel* sliders;

    QLabel* originalPic;
    QLabel* QImageLabel;
    QLabel* CVMatriceLabel;
    QLabel* reconstructedChess;

    cv::Mat intrinsic;
    cv::Mat distcoeffs;
    //bool parametersGet;
    //std::vector<cv::Mat>* rvecs;
    //std::vector<cv::Mat>* tvecs;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
