#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "calibration.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //this->argval = argv;
    //this->parametersGet = false;
    this->createAction();
    this->createMenu();
    this->createTab();

    undistAct->setDisabled(intrinsic.empty() && distcoeffs.empty());

    this->setCentralWidget(this->tabWidget);

    this->picture = new QImage();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createAction() {
    // Creating the actions linked to functions

    // Opening a single image
    this->openFileAct = new QAction("Ouvrir", this);
    this->openFileAct->setShortcut(QKeySequence::Open);
    connect(this->openFileAct, SIGNAL(triggered()), this, SLOT(open()));

    // Close the application
    this->exitAppAct = new QAction("Fermer", this);
    this->exitAppAct->setShortcut(QKeySequence::Close);
    connect(this->exitAppAct, SIGNAL(triggered()), this, SLOT(close()));

    // Do the calibration with images
    this->calibrAct = new QAction("Parametrer la calibration (Image)", this);
    connect(this->calibrAct, SIGNAL(triggered()), this, SLOT(getCalibrationParam()));

    // Do the calibration with a video (not implemented yet)
    this->calibrActVid = new QAction("Parametrer la calibration (Vidéo)", this);
    connect(this->calibrActVid, SIGNAL(triggered()), this, SLOT(getCalibrationParamVid()));

    // Undistort images
    this->undistAct = new QAction("Appliquer la calibration", this);
    connect(this->undistAct, SIGNAL(triggered()), this, SLOT(applyUndistort()));

    this->aboutAct = new QAction("Infos", this);
    connect(this->aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenu() {
    // Creating file menu
    this->fileMenu = menuBar()->addMenu("Fichier");
    this->fileMenu->addAction(openFileAct);
    this->fileMenu->addSeparator();
    this->fileMenu->addAction(calibrAct);
    this->fileMenu->addAction(calibrActVid);
    this->fileMenu->addAction(undistAct);
    this->fileMenu->addSeparator();
    this->fileMenu->addAction(exitAppAct);

    // Creating about menu
    this->aboutMenu = menuBar()->addMenu("A propos");
    this->aboutMenu->addAction(aboutAct);
}

void MainWindow::createTab() {
    this->tabWidget = new QTabWidget(this);

    this->originalPic = new QLabel(this->tabWidget);

    this->tabWidget->addTab(this->originalPic, "Image original");
}

void MainWindow::open() {
    QStringList fileList = QFileDialog::getOpenFileNames(this,
                                                "Sélectionnez des images",
                                                "Images/",
                                                "Image (*.png *.jpg)",
                                                NULL,
                                                QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly
                                                );

    if ( fileList.isEmpty() ) return;

    for(int i = 0; i < fileList.size(); i++){

        if ( !this->picture->load(fileList.at(i)) ) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir cette image");
            return;
        }
        if ( this->picture->isNull() ) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir une image vide");
            return;
        }
    }
    // On affiche l'image original sans aucune convertion
    this->originalPic->setPixmap(QPixmap::fromImage(*picture));

    }

void MainWindow::about() {
    QMessageBox::information(this, "A propos", "Application realise dans le cadre du projet tech de l'universite de Bordeaux ! Permet d'ouvrir et afficher une image de type \" QImage\" et de la convertir en type \" CvMatrice\"");
}

void MainWindow::close() {
    QMessageBox::StandardButton answer = QMessageBox::question(this, "Quitter", "Quitter l'application ?", QMessageBox::Yes | QMessageBox::No);
    if ( answer == QMessageBox::Yes ) {
       QApplication::quit();
    }
}

void MainWindow::getCalibrationParam(){
    QStringList fileList = QFileDialog::getOpenFileNames(this,
                                                "Sélectionnez des images",
                                                "Images/",
                                                "Image (*.png *.jpg)",
                                                NULL,
                                                QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly
                                                );

    if ( fileList.isEmpty() ) return;

    for(int i = 0; i < fileList.size(); i++){

        if ( !this->picture->load(fileList.at(i)) ) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir cette image");
            return;
        }
        if ( this->picture->isNull() ) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir une image vide");
            return;
        }
    }

    calibration::calib(this, fileList, fileList.size(), 9, 6, false);
    // Enable the undistort in the menu
    undistAct->setDisabled(intrinsic.empty() && distcoeffs.empty());
}

void MainWindow::getCalibrationParamVid(){
    QString file = QFileDialog::getOpenFileName(this,
                                                "Sélectionnez une video",
                                                "Video/",
                                                "Video (*.mp4)",
                                                NULL,
                                                QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly
                                                );

    if ( file.isEmpty() ) return;

    QStringList fileList;
    fileList.append(file);

    calibration::calib(this, fileList, fileList.size(), 9, 6, true);
    // Enable the undistort in the menu
    undistAct->setDisabled(intrinsic.empty() && distcoeffs.empty());

}

void MainWindow::applyUndistort(){
    QStringList fileList = QFileDialog::getOpenFileNames(this,
                                                "Sélectionnez des images",
                                                "Images/",
                                                "Image (*.png *.jpg)",
                                                NULL,
                                                QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly
                                                );

    if ( fileList.isEmpty() ) return;

    for(int i = 0; i < fileList.size(); i++){

        if ( !this->picture->load(fileList.at(i)) ) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir cette image");
            return;
        }
        if ( this->picture->isNull() ) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir une image vide");
            return;
        }
    }
    calibration::undistort(fileList, this->intrinsic, this->distcoeffs);
}

cv::Mat toCvMat(QImage *img, bool copy) {
    cv::Mat mat = cv::Mat(img->height(), img->width(), CV_8UC4, const_cast<uchar*>(img->bits()), img->bytesPerLine());
    return ( copy ) ? mat.clone() : mat ;
}
