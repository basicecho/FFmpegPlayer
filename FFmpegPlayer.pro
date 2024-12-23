QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    ffmpegplayer.cpp \
    ffmpegsource.cpp \
    framebuffer.cpp \
    main.cpp \
    mainclass.cpp \
    myconfig.cpp \
    widget.cpp

HEADERS += \
    ffmpegplayer.h \
    ffmpegsource.h \
    framebuffer.h \
    mainclass.h \
    myconfig.h \
    widget.h

FORMS += \
    widget.ui

DISTFILES += \
    config/config.ini

unix
{
    INCLUDEPATH += $(HOME)/Software/ffmpeg/include
    QMAKE_LIBDIR += $(HOME)/Software/ffmpeg/lib

    LIBS += -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale \
        -lpthread
}

win32
{
    INCLUDEPATH += D:\ffmpeg\ffmpeg-master-latest-win64-gpl-shared\include
    QMAKE_LIBDIR += D:\ffmpeg\ffmpeg-master-latest-win64-gpl-shared\lib
    LIBS += -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale
}
