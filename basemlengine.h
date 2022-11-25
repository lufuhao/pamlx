//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef BASEMLENGINE_H
#define BASEMLENGINE_H

#include "engine.h"

class BasemlEngine : public Engine
{
    Q_OBJECT

public:
    explicit BasemlEngine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent = 0);

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
//    int            nComb_noisy;//
    bool           bButt_cleandata;
//    int            nComb_verbose;//
    int            nComb_runmode;
    int            nComb_method;
    int            nComb_clock;
    double         nEdit_TipDate;
    double         nEdit_TipDate_TimeUnit;
//    bool           bButt_fix_rgene;
    int            nComb_Mgene;
    int            nComb_nhomo;
    bool           bButt_getSE;
    bool           bButt_RateAncestor;
    int            nComb_model;
    bool           bButt_fix_kappa;
    double         nEdit_kappa;
    bool           bButt_fix_alpha;
    double         nEdit_alpha;
    bool           bButt_Malpha;
    int            nEdit_ncatG;
    bool           bButt_fix_rho;
    double         nEdit_rho;
    int            nComb_nparK;
    int            nEdit_ndata;
//    bool           bButt_bootstrap;
    QString        strEdit_Small_Diff;//nEdit_Small_Diff
    int            nComb_icode;
    int            nComb_fix_blength;
//    int            nTab_seqtype;
};

#endif // MCMCTREEENGINE_H
