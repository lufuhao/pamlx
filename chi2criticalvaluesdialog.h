//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef CHI2CRITICALVALUESDIALOG_H
#define CHI2CRITICALVALUESDIALOG_H

#include "ui_chi2criticalvaluesdialog.h"

class Chi2CriticalValuesDialog : public QDialog, private Ui::Chi2CriticalValuesDialog
{
    Q_OBJECT

public:
    explicit Chi2CriticalValuesDialog(QWidget *parent = 0);
};

#endif // CHI2CRITICALVALUESDIALOG_H
