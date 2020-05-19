#-------------------------------------------------
#
# Project created by QtCreator 2020-01-06T12:57:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QT_FACE_TRACKER_DEMO
TEMPLATE = app

include(shared.pri)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        MainWindow.cpp \
        PDM.cc \
        CLM.cc \
        Patch.cc \
        PAW.cc \
        FDet.cc \
        Tracker.cc \
        FCheck.cc \
        IO.cc \

HEADERS += \
        MainWindow.h

FORMS += \
        MainWindow.ui

INCLUDEPATH += \
        $$PWD/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

macx {
    OPENCV_PATH = /usr/local/opt/opencv@3 # brew install opencv@3
    OPENCV_VERSION = 3.4
    OPENCV_LIB_SUFFIX = .$$OPENCV_VERSION

    INCLUDEPATH += \
        $$OPENCV_PATH/include \

    LIBS += \
        -L"$$OPENCV_PATH/lib" \

    LIBS += \
        -lopencv_core$$OPENCV_LIB_SUFFIX \
        -lopencv_highgui$$OPENCV_LIB_SUFFIX \
        -lopencv_imgproc$$OPENCV_LIB_SUFFIX \
        -lopencv_videoio$$OPENCV_LIB_SUFFIX \
        -lopencv_imgcodecs$$OPENCV_LIB_SUFFIX \
        -lopencv_objdetect$$OPENCV_LIB_SUFFIX \
        -lopencv_calib3d$$OPENCV_LIB_SUFFIX \
}

RESOURCES += \
    Resources.qrc
