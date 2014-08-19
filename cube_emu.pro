# add 'CONFIG+=debug' for debugging

QT += quick qml
HEADERS = GUIMain.h Cube.h CubeControl.h IO.h
SOURCES = GUIMain.cc Cube.cc CubeControl.cc IO.cc

QMAKE_CXXFLAGS *= -std=c++11
