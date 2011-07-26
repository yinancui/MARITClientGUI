#-------------------------------------------------
#
# Project created by QtCreator 2011-07-09T00:02:20
#
#-------------------------------------------------

QT       += core gui
QT       += network

TARGET = ClientGUI
TEMPLATE = app


SOURCES += main.cpp\
        client.cpp \
    errorcode.cpp \
    port.cpp \
    Data.cpp

HEADERS  += client.h \
    errorcode.h \
    port.h \
    Data.h

FORMS    += client.ui


#LIBS    += -L/usr/local/lib/vtk-5.6 -lvtkCommon -lvtksys -lQVTK -lvtkQtChart -lvtkViews -lvtkWidgets -lvtkInfovis -lvtkRendering -lvtkGraphics -lvtkImaging -lvtkIO -lvtkFiltering -lvtklibxml2 -lvtkDICOMParser -lvtkpng -lvtkpng -lvtktiff -lvtkzlib -lvtkjpeg -lvtkalglib -lvtkexpat -lvtkverdict -lvtkmetaio -lvtkNetCDF -lvtksqlite -lvtkexoIIc -lvtkftgl -lvtkfreetype -lvtkHybrid

#INCLUDEPATH += /usr/local/include/vtk-5.6
