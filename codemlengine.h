//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef CODEMLENGINE_H
#define CODEMLENGINE_H

#include "engine.h"

class CodemlEngine : public Engine
{
    Q_OBJECT

public:
    explicit CodemlEngine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent = 0);

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
    enum Trilean { Null = -1, False, True };
    enum { maxNSsitesModels = 27 };

public:
    QString        strEdit_seqfile;
    QString        strEdit_treefile;
    QString        strEdit_outfile_Name;
    QString        strEdit_outfile_Location;
    int            nComb_seqtype;
//    int            nComb_noisy;//
    bool           bButt_cleandata;
    int            nComb_runmode;
    double         nEdit_t_gamma[2];
    double         nEdit_w_gamma[2];
    int            nComb_method;
    int            nComb_clock;
//                   TipDate
    bool           bButt_getSE;
    bool           bButt_RateAncestor;
    int            nComb_CodonFreq;
    int            nComb_estFreq;
//    int            nComb_verbose;//
    int            nComb_model;
//                   hkyREV
    int            nComb_aaDist;
    QString        strEdit_aaRatefile;
    Trilean        triList_nsmodels[maxNSsitesModels];
//                   NShmm
    int            nComb_icode;
    int            nComb_Mgene;
    bool           bButt_fix_kappa;
    double         nEdit_kappa;
    bool           bButt_fix_omega;
    double         nEdit_omega;
    bool           bButt_fix_alpha;
    double         nEdit_alpha;
    bool           bButt_Malpha;
    int            nEdit_ncatG;
//    bool           bButt_fix_rho;
//    double         nEdit_rho;
    int            nEdit_ndata;
//    bool           bButt_bootstrap;
    QString        strEdit_Small_Diff;//nEdit_Small_Diff
    int            nComb_fix_blength;
};

#endif // MCMCTREEENGINE_H
