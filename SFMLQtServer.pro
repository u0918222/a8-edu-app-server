#-------------------------------------------------
#
# Project created by QtCreator 2016-11-17T10:25:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SFMLQtServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ServerPing.cpp

HEADERS  += mainwindow.h \
    ServerPing.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/SFML-2.4.1-windows-vc14-32-bit/SFML-2.4.1/lib/ -lsfml-graphics -lsfml-main -lsfml-network -lsfml-window -lsfml-system -lsfml-audio
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/SFML-2.4.1-windows-vc14-32-bit/SFML-2.4.1/lib/ -lsfml-graphics-d -lsfml-main-d -lsfml-network-d -lsfml-window-d -lsfml-system-d -lsfml-audio-d

LIBS += -L$$PWD/SFML-2.4.1-windows-vc14-32-bit/SFML-2.4.1/bin

INCLUDEPATH += SFML-2.4.1-windows-vc14-32-bit/SFML-2.4.1/include
DEPENDPATH += SFML-2.4.1-windows-vc14-32-bit/SFML-2.4.1/bin

win32: LIBS += -L$$PWD/../mysql-5.7.16-win32/lib/ -llibmysql

INCLUDEPATH += $$PWD/../mysql-5.7.16-win32/include
DEPENDPATH += $$PWD/../mysql-5.7.16-win32/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../mysql-5.7.16-win32/lib/libmysql.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../mysql-5.7.16-win32/lib/liblibmysql.a
