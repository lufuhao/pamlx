//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef OUTPUTWINDOW_H
#define OUTPUTWINDOW_H

#include "ui_outputwindow.h"

class OutputWindow : public QWidget, public Ui::OutputWindow
{
    Q_OBJECT

public:
    explicit OutputWindow(QWidget *parent = 0);
    virtual ~OutputWindow();

protected:
    void closeEvent(QCloseEvent* event);

signals:
    void closed();
    void deleted();
};

#endif // OUTPUTWINDOW_H
