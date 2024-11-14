QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    ffmpegplayer.cpp \
    main.cpp \
    myconfig.cpp \
    widget.cpp

HEADERS += \
    ffmpegplayer.h \
    myconfig.h \
    widget.h

FORMS += \
    widget.ui

unix
{
    INCLUDEPATH += $(HOME)/Software/ffmpeg/include

    QMAKE_LIBDIR += $(HOME)/Software/ffmpeg/lib

    LIBS += -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale
}

DISTFILES += \
    config/config.ini
