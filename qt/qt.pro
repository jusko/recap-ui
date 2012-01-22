HEADERS += \
    captureform.h \
    taglineedit.h \
    tagshortcutdialog.h \
    recallview.h \
    globals.h \
    itemmodel.h \
    qtserializerwrapper.h

SOURCES += \
    captureform.cpp \
    main.cpp \
    taglineedit.cpp \
    tagshortcutdialog.cpp \
    recallview.cpp \
    itemmodel.cpp \
    qtserializerwrapper.cpp

INCLUDEPATH += ../../core/

OTHER_FILES += ../../../TODO

LIBS += -L../../core/ -lrecapcore
