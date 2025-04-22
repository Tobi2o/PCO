#message("Building student project")
include(../QtrainSim/QtrainSim.pri)

CONFIG += c++17

LIBS += -lpcosynchro

HEADERS +=  \
    src/locomotive.h \
    src/launchable.h \
    src/locomotivebehavior.h \
    src/synchro.h \
    src/synchrointerface.h \
    src/trainTrack.h

SOURCES +=  \
    src/locomotive.cpp \
    src/cppmain.cpp \
    src/locomotivebehavior.cpp \
    src/synchro.cpp \
    src/trainTrack.cpp
