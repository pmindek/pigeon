QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        canvas.cpp \
        main.cpp \
        mainwindow.cpp \
	util/vjmini/sources/mappingmanager.cpp \
	util/vjmini/sources/parameter.cpp \
	util/vjmini/sources/resourcemanager.cpp \
	util/vjmini/sources/resourceprovider.cpp \
	util/vjmini/sources/resourcerenderer.cpp \
	util/vjmini/sources/shadermanager.cpp \
	util/vjmini/sources/textureprovider.cpp \
		util/vjmini/vjmini.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG(release, debug|release):DESTDIR = $$PWD/production/release
else:DESTDIR = $$PWD/production/debug

#nn
CONFIG(release, debug|release):LIBS += $$PWD/production/release/nn.lib
else:LIBS += $$PWD/production/debug/nn.lib
INCLUDEPATH += $$PWD/libs/nn/src

HEADERS += \
	canvas.h \
	mainwindow.h \
	util/nn_global.h \
	util/vjmini/sources/mappingmanager.h \
	util/vjmini/sources/parameter.h \
	util/vjmini/sources/resourcemanager.h \
	util/vjmini/sources/resourceprovider.h \
	util/vjmini/sources/resourcerenderer.h \
	util/vjmini/sources/shadermanager.h \
	util/vjmini/sources/textureprovider.h \
	util/vjmini/vjmini.h

QT += widgets
