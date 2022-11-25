//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "chi2dialog.h"
#include "chi2criticalvaluesdialog.h"
#include <QCloseEvent>
#include <QDir>

Chi2Dialog::Chi2Dialog(const QString &pamlPath, QWidget *parent) :
    QDialog(parent),
    statusOk(false),
    chi2CriticalValuesDialog(NULL)
{
    setupUi(this);

#ifdef Q_OS_MAC
    QFont font;
    font.setPointSize(13);
    setFont(font);
#endif

    setWindowFlags(Qt::WindowMinimizeButtonHint);

    QObject::connect( &process, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadyReadStandardOutput()) );

    setPath(pamlPath);
}

Chi2Dialog::~Chi2Dialog()
{
    delete chi2CriticalValuesDialog;
    chi2CriticalValuesDialog = NULL;
}

void Chi2Dialog::closeEvent(QCloseEvent* event)
{
    delete chi2CriticalValuesDialog;
    chi2CriticalValuesDialog = NULL;

    event->accept();
}

bool Chi2Dialog::setPath(const QString& pamlPath)
{
#ifdef Q_OS_WIN32
    mPath = pamlPath + "/bin/chi2.exe";
#else
    mPath = pamlPath + "/bin/chi2";
#endif

    if( QFileInfo(mPath).isFile() )
        statusOk = true;
    else
        statusOk = false;

    pushButton_2->setEnabled(statusOk);

    return statusOk;
}

void Chi2Dialog::on_pushButton_2_clicked()
{
    int df = spinBox->value();
    double statistics = doubleSpinBox->value();

    QStringList arguments;
    arguments << QString::number(df) << QString::number(statistics);

    process.start(mPath, arguments);
}

void Chi2Dialog::on_pushButton_3_clicked()
{
    if(!chi2CriticalValuesDialog)
        chi2CriticalValuesDialog = new Chi2CriticalValuesDialog();

    chi2CriticalValuesDialog->show();

    if( chi2CriticalValuesDialog->isMinimized() )
        chi2CriticalValuesDialog->showNormal();
    if( !chi2CriticalValuesDialog->isActiveWindow() )
        chi2CriticalValuesDialog->activateWindow();
}

void Chi2Dialog::onReadyReadStandardOutput()
{
    QByteArray newData = process.readAllStandardOutput();
    QList<QByteArray> list = newData.split('=');
    QList<QByteArray>::const_iterator it = list.begin();

    it+=2;
    lineEdit_3->setText(*it);
    it++;
    lineEdit->setText(*it);
}
