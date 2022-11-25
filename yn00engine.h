//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef YN00ENGINE_H
#define YN00ENGINE_H

#include "engine.h"

class YN00Engine : public Engine
{
    Q_OBJECT

public:
    explicit YN00Engine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent = 0);

public:
    bool setPath(const QString& pamlPath);
    void refreshFileContent();

private:
    void setDefaultData();
    bool readFile(const QString& fileName);
    bool writeFile(const QString& fileName);
    bool createTmpCtlFile(const QString& fileName);

public:
    bool onNewDocument();
    bool onCloseDocument();

protected:
    bool doOpen(const QString& fileName);
    bool doSave(const QString& fileName);

public slots:
    bool run();

public:
    QString        strEdit_seqfile;
    QString        strEdit_outfile_Name;
    QString        strEdit_outfile_Location;
    int            nComb_verbose;
    int            nComb_noisy;
    int            nComb_icode;
    bool           bButt_weighting;
    bool           bButt_commonkappa;
    bool           bButt_commonf3x4;
    int            nEdit_ndata;
};

#endif // YN00ENGINE_H
