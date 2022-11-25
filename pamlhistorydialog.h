//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef PAMLHISTORYDIALOG_H
#define PAMLHISTORYDIALOG_H

#include "ui_pamlhistorydialog.h"

class PAMLHistoryDialog : public QDialog, private Ui::PAMLHistoryDialog
{
    Q_OBJECT

public:
    explicit PAMLHistoryDialog(const QString &pamlHistory_FileName, const QFont &outputFont = QFont(), QWidget *parent = 0);
};

#endif // PAMLHISTORYDIALOG_H
