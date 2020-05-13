QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_FILE = appIcon.rc

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/connection.cpp \
    src/database.cpp \
    src/mainwindow.cpp \
    src/map.cpp \
    src/packet.cpp \
    src/qr.cpp \
    src/sql_code.cpp \
    main.cpp

HEADERS += \
    include/connection.h \
    include/database.h \
    include/mainwindow.h \
    include/map.h \
    include/packet.h \
    include/qr.h \
    include/sql_code.h \


FORMS += \
    src/ui/connection.ui \
    src/ui/database.ui \
    src/ui/mainwindow.ui \
    src/ui/map.ui \
    src/ui/sql_code.ui

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/../shared/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
