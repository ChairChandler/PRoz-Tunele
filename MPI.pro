TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/openmpi/include

SOURCES += \
        main.cpp \
        models/richmaninfo.cpp \
        msg/msgreceiver.cpp \
        msg/msgsender.cpp \
        richman/msgdispatcher.cpp \
        richman/richman.cpp \
        richman/runnable.cpp \
        richman/tunnelwalker.cpp \
        tunnel/tunnel.cpp

HEADERS += \
    models/packet.h \
    models/place.h \
    models/reply.h \
    models/request.h \
    models/richmaninfo.h \
    msg/msgcomm.h \
    msg/msgreceiver.h \
    msg/msgsender.h \
    richman/msgdispatcher.h \
    richman/richman.h \
    richman/runnable.h \
    richman/tunnelwalker.h \
    tunnel/limiteddeque.h \
    tunnel/limitedset.h \
    tunnel/tunnel.h
