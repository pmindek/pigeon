#-------------------------------------------------
#
# Project created by QtCreator 2017-01-02T20:06:15
#
#-------------------------------------------------

QT       += widgets opengl

TARGET = nn
TEMPLATE = lib

INCLUDEPATH += src

DEFINES += NN_LIBRARY

CONFIG(release, debug|release):DESTDIR = $$PWD/../../production/release
else:DESTDIR = $$PWD/../../production/debug

SOURCES += \
	src/nn.cpp \
	src/nnu.cpp \
	src/resources.nn/nnresource.cpp \
	src/resources.nn/nnshader.cpp \
	src/resources.nn/nnpagedarray.cpp \
	src/util.nn/Perlin.cpp \
	src/util.nn/SimplexNoise.cpp \
    src/util.nn/gizmos.cpp \
    src/camera.cpp \
    src/cameracontroller.cpp

HEADERS += \
	src/nn_global.h \
	src/nn.h \
	src/nnu.h \
	src/resources.nn/nnresource.h \
	src/resources.nn/nnshader.h \
	src/resources.nn/nnpagedarray.h \
	src/util.nn/Perlin.h \
	src/util.nn/SimplexNoise.h \
    src/util.nn/gizmos.h \
    src/camera.h \
    src/cameracontroller.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
