#include "depthmap.h"

void depthmap::depthmap(QStringList sList, int numBoards,int numCornersH, int numCornersV, bool isVideo){

    std::list<cv::Mat> matList, matList2;     // List of matrix of the given images
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
            matList2.push_back(tmpMat);
            printf("Loagind %d done!\n", c);
        }
    }

    cv::Size boardSize = cv::Size(numCornersH, numCornersV);    // The size of the chessboard

    vector<vector<cv::Point3f>> objectPoints;   // Points in a 3D space
    std::vector<std::vector<cv::Point2f>> imagePoints1, imagePoints2;  // Points on the image (2D)
    std::vector<cv::Point2f> corners;   // Corners of the chessboard
    int successes = 0;  // Number of images skipped
    int numSquares = numCornersH * numCornersV; // Total number of squares

    cv::Mat image;  // Current image
    cv::Mat grayImage;  // Image converted to a gray scale image
    vector<cv::Point3f> obj;    // Points of the chessboard in the current image

    cv::Size imageSize = image.size;

    if(isVideo){
        capture >> image;
    } else {
        image = matList.front();    // Getting the first image
    }

    // Entering pattern searching loop

    // LOOP FOR VIDEO 1
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

            imagePoints1.push_back(corners);
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

    successes = 0;
    // LOOP FOR SECOND VIDEO
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

            imagePoints2.push_back(corners);
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

    cv::Mat intrinsic1, intrinsic2, distcoeffs1, distcoeffs2, R, T, E, F;
    cv::stereoCalibrate(objectPoints, imagePoints1, imagePoints2, intrinsic1, distcoeffs1, intrinsic2, distcoeffs2, imageSize, R, T, E, F);

    cv::Mat R1, R2, P1, P2, Q;
    cv::stereoRectify(intrinsic1, distcoeffs1, intrinsic2, distcoeffs2, imageSize, R, T, R1, R2, P1, P2, Q);

    cv::Mat map1, map2;
    cv::initUndistortRectifyMap(intrinsic1, distcoeffs1, R1, P1, imageSize, CV_32FC1, map1, map2);

    image = matList2.front();
    cv::Mat correctedImg;
    cv::remap(image, correctedImg, map1, map2, cv::INTER_LINEAR);

    /*
     *
     *
     * APPELER LA FONCTION DE DISPARITE ICI AVEC LES IMAGES RECTIFIEES
     *
     *
     *
    */

    cv::Mat depthMap;
    cv::reprojectImageTo3D(disp, depthMap, Q);

}
