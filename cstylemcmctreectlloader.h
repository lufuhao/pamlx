#ifndef CSTYLEMCMCTREECTLLOADER_H
#define CSTYLEMCMCTREECTLLOADER_H

//----- C Style code for loading mcmctree ctl file in paml:  paml.h tools.c mcmctree.c -----

#include <QString>

#define NS            800
#define NBRANCH      (NS*2-2)
#define NNODE        (NS*2-1)
#define MAXNSONS      3
#define NGENE         8000          /* used for gnodes[NGENE] */
#define NMORPHLOCI    10            /* used for com.zmorph[] */
#define LSPNAME       50
#define NCODE         64
#define NCATG         50
#define MaxNFossils   200

class MCMCTreeEngine;

class CStyleMCMCTreeCtlLoader
{
public:
    CStyleMCMCTreeCtlLoader(MCMCTreeEngine* engine) : engine(engine) {}
    int GetOptions(char *ctlf);
    void error2(const char * message);
    void warning(const char * message);

public:
    struct CommonInfo {
        unsigned char *z[NS];
        char *spname[NS], seqf[512],outf[512],treef[512],daafile[512],mcmcf[512],inBVf[512];
        char oldconP[NNODE];       /* update conP for node? (0 yes; 1 no) */
        int seqtype, ns, ls, ngene, posG[2],lgene[1], *pose, npatt, readpattern;
        int np, ncode, ntime, nrate, nrgene, nalpha, npi, ncatG, print;
        int cleandata, ndata;
        int model, clock, fix_kappa, fix_alpha, fix_rgene, Mgene;
        int method, icode, codonf, aaDist, NSsites;
        double *fpatt, kappa, alpha, TipDate, TipDate_TimeUnit;
        double rgene[NGENE],piG[NGENE][NCODE];  /* not used */
        double (*plfun)(double x[],int np), freqK[NCATG], rK[NCATG], *conP, *fhK;
        double pi[NCODE];
        int curconP;                    /* curconP = 0 or 1 */
        size_t sconP;
        double *conPin[2], space[100000];  /* change space[] to dynamic memory? */
        int conPSiteClass, NnodeScale;
        char *nodeScale;    /* nScale[ns-1] for interior nodes */
        double *nodeScaleF;       /* nScaleF[npatt] for scale factors */
    }  com;

    struct SPECIESTREE {
        int nbranch, nnode, root, nspecies, nfossil;
        double RootAge[4];
        struct TREESPN {
            char name[LSPNAME+1], fossil, usefossil;  /* fossil: 0, 1(L), 2(U), 3(B), 4(G) */
            int father, nson, sons[2];
            double age, pfossil[7];     /* parameters in fossil distribution */
            double *rates;              /* log rates for loci */
        } nodes[2*NS-1];
    }  sptree;
    /* all trees are binary & rooted, with ancestors unknown. */

    struct DATA { /* locus-specific data and tree information */
        int ns[NGENE], ls[NGENE], npatt[NGENE], ngene, nmorphloci, lgene[NGENE];
        int root[NGENE+1], conP_offset[NGENE];
        int priortime, priorrate;
        char datatype[NGENE], *z[NGENE][NS], cleandata[NGENE];
        double *zmorph[NMORPHLOCI][NS*2-1], *Rmorph[NMORPHLOCI];
        double *fpatt[NGENE], lnpT, lnpR, lnpDi[NGENE], pi[NGENE][NCODE];
        double kappa[NGENE], alpha[NGENE];
        double BDS[4];  /* parameters in the birth-death-sampling model */
        double kappagamma[2], alphagamma[2];
        double pfossilerror[3], /* (p_beta, q_beta, NminCorrect) */ Pfossilerr, *CcomFossilErr;
        int rgeneprior;         /* 0: conditional iid; 1: gamma-Dirichlet */
        double rgene[NGENE+1], sigma2[NGENE+1], rgenepara[3], sigma2para[3];
        double *blMLE[NGENE], *Gradient[NGENE], *Hessian[NGENE];
        int transform;
    }  data;

    struct MCMCPARAMETERS {
       int resetFinetune, burnin, nsample, sampfreq, usedata, saveconP, print;
       double finetune[6];
    }  mcmc; /* control parameters */
//    struct MCMCPARAMETERS {
//        int burnin, nsample, sampfreq, usedata, saveconP, print;
//        int nfinetune, finetuneOffset[8];
//        char  *accept;
//        double *finetune, *Pjump;
//    }  mcmc; /* control parameters */

    enum {SQRT_B=1, LOG_B, ARCSIN_B} B_Transforms;

    int seed;
    int noisy;

    QString seqfile;
    QString treefile;
    QString outfile;
    QString RootAge;

private:
    MCMCTreeEngine* engine;
};

#endif // CSTYLEMCMCTREECTLLOADER_H
