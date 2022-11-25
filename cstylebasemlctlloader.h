#ifndef CSTYLEBASEMLCTLLOADER_H
#define CSTYLEBASEMLCTLLOADER_H

//----- C Style code for loading baseml ctl file in paml:  paml.h tools.c baseml.c -----

#include <QString>

#define NS            7000
#define NBRANCH       (NS*2-2)
#define NNODE         (NS*2-1)
#define MAXNSONS      200
#define NGENE         500
#define LSPNAME       50
#define NCODE         5
#define NCATG         100

class BasemlEngine;

class CStyleBasemlCtlLoader
{
public:
    CStyleBasemlCtlLoader(BasemlEngine* engine) : engine(engine) {}
    int GetOptions(char *ctlf);
    void error2(const char * message);

public:
    struct CommonInfo {
       unsigned char *z[NS];
       char *spname[NS], seqf[512],outf[512],treef[512], cleandata;
       char oldconP[NNODE];  /* update conP for nodes to save computation (0 yes; 1 no) */
       int seqtype, ns, ls, ngene, posG[NGENE+1], lgene[NGENE], *pose, npatt, readpattern;
       int np, ntime, nrgene, nrate, nalpha, npi, nhomo, ncatG, ncode, Mgene;
       size_t sspace, sconP;
       int fix_kappa, fix_rgene, fix_alpha, fix_rho, nparK, fix_blength;
       int clock, model, getSE, runmode, print,verbose, ndata, bootstrap;
       int icode, coding, method;
       int nbtype;
       double *fpatt, kappa, alpha, rho, rgene[NGENE], pi[4],piG[NGENE][4], TipDate, TipDate_TimeUnit;
       double freqK[NCATG], rK[NCATG], MK[NCATG*NCATG];
       double (*plfun)(double x[],int np), *conP, *fhK, *space;
       int conPSiteClass;        /* is conP memory allocated for each class? */
       int NnodeScale;
       char *nodeScale;          /* nScale[ns-1] for interior nodes */
       double *nodeScaleF;       /* nScaleF[npatt] for scale factors */
       int fix_omega;
       double omega;
    }  com;

    struct DATA { /* locus-specific data and tree information */
       int ns[NGENE], ls[NGENE], npatt[NGENE], ngene, lgene[NGENE];
       int root[NGENE+1], BlengthMethod, fix_nu, nbrate[NGENE];
       char *z[NGENE][NS], cleandata[NGENE];
       double *fpatt[NGENE], lnpT, lnpR, lnpDi[NGENE];
       double Qfactor[NGENE], pi[NGENE][NCODE], nu[NGENE];
       double rgene[NGENE], kappa[NGENE], alpha[NGENE], omega[1];
       int NnodeScale[NGENE];
       char *nodeScale[NGENE];    /* nScale[data.ns[locus]-1] for interior nodes */
    }  data;

    double Small_Diff;//=1e-6;  /* reasonable values 1e-5, 1e-7 */
    int noisy;

    QString seqfile;
    QString treefile;
    QString outfile;

private:
    BasemlEngine* engine;
};

#endif // CSTYLEBASEMLCTLLOADER_H
