#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <FaceTracker/Tracker.h>

#include <opencv2/opencv.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void showEvent(QShowEvent *event) override;

private slots:
    void updateWindow();

private:
    void __startCapture(void);
    void __showFrame(const cv::Mat &frame);
    void __draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi);

private:
    Ui::MainWindow *ui;

    QTimer *m_timer;

    cv::VideoCapture m_cap;

    cv::Mat m_frame;

    std::shared_ptr<FACETRACKER::Tracker> m_trackerPtr;
    cv::Mat m_tri;
    cv::Mat m_con;
    cv::Mat m_pose;

    bool m_trackingFailed = false, m_fcheck = false;
    std::vector<int> m_wSize1;
    std::vector<int> m_wSize2;
    int m_nIter = 5;
    double m_clamp = 3, m_fTol = 0.01;

    bool m_init = false;
};

#endif // MAINWINDOW_H
