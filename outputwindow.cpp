//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "outputwindow.h"
#include <QCloseEvent>

#define MAX_BLOCK_COUNT 1000

OutputWindow::OutputWindow(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    textEdit->document()->setMaximumBlockCount(MAX_BLOCK_COUNT);
}

OutputWindow::~OutputWindow()
{
    emit closed();
    emit deleted();
}

void OutputWindow::closeEvent(QCloseEvent* event)
{
    emit closed();
    event->accept();
}
