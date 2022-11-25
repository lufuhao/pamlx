//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "chi2criticalvaluesdialog.h"
#include <QFile>
#include <QTextStream>

Chi2CriticalValuesDialog::Chi2CriticalValuesDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

#ifdef Q_OS_MAC
    QFont font;
    font.setPointSize(13);
    pushButton->setFont(font);
#endif

    setWindowFlags(Qt::WindowMinimizeButtonHint);

    QFile chi2CriticalValues_File(":/chi2criticalvalues.txt");
    chi2CriticalValues_File.open(QIODevice::ReadOnly);

    textEdit->setText( QTextStream(&chi2CriticalValues_File).readAll() );
}
