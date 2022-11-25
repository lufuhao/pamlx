//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

//#include <QtGui/QApplication>
#include <QtWidgets/QApplication>
//#include <QSplashScreen>
#include <QtWidgets/QSplashScreen>
#include "mainwindow.h"
#include "info.h"
#include "mydebug.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(APPLICATION_NAME);
    a.setApplicationVersion(APPLICATION_VERSION);
    a.setOrganizationName(ORGANIZATION_NAME);

    QSplashScreen* splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/splash.png"));
    splash->show();
    splash->showMessage(QObject::tr("Loading..."), Qt::AlignCenter | Qt::AlignBottom);

#ifdef MYDEBUG
    mydebugfp = fopen("mydebuglog", "wb");
#endif

    MainWindow w;
    w.show();

    qApp->processEvents();

    splash->finish(&w);
    delete splash;

    return a.exec();
}
