//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef PAMPENGINE_H
#define PAMPENGINE_H

#include "engine.h"

class PampEngine : public Engine
{
    Q_OBJECT

public:
    explicit PampEngine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent = 0);

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
    QString        strEdit_treefile;
    QString        strEdit_outfile_Name;
    QString        strEdit_outfile_Location;
    int            nComb_seqtype;
    int            nEdit_ncatG;
    int            nComb_nhomo;
};

#endif // PAMPENGINE_H
