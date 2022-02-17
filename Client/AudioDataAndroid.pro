QT       += core gui
QT       += androidextras
QT       += core-private
QT       += multimedia
QT       += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    RecoderAndPlayer/recoderandplayer.cpp \
    RecoderAndPlayer/threadrecoder.cpp \
    RecoderAndPlayer/threadrecodertime.cpp \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    RecoderAndPlayer/recoderandplayer.h \
    RecoderAndPlayer/threadrecoder.h \
    RecoderAndPlayer/threadrecodertime.h \
    mainwindow.h \


FORMS += \
    RecoderAndPlayer/recoderandplayer.ui \
    mainwindow.ui \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
