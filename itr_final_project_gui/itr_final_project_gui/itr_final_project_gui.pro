QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += QT_NO_DEBUG_OUTPUT

INCLUDEPATH += \
    app/app_config \
    app/mainwindow \
    app/uart \
    middlewares/qcustomplot \

SOURCES += \
    app/mainwindow/mainwindow.cpp \
    app/mainwindow/mainwindow_button.cpp \
    app/uart/uart.cpp \
    middlewares/qcustomplot/qcustomplot.cpp \
    main.cpp \

# SOURCES += \
#     main.cpp \
#     mainwindow.cpp

HEADERS += \
    app/app_config/app_config.h \
    app/mainwindow/mainwindow.h \
    app/uart/uart.h \
    middlewares/qcustomplot/qcustomplot.h

# HEADERS += \
#     mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
