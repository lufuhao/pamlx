//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef AMINOACIDFREQUENCYDIALOG_H
#define AMINOACIDFREQUENCYDIALOG_H

#include "ui_aminoacidfrequencydialog.h"

class AminoAcidFrequencyDialog : public QDialog, public Ui::AminoAcidFrequencyDialog
{
    Q_OBJECT

public:
    explicit AminoAcidFrequencyDialog(QWidget *parent = 0);
};

#endif // AMINOACIDFREQUENCYDIALOG_H
