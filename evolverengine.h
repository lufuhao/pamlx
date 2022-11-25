//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef EVOLVERENGINE_H
#define EVOLVERENGINE_H

#include "engine.h"

class EvolverEngine : public Engine
{
    Q_OBJECT

public:
    enum Type { Nucleotide = 0, Codon, AminoAcid };

public:
    explicit EvolverEngine(const QString &pamlPath, EvolverEngine::Type type, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent = 0);

public:
    bool setPath(const QString& pamlPath);
    void refreshFileContent();
    Type fileType() const { return mFileType; }
    void setFileType(Type type);

private:
    void setDefaultData(Type type);
    bool readFile(const QString& fileName, Type type);
    bool writeFile(const QString& fileName, Type type);
    bool createTmpCtlFile(const QString& fileName, Type type);

public:
    bool onNewDocument();
    bool onCloseDocument();

protected:
    bool doOpen(const QString& fileName);
    bool doSave(const QString& fileName);

public slots:
    bool run();

public:
    QString        strEdit_Output_Location;
    int            nComb_outFormat;
    int            nEdit_seed;
    int            nEdit_seqs;
    int            nEdit_sites;
    int            nEdit_replicates;
    double         nEdit_treeLength;
    QString        strEdit_tree;

    int            nComb_model;
    QString        strEdit_parameters;
    QString        strEdit_aminoAcidSbstRtFile;
    double         nEdit_alpha;
    int            nEdit_ncatG;
    double         nEdit_omega;
    double         nEdit_kappa;
    double         nEdit_baseFrequencies[4];
    double         nEdit_codonFrequencies[64];
    double         nEdit_aminoAcidFrequencies[20];

private:
    Type mFileType;
};

#endif // MCMCTREEENGINE_H
