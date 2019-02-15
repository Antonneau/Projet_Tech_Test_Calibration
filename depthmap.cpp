#include "depthmap.h"
#include "utils.h"

using namespace std;

void depthmap::Depthmap(QStringList sList, int numBoards,int numCornersH, int numCornersV, bool isVideo){

    std::list<cv::Mat> matListL, matListR, matList2;     // List of matrix of the given images
    cv::VideoCapture capture;

    if(isVideo){
        cout << sList.at(0).toStdString() << endl;
        capture = cv::VideoCapture(sList.at(0).toStdString());
    } else {

        printf("Entering calibration\n");
        QImage *tmpImageL = new QImage();
        QImage *tmpImageR = new QImage();

        for(int c = 0; c < sList.size()/2; c++){  // We give to matList all the images
            printf("loading img %d and %d...\n", c+1, (c+1) + sList.size()/2);
            tmpImageL->load(sList.at(c));
            cv::Mat tmpMatL = Utils::Convert::qImage::toCvMat(tmpImageL, true);
            matListL.push_back(tmpMatL);
            tmpImageR->load(sList.at(c + sList.size()/2));
            cv::Mat tmpMatR = Utils::Convert::qImage::toCvMat(tmpImageR, true);
            matListR.push_back(tmpMatR);
            matList2.push_back(tmpMatL);
            matList2.push_back(tmpMatR);
            printf("Loading %d done!\n", c);
        }
    }

    cv::Size boardSize = cv::Size(numCornersH, numCornersV);    // The size of the chessboard

    vector<vector<cv::Point3f>> objectPoints;   // Points in a 3D space
    std::vector<std::vector<cv::Point2f>> imagePoints1, imagePoints2;  // Points on the image (2D)
    std::vector<cv::Point2f> cornersL, cornersR;   // Corners of the chessboard

    int successes = 0;  // Number of images skipped
    int numSquares = numCornersH * numCornersV; // Total number of squares

    cv::Mat imageL, imageR;  // Current image
    cv::Mat grayImageL, grayImageR;  // Image converted to a gray scale image
    vector<cv::Point3f> obj;    // Points of the chessboard in the current image

    // Entering pattern searching loop

    while(successes < numBoards/2){
        cout << "Snap number : " << successes +1 << " & " << successes+1+numBoards/2 << endl;

        // Refreshing image


        if(!isVideo){
            imageL = matListL.front();
            imageR = matListR.front();
        } else {
            capture >> imageL;
        }

        // Convert the image to a gray scale image
        cv::cvtColor(imageL, grayImageL, cv::COLOR_BGR2GRAY);
        cv::cvtColor(imageR, grayImageR, cv::COLOR_BGR2GRAY);

        // Searching for a pattern
        bool patternFoundL = cv::findChessboardCorners(grayImageL, boardSize, cornersL, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS);
        bool patternFoundR = cv::findChessboardCorners(grayImageR, boardSize, cornersR, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS);


        if  (patternFoundL && patternFoundR){
            // If a pattern is found, keep it and draw the current points of the chessboard
            cv::cornerSubPix(grayImageL, cornersL, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            cv::drawChessboardCorners(grayImageL, boardSize, cornersL, patternFoundL);
            cv::cornerSubPix(grayImageR, cornersR, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            cv::drawChessboardCorners(grayImageR, boardSize, cornersR, patternFoundR);

            obj.clear();
            for(int x = 0; x < numCornersV; x++){
                for(int y = 0; y < numCornersH; y++){
                    obj.push_back(cv::Point3f((float)y * numSquares, (float)x * numSquares, 0));
                }
            }

            imagePoints1.push_back(cornersL);
            imagePoints2.push_back(cornersR);
            objectPoints.push_back(obj);
            cout << "Snap stored!" << endl;
            successes++;
            if(isVideo)
                capture >> imageL;
            else {
                matListL.pop_front();
                matListR.pop_front();
            }
            if(successes>=numBoards)
                break;
        } else {
            // Else find another pattern
            cout << "!!! SNAP SKIPPED !!!" << endl;
            successes++;
            if(isVideo)
                capture >> imageL;
            else {
                matListL.pop_front();
                matListR.pop_front();
            }
            if(successes>=numBoards)
                break;
        }
    }

    cout << "Got all the points!" << endl;
    cout << imagePoints1.size() << " " << imagePoints2.size() << endl;

    cv::Mat image = matList2.front();
    matList2.pop_front();
    imageR = matList2.front();

    cv::Mat intrinsic1, intrinsic2, distcoeffs1, distcoeffs2, R, T, E, F;
    cv::stereoCalibrate(objectPoints, imagePoints1, imagePoints2, intrinsic1, distcoeffs1, intrinsic2, distcoeffs2, image.size(), R, T, E, F);

    cout << "Got stereo parameters..." << endl;

    cv::Mat R1, R2, P1, P2, Q;
    cv::stereoRectify(intrinsic1, distcoeffs1, intrinsic2, distcoeffs2, image.size(), R, T, R1, R2, P1, P2, Q);

    cout << "Rectifying camera..." << endl;

    cv::Mat map1, map2;
    cv::initUndistortRectifyMap(intrinsic1, distcoeffs1, R1, P1, image.size(), CV_32FC1, map1, map2);

    cout << "Starting to undistort..." << endl;

    cv::Mat correctedImgL, correctedImgR;
    cv::remap(image, correctedImgL, map1, map2, cv::INTER_LINEAR);
    cv::remap(imageR, correctedImgR, map1, map2, cv::INTER_LINEAR);

    cv::imshow("Corrected Image", correctedImgL);

    cout << "Remap done !" << endl;
    cout << "Creating disparity map..." << endl;

    cv::Mat disp = Utils::Convert::CvMat::toDisparity(correctedImgL, correctedImgR, Utils::Convert::SGBM);

    cout << "Creating depth map..." << endl;

    cv::Mat depthMap;
    cv::reprojectImageTo3D(disp, depthMap, Q);

    cout << "All done !" << endl;

    cv::imshow("Depth map", depthMap);

}
