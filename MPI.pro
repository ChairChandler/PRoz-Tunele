TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/openmpi/include

SOURCES += \
        main.cpp \
        models/packet.cpp \
        models/richmaninfo.cpp \
        msg/msgreceiver.cpp \
        msg/msgsender.cpp \
        richman/atomicrichmaninfo.cpp \
        richman/msgdispatcher.cpp \
        richman/richman.cpp \
        richman/tunnelwalker.cpp \
        thread/runnablethread.cpp \
        tunnel/tunnel.cpp \
        utils/distributedstream.cpp

HEADERS += \
    models/msgcomm.h \
    models/packet.h \
    models/place.h \
    models/richmaninfo.h \
    msg/msgreceiver.h \
    msg/msgsender.h \
    richman/atomicrichmaninfo.h \
    richman/msgdispatcher.h \
    richman/richman.h \
    thread/runnable.h \
    richman/tunnelwalker.h \
    thread/runnablethread.h \
    tunnel/limiteddeque.h \
    tunnel/limitedset.h \
    tunnel/tunnel.h \
    utils/appdebug.h \
    utils/distributedstream.h
