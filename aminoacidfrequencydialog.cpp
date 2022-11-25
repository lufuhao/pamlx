//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "aminoacidfrequencydialog.h"

AminoAcidFrequencyDialog::AminoAcidFrequencyDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QTableWidgetItem* item;
    for(int i =0; i<20; i++) {
        item = tableWidget->item(i, 0);
        item->setFlags( item->flags() & ~Qt::ItemIsEditable );
    }
}
