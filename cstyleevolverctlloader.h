#ifndef CSTYLEEVOLVERCTLLOADER_H
#define CSTYLEEVOLVERCTLLOADER_H

//----- C Style code for loading *.dat in paml:  paml.h tools.c evolver.c -----

#include <QString>

#define NS            7000
#define NBRANCH       (NS*2-2)
#define MAXNSONS      200
#define LSPNAME       50
#define NCODE         64
#define NCATG         40

class EvolverEngine;

class CStyleEvolverCtlLoader
{
public:
    CStyleEvolverCtlLoader(EvolverEngine* engine) : engine(engine) {}
    int GetOptions(char *ctlf);
    void error2(const char * message);

public:
    struct CommonInfo {
       unsigned char *z[2*NS-1];
       char spname[NS][LSPNAME+1], daafile[512], cleandata, readpattern;
       int ns, ls, npatt, np, ntime, ncode, clock, rooted, model, icode;
       int seqtype, *pose, ncatG, NSsites;
       double *fpatt, kappa, omega, alpha, pi[64], *conP, daa[20*20];
       double freqK[NCATG], rK[NCATG];
       char *siteID;    /* used if ncatG>1 */
       double *siterates;   /* rates for gamma or omega for site or branch-site models */
       double *omegaBS, *QfactorBS;     /* omega IDs for branch-site models */
    }  com;

    enum {BASEseq=0, CODONseq, AAseq, CODON2AAseq} DataTypes;
    enum {JC69, K80, F81, F84, HKY85, T92, TN93, REV} BaseModels;

    int noisy;
    int seed;
    int outFormat;
    int replicates;
    double treeLength;

    QString treeString;
    QString parameters;

private:
    EvolverEngine* engine;
};

#endif // CSTYLEEVOLVERCTLLOADER_H
