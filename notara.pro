QT +=  core widgets quick
HEADERS =   mainwindow.h \
    pdfview.h \
    pdfpage.h \
    pdfimageprovider.h \
    pdfpagemodel.h
SOURCES =   main.cpp \
    mainwindow.cpp \
    pdfview.cpp \
    pdfpage.cpp \
    pdfimageprovider.cpp \
    pdfpagemodel.cpp

QMAKE_CXXFLAGS += -std=c++11
INCLUDEPATH += /usr/include/poppler/qt5
LIBS        += -L/usr/lib -lpoppler-qt5

RESOURCES += \
    notara.qrc

OTHER_FILES +=
