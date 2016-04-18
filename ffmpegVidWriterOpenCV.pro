TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += link_pkgconfig
PKGCONFIG += libavcodec opencv  libavutil libswscale libavformat

SOURCES += main.cpp

HEADERS += \
    cap_ffmpeg_impl.hpp

