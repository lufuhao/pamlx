//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef CHI2DIALOG_H
#define CHI2DIALOG_H

#include "ui_chi2dialog.h"
#include <QProcess>

class Chi2CriticalValuesDialog;

class Chi2Dialog : public QDialog, private Ui::Chi2Dialog
{
    Q_OBJECT

public:
    explicit Chi2Dialog(const QString &pamlPath, QWidget *parent = 0);
    virtual ~Chi2Dialog();

protected:
void closeEvent(QCloseEvent* event);

public:
    bool setPath(const QString& pamlPath);
    const QString& path() const { return mPath; }
    bool isStatusOk() const { return statusOk; }

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void onReadyReadStandardOutput();

private:
    QString     mPath;
    bool        statusOk;
    QProcess    process;
    Chi2CriticalValuesDialog* chi2CriticalValuesDialog;
};

#endif // CHI2DIALOG_H
