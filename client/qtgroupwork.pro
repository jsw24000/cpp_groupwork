# QT       += core gui
QT += core gui sql
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addpostdialog.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    postwidget.cpp \
    selfcenterwindow.cpp \
    signup.cpp \
    client.cpp \
    comment.cpp \
    post.cpp \
    reply.cpp

HEADERS += \
    addpostdialog.h \
    login.h \
    mainwindow.h \
    postwidget.h \
    selfcenterwindow.h \
    signup.h \
    client.h \
    comment.h \
    post.h \
    reply.h

FORMS += \
    addpostdialog.ui \
    login.ui \
    mainwindow.ui \
    postwidget.ui \
    selfcenterwindow.ui \
    signup.ui \
    comment.ui \
    reply.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
