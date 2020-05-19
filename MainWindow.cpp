#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_timer = new QTimer(this);

    QString targetModelFilePath = QApplication::applicationDirPath() + "/face2.tracker";
    QFile::copy(":/model/face2.tracker", targetModelFilePath);

    QString targetTriFilePath = QApplication::applicationDirPath() + "/face.tri";
    QFile::copy(":/model/face.tri", targetTriFilePath);

    QString targetConFilePath = QApplication::applicationDirPath() + "/face.con";
    QFile::copy(":/model/face.con", targetConFilePath);

    qDebug() << "QApplication::applicationDirPath() : " << QApplication::applicationDirPath();

    m_trackerPtr = std::make_shared<FACETRACKER::Tracker>(targetModelFilePath.toStdString().c_str());

    m_pose = m_trackerPtr->_clm._pglobl;

    m_tri = FACETRACKER::IO::LoadTri(targetTriFilePath.toStdString().c_str());
    m_con = FACETRACKER::IO::LoadCon(targetConFilePath.toStdString().c_str());

    m_wSize1.push_back(7);
    m_wSize2.push_back(11);
    m_wSize2.push_back(9);
    m_wSize2.push_back(7);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    if (!m_init)
    {
        m_init = true;

        __startCapture();
    }
}

void MainWindow::updateWindow()
{
    m_cap >> m_frame;

    cv::flip(m_frame, m_frame, 1);



    cv::Mat gray;
    cv::cvtColor(m_frame, gray, CV_BGRA2GRAY);

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    std::vector<int> wSize = m_trackingFailed ? m_wSize2 : m_wSize1;

    if (m_trackerPtr->Track(gray, wSize, -1, m_nIter, m_clamp, m_fTol, m_fcheck) == 0)
    {
        int idx = m_trackerPtr->_clm.GetViewIdx();
        m_trackingFailed = false;
        __draw(m_frame, m_trackerPtr->_shape, m_con, m_tri, m_trackerPtr->_clm._visi[idx]);
    }
    else
    {
        qDebug() << "FrameReset!!";
        m_trackerPtr->FrameReset();
        m_trackingFailed = true;
    }

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    qDebug() << "FaceTracker duration : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms";

    cv::cvtColor(m_frame, m_frame, CV_BGRA2RGBA);

    QImage qImg((const unsigned char*) m_frame.data, m_frame.cols, m_frame.rows, QImage::Format_RGBA8888);

    ui->label->setPixmap(QPixmap::fromImage(qImg));
}

void MainWindow::__startCapture(void)
{
    const int labelWidth = ui->label->width();
    const int labelHeight = ui->label->height();

    m_cap.open(0);
    m_cap.set(CAP_PROP_FRAME_WIDTH, labelWidth);
    m_cap.set(CAP_PROP_FRAME_HEIGHT, labelHeight);

    if (!m_cap.isOpened())
        return;

    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateWindow()));
    m_timer->start(20);
}

void MainWindow::__draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi)
{
    int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;

    //draw triangulation
    c = CV_RGB(0,0,0);

    for(i = 0; i < tri.rows; i++)
    {
        if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
           visi.at<int>(tri.at<int>(i,1),0) == 0 ||
           visi.at<int>(tri.at<int>(i,2),0) == 0)
            continue;

        p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),shape.at<double>(tri.at<int>(i,0)+n,0));
        p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),shape.at<double>(tri.at<int>(i,1)+n,0));

        cv::line(image,p1,p2,c);

        p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),shape.at<double>(tri.at<int>(i,0)+n,0));
        p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),shape.at<double>(tri.at<int>(i,2)+n,0));

        cv::line(image,p1,p2,c);

        p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),shape.at<double>(tri.at<int>(i,2)+n,0));
        p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),shape.at<double>(tri.at<int>(i,1)+n,0));

        cv::line(image,p1,p2,c);
    }

    //draw connections
    c = CV_RGB(0,0,255);

    for(i = 0; i < con.cols; i++)
    {
        if(visi.at<int>(con.at<int>(0,i),0) == 0 ||
           visi.at<int>(con.at<int>(1,i),0) == 0)
            continue;

        p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),shape.at<double>(con.at<int>(0,i)+n,0));
        p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),shape.at<double>(con.at<int>(1,i)+n,0));

        cv::line(image,p1,p2,c,1);
    }

    //draw points
    for(i = 0; i < n; i++)
    {
        if(visi.at<int>(i,0) == 0)
            continue;

        p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));

        c = CV_RGB(255,0,0);

        cv::circle(image,p1,2,c);
    }

    double pitch = m_pose.at<double>(1, 0) * 180 / 3.14;
    double yaw = m_pose.at<double>(2, 0) * 180 / 3.14;
    double roll = m_pose.at<double>(3, 0) * 180 / 3.14;

    QString strYaw("yaw : %1");
    strYaw = strYaw.arg(yaw);

    QString strPitch("pitch : %1");
    strPitch = strPitch.arg(pitch);

    QString strRoll("roll : %1");
    strRoll = strRoll.arg(roll);

    cv::putText(image, strYaw.toStdString().c_str(), cv::Point(10, 50), CV_FONT_HERSHEY_DUPLEX, 1, CV_RGB(0,255,0), 2);
    cv::putText(image, strPitch.toStdString().c_str(), cv::Point(10, 100), CV_FONT_HERSHEY_DUPLEX, 1, CV_RGB(0,255,0), 2);
    cv::putText(image, strRoll.toStdString().c_str(), cv::Point(10, 150), CV_FONT_HERSHEY_DUPLEX, 1, CV_RGB(0,255,0), 2);

    cv::rectangle(image, m_trackerPtr->_rect, CV_RGB(0, 0, 255), 1);
}
