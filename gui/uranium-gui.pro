#-------------------------------------------------
#
# Project created by QtCreator 2019-06-01T14:53:00
#
#-------------------------------------------------

QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = uranium-gui
TEMPLATE = app

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

PROJECT_ROOT = $$PWD/..

SOURCES +=                       \
        main.cpp                 \
        CoreHandler.cpp          \
        MainWindow.cpp           \
        WebSocketClient.cpp      \
        WebSocketServer.cpp      \
        Dialogs.cpp              \
        AboutDialog.cpp          \
        ui/MainWindowUi.cpp      \
        ui/AboutUi.cpp           \
        log/LogImpl.cpp          \
        utils/modules.cpp        \
        utils/CQueue.cpp         \
        utils/Semaphore.cpp      \
        utils/SyncType.cpp       \
        utils/TimedSemaphore.cpp \
        utils/Timer.cpp          \
        utils/Times.cpp          \
        ../core/Config.cpp

HEADERS +=                       \
        MainWindow.h             \
        CoreHandler.h            \
        WebSocketClient.h        \
        WebSocketServer.h        \
        Dialogs.h                \
        AboutDialog.h            \
        log/LogImpl.h            \
        log/logs.h               \
        ui/MainWindowUi.h        \
        ui/AboutUi.h             \
        utils/modules.h          \
        utils/CQueue.h           \
        utils/RWLock.h           \
        utils/Semaphore.h        \
        utils/SyncType.h         \
        utils/TimedSemaphore.h   \
        utils/Timer.h            \
        utils/Times.h            \
        utils/clist.h            \
        utils/common.h           \
        ../core/Config.h

INCLUDEPATH +=                   \
        $$PWD                    \
        $$PWD/log                \
        $$PWD/utils              \
        $$PROJECT_ROOT           \
        $$PROJECT_ROOT/ipc       \
        $$PROJECT_ROOT/core

RESOURCES += resources/pictures.qrc

FORMS += \
        designer/MainWindow.ui  \
        designer/AboutDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
