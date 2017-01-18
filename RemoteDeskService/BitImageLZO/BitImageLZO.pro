#-------------------------------------------------
#
# Project created by QtCreator 2016-12-08T13:47:48
#
#-------------------------------------------------

QT       -= core gui

TARGET = BitImageLZO
TEMPLATE = lib

INCLUDEPATH += /usr/java/jdk1.8.0_111/include

SOURCES += \
    testmini.c \
    minilzo.c \
    com_rouies_remotedesktop_decompress_LZODecompresor.cpp

HEADERS += \
    minilzo.h \
    lzodefs.h \
    lzoconf.h \
    com_rouies_remotedesktop_decompress_LZODecompresor.h

DESTDIR=/root/out

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
