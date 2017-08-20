#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

#define ENABLE_OPENCV 0
#include <QMainWindow>
#include <QVideoEncoderSettings>
#include <QMutex>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>

#if ENABLE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#endif

extern "C"
{
//必须用按c语言编译
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVideoEncoderSettings *videoCom;

    void StartSream();
    void stopStream();
    bool Init();

    void detectAndDisplay( const QImage &image );

#if ENABLE_OPENCV
    QImage cvMat2QImage(const cv::Mat& mat);
    cv::Mat QImage2cvMat(QImage image);
    cv::Mat image;  // the image variable
#endif

    QMutex mutex;
    AVPicture  pAVPicture;
    AVFormatContext *pAVFormatContext;
    AVCodecContext *pAVCodecContext;
    AVFrame *pAVFrame;
    SwsContext * pSwsContext;
    AVPacket pAVPacket;

    QTimer *m_timerPlay;
    QString m_str_url;
    int videoStreamIndex;
    int m_i_frameFinished;
    int videoHeight,videoWidth;



public slots:
    void SetImageSlots(const QImage &image);
    void playSlots();

signals:
    void GetImage(const QImage &image);
private slots:
    void on_Btn_Start_clicked();
    void on_Btn_Stop_clicked();
};

#endif // MAINWINDOW_H
