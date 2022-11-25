//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef MCMCTREEENGINE_H
#define MCMCTREEENGINE_H

#include "engine.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_MAC) || defined(Q_OS_LINUX)
#define STREAMLINED_OUTPUT
#endif

class MCMCTreeEngine : public Engine
{
    Q_OBJECT

public:
    explicit MCMCTreeEngine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent = 0);

public:
    bool setPath(const QString& pamlPath);
    void refreshFileContent();
    const QString& version() const { return mVersion; }
    int compareVersion(const QString& versionStr) const;

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
#ifdef STREAMLINED_OUTPUT
    void onReadyReadStandardOutput();
#endif

public:
    int            nEdit_seed;
    QString        strEdit_seqfile;
    QString        strEdit_treefile;
    QString        strEdit_outfile_Name;
    QString        strEdit_outfile_Location;
//    QString        strEdit_mcmcfile;
//    int            nComb_seqtype;
//    QString        strEdit_aaRatefile;
//    int            nComb_icode;
//    int            nComb_noisy;
    int            nComb_usedata;
    int            nEdit_ndata;
    int            nComb_model;
    int            nComb_clock;
    double         nEdit_TipDate;
    double         nEdit_TipDate_TimeUnit;
    QString        strEdit_RootAge;
//    double         nEdit_fossilerror[3];
    double         nEdit_alpha;
    int            nEdit_ncatG;
    bool           bButt_cleandata;
    double         nEdit_BDparas[4];
    double         nEdit_kappa_gamma[2];
    double         nEdit_alpha_gamma[2];
    int            nComb_rgeneprior;
    double         nEdit_rgene_para[3];
    double         nEdit_sigma2_para[3];
    bool           bButt_print;
    int            nEdit_burnin;
    int            nEdit_sampfreq;
    int            nEdit_nsample;
    bool           bButt_autoAdjustFinetune;
    double         nEdit_finetune[5/*6*/];

private:
#ifdef STREAMLINED_OUTPUT
    int streamlineFlag;
#endif
    QString mVersion;
};

#endif // MCMCTREEENGINE_H
