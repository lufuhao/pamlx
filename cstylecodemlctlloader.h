#ifndef CSTYLECODEMLCTLLOADER_H
#define CSTYLECODEMLCTLLOADER_H

//----- C Style code for loading codeml ctl file in paml:  paml.h tools.c codeml.c -----

#include <QString>

#define NS            7000
#define NBRANCH       (NS*2-2)
#define NNODE         (NS*2-1)
#define MAXNSONS      100
#define NGENE         2000
#define LSPNAME       50
#define NCODE         64
#define NCATG         40
#define NBTYPE        17

class CodemlEngine;

class CStyleCodemlCtlLoader
{
public:
    CStyleCodemlCtlLoader(CodemlEngine* engine) : engine(engine) {}
    int GetOptions(char *ctlf);
    void error2(const char * message);
    int ReadNSsitesModels(char *line);

public:
    struct common_info {
       unsigned char *z[NS];
       char *spname[NS], seqf[512],outf[512],treef[512],daafile[512], cleandata;
       char oldconP[NNODE];       /* update conP for nodes? to save computation */
       int seqtype, ns, ls, ngene, posG[NGENE+1], lgene[NGENE], npatt,*pose, readpattern;
       int runmode,clock, verbose,print, codonf,aaDist,model,NSsites;
       int nOmega, nbtype, nOmegaType;  /* branch partition, AA pair (w) partition */
       int method, icode, ncode, Mgene, ndata, bootstrap;
       int fix_rgene,fix_kappa,fix_omega,fix_alpha,fix_rho,nparK,fix_blength,getSE;
       int np, ntime, nrgene, nkappa, npi, nrate, nalpha, ncatG, hkyREV;
       size_t sconP, sspace;
       double *fpatt, *space, kappa,omega,alpha,rho,rgene[NGENE], TipDate, TipDate_TimeUnit;
       double pi[NCODE], piG[NGENE][64], fb61[64];
       double f3x4[NGENE][12], *pf3x4, piAA[20];
       double freqK[NCATG], rK[NCATG], MK[NCATG*NCATG],daa[20*20], *conP, *fhK;
       double (*plfun)(double x[],int np);
       double hyperpar[4]; /* kostas, the hyperparameters for the prior distribution of distance & omega */
       double omega_fix;  /* fix the last w in the NSbranchB, NSbranch2 models
              for lineages.  Useful for testing whether w>1 for some lineages. */
       int     conPSiteClass; /* conPSiteClass=0 if (method==0) and =1 if (method==1)?? */
       int     NnodeScale;
       char   *nodeScale;        /* nScale[ns-1] for interior nodes */
       double *nodeScaleF;       /* nScaleF[npatt] for scale factors */
      /* pomega & pkappa are used to communicate between SetParameters & ConditionalPNode
         & eigenQcodon.  Try to remove them? */
       double *pomega, pkappa[5], *ppi;
    }  com;

    struct DATA { /* locus-specific data and tree information */
       int ns[NGENE], ls[NGENE], npatt[NGENE], ngene, lgene[NGENE];
       int root[NGENE+1], BlengthMethod, fix_nu, nbrate[NGENE], icode[NGENE];
       int datatype[1];
       char   *z[NGENE][NS], cleandata[NGENE];
       char   idaafile[NGENE], daafile[NGENE][40];
       double *fpatt[NGENE], lnpT, lnpR, lnpDi[NGENE];
       double Qfactor[NGENE], pi[NGENE][NCODE];
       double rgene[NGENE], kappa[NGENE], alpha[NGENE], omega[NGENE];
       int NnodeScale[NGENE];
       char *nodeScale[NGENE];    /* nScale[data.ns[locus]-1] for interior nodes */
    }  data;

    static const int maxNSsitesModels = 27;

    /* variables for batch run of site models */
    int ncatG0, insmodel, nnsmodels, nsmodels[15];

    double Small_Diff;//=1e-6;  /* reasonable values 1e-5, 1e-7 */
    int noisy;

    QString seqfile;
    QString treefile;
    QString outfile;
    QString aaRatefile;

private:
    CodemlEngine* engine;
};

#endif // CSTYLECODEMLCTLLOADER_H
