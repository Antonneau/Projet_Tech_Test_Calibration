#include "calibration.h"
#include "mainwindow.h"
#include "utils.h"

using namespace std;


void calibration::calib(MainWindow* win, QStringList sList, int numBoards,int numCornersH, int numCornersV, bool isVideo){

    std::list<cv::Mat> matList;     // List of matrix of the given images
    cv::VideoCapture capture;

    if(isVideo){
        cout << sList.at(0).toStdString() << endl;
        capture = cv::VideoCapture(sList.at(0).toStdString());
    } else {

        printf("Entering calibration\n");
        QImage *tmpImage = new QImage();

        for(int c = 0; c < sList.size(); c++){  // We give to matList all the images
            printf("loading img %d...\n", c);
            tmpImage->load(sList.at(c));
            cv::Mat tmpMat = Utils::Convert::qImage::toCvMat(tmpImage, true);
            matList.push_back(tmpMat);
            printf("Loagind %d done!\n", c);
        }
    }

    cv::Size boardSize = cv::Size(numCornersH, numCornersV);    // The size of the chessboard

    vector<vector<cv::Point3f>> objectPoints;   // Points in a 3D space
    std::vector<std::vector<cv::Point2f>> imagePoints;  // Points on the image (2D)
    std::vector<cv::Point2f> corners;   // Corners of the chessboard
    int successes = 0;  // Number of images skipped
    int numSquares = numCornersH * numCornersV; // Total number of squares

    cv::Mat image;  // Current image
    cv::Mat grayImage;  // Image converted to a gray scale image
    vector<cv::Point3f> obj;    // Points of the chessboard in the current image

    if(isVideo){
        capture >> image;
    } else {
        image = matList.front();    // Getting the first image
    }

    // Entering pattern searching loop
    while(successes < numBoards){
        // Convert the image to a gray scale image
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
        // Refreshing image
        if(!isVideo)
            image = matList.front();
        // Searching for a pattern
        bool patternFound = cv::findChessboardCorners(grayImage, boardSize, corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS);

        if  (patternFound){
            // If a pattern is found, keep it and draw the current points of the chessboard
            cv::cornerSubPix(grayImage, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            cv::drawChessboardCorners(grayImage, boardSize, corners, patternFound);

            obj.clear();
            for(int x = 0; x < numCornersV; x++){
                for(int y = 0; y < numCornersH; y++){
                    obj.push_back(cv::Point3f((float)y * numSquares, (float)x * numSquares, 0));
                }
            }

            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
            cout << "Snap stored!" << endl;
            successes++;
            if(isVideo)
                capture >> image;
            else
                matList.pop_front();
            if(successes>=numBoards)
                break;
        } else {
            // Else find another pattern
            cout << "Snap skipped" << endl;
            successes++;
            if(isVideo)
                capture >> image;
            else
                matList.pop_front();
            if(successes>=numBoards)
                break;
        }
    }
    std::vector<cv::Mat> rvecs;
    std::vector<cv::Mat> tvecs;

    cv::Mat newIntrinsic;
    cv::Mat newDistCoeffs;

    // Do the calibration with the parameters
    cv::calibrateCamera(objectPoints, imagePoints, image.size(), newIntrinsic, newDistCoeffs, rvecs, tvecs);

    win->setIntrinsic(newIntrinsic);
    win->setDistcoeffs(newDistCoeffs);

    //  POUR LA CARTE DE PROFONDEUR
    //
    //cv::stereoCalibrate
    //cv::stereoRectify     Donne la matrice Q utilisée par la fonction reprojectImageTo3D
    //cv::initUndistortRectifyMap()
    //cv::remap()
    //cv::reprojectImageTo3D()
}

void calibration::undistort(QStringList sList, cv::Mat intrin, cv::Mat distco){
    // Matrix of the images (one for the images left, the other for skipped images)
    std::list<cv::Mat> undistortMatList;
    std::list<cv::Mat> pastUndistMat;

    printf("Entering undistort\n");
    QImage *tmpImage = new QImage();

    // Loading images
    for(int c = 0; c < sList.size(); c++){
        printf("loading img %d...\n", c);
        tmpImage->load(sList.at(c));
        cv::Mat tmpMat = Utils::Convert::qImage::toCvMat(tmpImage, true);
        undistortMatList.push_back(tmpMat);
        printf("Loagind %d done!\n", c);
    }

    // Get images
    cv::Mat image = undistortMatList.front();
    cv::Mat imageUndistored;

    while(1){
        // Undistort the current frame
        cv::undistort(image, imageUndistored, intrin, distco);
        cv::imshow("Base Image", image);
        cv::imshow("Undistored Image", imageUndistored);
        int key = cv::waitKey(1);
        if(key==27){
            // If "espace" is pressed, leave undistort
            cv::destroyWindow("Base Image");
            cv::destroyWindow("Undistored Image");
            break;
        }
        if(key == 'd' && undistortMatList.size() >= 2){
            // If "d" is pressed and there is an image left, go to the next frame
            pastUndistMat.push_front(undistortMatList.front());
            undistortMatList.pop_front();
            image = undistortMatList.front();
        }
        if(key == 'q' && pastUndistMat.size() >= 1){
            // If "q" is pressed and there is an image skipped, go to the previous frame
            undistortMatList.push_front(pastUndistMat.front());
            pastUndistMat.pop_front();
            image = undistortMatList.front();
        }
    }
}

