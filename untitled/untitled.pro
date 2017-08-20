#-------------------------------------------------
#
# Project created by QtCreator 2016-03-14T16:46:29
#
#-------------------------------------------------

QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app



SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += D:\myLib\ffmpeg-3.3.3-win64-dev\include
LIBS += -LD:\myLib\ffmpeg-3.3.3-win64-dev\lib \
        -lavcodec \
        -lavfilter \
        -lavformat \
        -lswscale \
        -lavutil

#INCLUDEPATH += E:\opencv\opencv_249\build\include \
#                E:\opencv\opencv_249\build\include\opencv \
#                E:\opencv\opencv_249\build\include\opencv2

#LIBS += E:\opencv\opencv_249\build\x64\mingw\lib\libopencv_core249d.dll.a \
#        E:\opencv\opencv_249\build\x64\mingw\lib\libopencv_highgui249d.dll.a \
#        E:\opencv\opencv_249\build\x64\mingw\lib\libopencv_imgproc249d.dll.a \
#        E:\opencv\opencv_249\build\x64\mingw\lib\libopencv_features2d249d.dll.a \
#        E:\opencv\opencv_249\build\x64\mingw\lib\libopencv_calib3d249d.dll.a \
#        E:\opencv\opencv_249\build\x64\mingw\lib\libopencv_objdetect249d.dll.a \
#        E:\opencv\opencv_249\build\x64\mingw\lib\libopencv_imgproc249d.dll.a
