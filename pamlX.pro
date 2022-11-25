#-------------------------------------------------
#
# Project created by QtCreator 2012-03-31T22:51:33
#
# Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
#
#-------------------------------------------------

QT       += core gui

TARGET = pamlX
TEMPLATE = app

DEFINES += QT_NO_CAST_TO_ASCII

SOURCES += main.cpp\
    mainwindow.cpp \
    document.cpp \
    engine.cpp \
    mcmctreeengine.cpp \
    basemlengine.cpp \
    codemlengine.cpp \
    pampengine.cpp \
    yn00engine.cpp \
    evolverengine.cpp \
    cstylemcmctreectlloader.cpp \
    cstylebasemlctlloader.cpp \
    cstylecodemlctlloader.cpp \
    cstylepampctlloader.cpp \
    cstyleyn00ctlloader.cpp \
    cstyleevolverctlloader.cpp \
    chi2dialog.cpp \
    chi2criticalvaluesdialog.cpp \
    codonfrequencydialog.cpp \
    aminoacidfrequencydialog.cpp \
    opendialog.cpp \
    configurationdialog.cpp \
    pamlhistorydialog.cpp \
    outputwindow.cpp \
    mydebug.cpp

HEADERS  += mainwindow.h \
    def.h \
    info.h \
    document.h \
    engine.h \
    mcmctreeengine.h \
    basemlengine.h \
    codemlengine.h \
    pampengine.h \
    yn00engine.h \
    evolverengine.h \
    cstylemcmctreectlloader.h \
    cstylebasemlctlloader.h \
    cstylecodemlctlloader.h \
    cstylepampctlloader.h \
    cstyleyn00ctlloader.h \
    cstyleevolverctlloader.h \
    chi2dialog.h \
    chi2criticalvaluesdialog.h \
    codonfrequencydialog.h \
    aminoacidfrequencydialog.h \
    opendialog.h \
    configurationdialog.h \
    pamlhistorydialog.h \
    outputwindow.h \
    mydebug.h

FORMS    += mainwindow.ui \
    chi2dialog.ui \
    chi2criticalvaluesdialog.ui \
    codonfrequencydialog.ui \
    aminoacidfrequencydialog.ui \
    opendialog.ui \
    configurationdialog.ui \
    pamlhistorydialog.ui \
    outputwindow.ui

RESOURCES += \
    pamlxresources.qrc

win32 {
    RC_FILE = pamlxinfo-win32.rc
}

mac {
    ICON = pamlxlogo-mac.icns
    QMAKE_INFO_PLIST = pamlxinfo-mac.plist
}
