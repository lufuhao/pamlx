//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include "ui_opendialog.h"

#include "mainwindow.h"

class OpenDialog : public QDialog, private Ui::OpenDialog
{
    Q_OBJECT

public:
    explicit OpenDialog(MainWindow::ControlFileType &aType, QWidget *parent = 0);

public slots:
    void accept();

private slots:
    void on_listWidget_currentRowChanged(int currentRow);
    void on_listWidget_2_currentRowChanged(int currentRow);

private:
    MainWindow::ControlFileType& type;
};

#endif // OPENDIALOG_H
