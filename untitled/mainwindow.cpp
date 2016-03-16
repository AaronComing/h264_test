#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"string"

using namespace cv;
string face_cascade_name = "haarcascade_frontalface_alt2.xml";
//该文件存在于OpenCV安装目录下的\sources\data\haarcascades内，需要将该xml文件复制到当前工程目录下
cv::CascadeClassifier face_cascade;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_str_url = "http://192.168.1.106:8090";

    m_timerPlay = new QTimer;
    m_timerPlay->setInterval(7);
    connect(m_timerPlay,SIGNAL(timeout()),this,SLOT(playSlots()));
    connect(this,SIGNAL(GetImage(const QImage )),this,SLOT(SetImageSlots(const QImage )));

    m_i_frameFinished = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::StartSream(){

    videoStreamIndex=-1;
    av_register_all();//注册库中所有可用的文件格式和解码器
    avformat_network_init();//初始化网络流格式,使用RTSP网络流时必须先执行
    pAVFormatContext = avformat_alloc_context();//申请一个AVFormatContext结构的内存,并进行简单初始化
    pAVFrame=av_frame_alloc();
    if (this->Init())
    {
        m_timerPlay->start();
    }
}

void MainWindow::stopStream()
{
    m_timerPlay->stop();

    avformat_free_context(pAVFormatContext);
    av_frame_free(&pAVFrame);
    sws_freeContext(pSwsContext);
}


bool MainWindow::Init()
{
    if( !face_cascade.load( face_cascade_name) ){
        qDebug()<<"级联分类器错误，可能未找到文件，拷贝该文件到工程目录下！";
        return false;
    }
    if(m_str_url.isEmpty())
        return false;
    //打开视频流
    int result=avformat_open_input(&pAVFormatContext, m_str_url.toStdString().c_str(),NULL,NULL);
    if (result<0){
        qDebug()<<"打开视频流失败";
        return false;
    }

    //获取视频流信息
    result=avformat_find_stream_info(pAVFormatContext,NULL);
    if (result<0){
        qDebug()<<"获取视频流信息失败";
        return false;
    }

    //获取视频流索引
    videoStreamIndex = -1;
    for (uint i = 0; i < pAVFormatContext->nb_streams; i++) {
        if (pAVFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex==-1){
        qDebug()<<"获取视频流索引失败";
        return false;
    }

    //获取视频流的分辨率大小
    pAVCodecContext = pAVFormatContext->streams[videoStreamIndex]->codec;
    videoWidth=pAVCodecContext->width;
    videoHeight=pAVCodecContext->height;

    avpicture_alloc(&pAVPicture,AV_PIX_FMT_RGB24,videoWidth,videoHeight);

    AVCodec *pAVCodec;

    //获取视频流解码器
    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    pSwsContext = sws_getContext(videoWidth,videoHeight,AV_PIX_FMT_YUV420P,videoWidth,videoHeight,AV_PIX_FMT_RGB24,SWS_BICUBIC,0,0,0);

    //打开对应解码器
    result=avcodec_open2(pAVCodecContext,pAVCodec,NULL);
    if (result<0){
        qDebug()<<"打开解码器失败";
        return false;
    }

    qDebug()<<"初始化视频流成功";
    return true;
}

void MainWindow::playSlots()
{
    //一帧一帧读取视频
    if (av_read_frame(pAVFormatContext, &pAVPacket) >= 0){
        if(pAVPacket.stream_index==videoStreamIndex){
            qDebug()<<"开始解码"<<QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            avcodec_decode_video2(pAVCodecContext, pAVFrame, &m_i_frameFinished, &pAVPacket);
            if (m_i_frameFinished){
                mutex.lock();
                sws_scale(pSwsContext,(const uint8_t* const *)pAVFrame->data,pAVFrame->linesize,0,videoHeight,pAVPicture.data,pAVPicture.linesize);
                //发送获取一帧图像信号
                QImage image(pAVPicture.data[0],videoWidth,videoHeight,QImage::Format_RGB888);
                emit GetImage(image);
                mutex.unlock();
            }
        }
    }
    av_free_packet(&pAVPacket);//释放资源,否则内存会一直上升
}

void MainWindow::on_Btn_Start_clicked()
{
    StartSream();
}

void MainWindow::on_Btn_Stop_clicked()
{
    stopStream();
}


void MainWindow::SetImageSlots(const QImage &image)
{
    if (image.height()>0){


        detectAndDisplay(image); //调用人脸检测函数

    }
}

void MainWindow::detectAndDisplay(const QImage &image){
    std::vector<Rect> faces;
    Mat face_gray;
     cv::Mat face = QImage2cvMat(image);

    cvtColor( face, face_gray, CV_BGR2GRAY );  //rgb类型转换为灰度类型
    equalizeHist( face_gray, face_gray );   //直方图均衡化

    face_cascade.detectMultiScale( face_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(1, 1) );

    for( int i = 0; i < faces.size(); i++ ){
        Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        ellipse( face, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 0), 2,7, 0 );
    }

    QImage image1 = cvMat2QImage(face);
    QPixmap pix = QPixmap::fromImage(image1.scaled(640,360));
    ui->Lab_VideoOut->setPixmap(pix);
}


QImage MainWindow::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}
cv::Mat MainWindow::QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

