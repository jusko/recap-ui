HEADERS += \
    src/captureform.h \
    src/taglineedit.h \
    src/recallview.h \
    src/itemmodel.h \
    src/qtserializerwrapper.h \
    src/recapapp.h

SOURCES += \
    src/captureform.cpp \
    src/main.cpp \
    src/taglineedit.cpp \
    src/recallview.cpp \
    src/itemmodel.cpp \
    src/qtserializerwrapper.cpp \
    src/recapapp.cpp

INCLUDEPATH += include/ src/

OTHER_FILES += TODO

LIBS += -Llib/ -lrecapcore

TARGET = recap-ui-qt
