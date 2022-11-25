//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "opendialog.h"

OpenDialog::OpenDialog(MainWindow::ControlFileType &aType, QWidget *parent) :
    QDialog(parent),
    type(aType)
{
    setupUi(this);

#if defined(Q_OS_MAC)
    QFont font;
    font.setPointSize(13);
    setFont(font);
    resize(520, 242);
#elif defined(Q_OS_LINUX)
    resize(410, 200);
#endif

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled( listWidget->currentRow() != -1 || listWidget_2->currentRow() != -1 );

    int row = int(type);
    if(row >= -1 && row < 5) {
        listWidget->setCurrentRow(row);
        if(row != -1)
            listWidget->setFocus();
    }
    if(row == -1 || row >= 5) {
        if(row != -1) row -= 5;
        listWidget_2->setCurrentRow(row);
        if(row != -1)
            listWidget_2->setFocus();
    }
}

void OpenDialog::accept()
{
    QDialog::accept();

    int row = listWidget->currentRow();
    if(row == -1) {
        row = listWidget_2->currentRow();
        if(row != -1) row += 5;
    }

    type = MainWindow::ControlFileType(row);
}

void OpenDialog::on_listWidget_currentRowChanged(int currentRow)
{
    if(currentRow != -1)
        listWidget_2->setCurrentRow(-1);

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled( currentRow != -1 || listWidget_2->currentRow() != -1 );
}

void OpenDialog::on_listWidget_2_currentRowChanged(int currentRow)
{
    if(currentRow != -1)
        listWidget->setCurrentRow(-1);

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled( currentRow != -1 || listWidget->currentRow() != -1 );
}
