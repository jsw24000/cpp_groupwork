QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addpostdialog.cpp \
    client.cpp \
    comment.cpp \
    main.cpp \
    mainwindow.cpp \
    post.cpp \
    postwidget.cpp \
    reply.cpp

HEADERS += \
    addpostdialog.h \
    client.h \
    comment.h \
    mainwindow.h \
    post.h \
    postwidget.h \
    reply.h

FORMS += \
    addpostdialog.ui \
    comment.ui \
    mainwindow.ui \
    postwidget.ui \
    reply.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
