TEMPLATE	= app
LANGUAGE	= C++

QT     +=  gui

CONFIG +=  qt
CONFIG +=  warn_on link_pkgconfig
CONFIG +=  c++1z
CONFIG +=  console
CONFIG -=  app_bundle

DEFINES += QT_DEPRECATED_WARNINGS


#SOURCES += $$files(rest_server.cpp, true)
#SOURCES += $$files(requesthandler.cpp, true)
#SOURCES += $$files(requesthandler.hpp, true)

HEADERS += $$files(*.hpp, true)
SOURCES += $$files(*.cpp, true)



INCLUDEPATH += $$PWD/src
INCLUDEPATH += /home/pat/Source/GitHub/pistache/build/src


LIBS += -lpistache
LIBS += -pthread
LIBS +=  -lstdc++

#  fftw-3.3.8
packagesExist(fftw3) {
    PKGCONFIG += fftw3
    DEFINES += FFTWPROGS
}

gcc|clang {
    QMAKE_CXXFLAGS += -ffast-math -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

linux-clang {
    system(clang++ --version | grep 'version.3.[0-8]') {
        DEFINES += _OPENMP=201107
        QMAKE_CXXFLAGS += -Wno-macro-redefined
    }
}

unix {
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

msvc {
    DEFINES += _USE_MATH_DEFINES
    QMAKE_CXXFLAGS += /fp:fast /openmp
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

message(****  symart_server.pro  ****)
message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Qt resources can be found in the following locations:)
message(Documentation: $$[QT_INSTALL_DOCS])
message(Header files: $$[QT_INSTALL_HEADERS])
message(Libraries: $$[QT_INSTALL_LIBS])
message(Binary files (executables): $$[QT_INSTALL_BINS])
message(Plugins: $$[QT_INSTALL_PLUGINS])
message(Data files: $$[QT_INSTALL_DATA])
message(Translation files: $$[QT_INSTALL_TRANSLATIONS])
message(Settings: $$[QT_INSTALL_CONFIGURATION])
message(PWD = $$PWD)
message(INCLUDEPATH = $$INCLUDEPATH)
