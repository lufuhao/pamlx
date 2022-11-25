//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef CODONFREQUENCYDIALOG_H
#define CODONFREQUENCYDIALOG_H

#include "ui_codonfrequencydialog.h"

class CodonFrequencyDialog : public QDialog, public Ui::CodonFrequencyDialog
{
    Q_OBJECT

public:
    explicit CodonFrequencyDialog(QWidget *parent = 0);
};

#endif // CODONFREQUENCYDIALOG_H
