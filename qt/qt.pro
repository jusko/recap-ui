HEADERS += \
    src/captureform.h \
    src/taglineedit.h \
    src/recallview.h \
    src/itemmodel.h \
    src/qtserializerwrapper.h \
    src/recapapp.h \
    src/itemsortfilterproxymodel.h \
    src/cryptomediator.h \
    src/recallpage.h

SOURCES += \
    src/captureform.cpp \
    src/main.cpp \
    src/taglineedit.cpp \
    src/recallview.cpp \
    src/itemmodel.cpp \
    src/qtserializerwrapper.cpp \
    src/recapapp.cpp \
    src/itemsortfilterproxymodel.cpp \
    src/cryptomediator.cpp \
    src/recallpage.cpp

INCLUDEPATH += include/ src/

OTHER_FILES += TODO

LIBS += -Llib/ -lrecapcore

TARGET = recap-ui-qt

RESOURCES += \
    res/resources.qrc
