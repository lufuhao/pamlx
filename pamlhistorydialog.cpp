//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "pamlhistorydialog.h"
#include <QDir>
#include <QTextStream>
#include <QtWidgets/QMessageBox>

PAMLHistoryDialog::PAMLHistoryDialog(const QString &pamlHistory_FileName, const QFont &outputFont, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QString pamlHistory;
    QFile pamlHistory_File(pamlHistory_FileName);
    if( pamlHistory_File.open(QIODevice::ReadOnly) ) {
        QTextStream pamlHistoryIn(&pamlHistory_File);
        pamlHistory = pamlHistoryIn.readAll();

        QString end_of_file = "// end of file";
        int position = pamlHistory.lastIndexOf(end_of_file);
        if(position != -1)
            pamlHistory.remove(position, pamlHistory.size());
    }
    else
        pamlHistory = tr("ERROR when open: %1").arg(QDir::toNativeSeparators(pamlHistory_FileName));

    textEdit->setCurrentFont(outputFont);
    textEdit->setText(pamlHistory);
}
