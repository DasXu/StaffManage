#-------------------------------------------------
#
# Project created by QtCreator 2023-08-28T12:02:28
#
#-------------------------------------------------

QT       += core gui  sql
QT       += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StaffTest
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

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    login.cpp \
    addnew.cpp \
    changeuser.cpp \
    prutype.cpp \
    detial.cpp \
    rules.cpp \
    ruleedit.cpp \
    ruleedit2.cpp \
    postscore.cpp \
    structure.cpp

HEADERS += \
        mainwindow.h \
    login.h \
    addnew.h \
    changeuser.h \
    prutype.h \
    detial.h \
    rules.h \
    ruleedit.h \
    ruleedit2.h \
    postscore.h \
    structure.h

FORMS += \
        mainwindow.ui \
    login.ui \
    addnew.ui \
    changeuser.ui \
    prutype.ui \
    detial.ui \
    rules.ui \
    ruleedit.ui \
    ruleedit2.ui \
    postscore.ui \
    structure.ui

LIBS += $$PWD/libxl/lib/libxl.lib
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    picture.qrc

