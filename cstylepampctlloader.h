#ifndef CSTYLEPAMPCTLLOADER_H
#define CSTYLEPAMPCTLLOADER_H

//----- C Style code for loading baseml ctl file in paml:  yn00.h -----

#include <QString>

class PampEngine;

class CStylePampCtlLoader
{
public:
    CStylePampCtlLoader(PampEngine* engine) : engine(engine) {}
    int GetOptions(char *ctlf);
    void error2(const char * message);

public:
    struct CommonInfo {
//       char *z[NS], *spname[NS], seqf[256],outf[256],treef[256];
       int seqtype/*, ns, ls, ngene, posG[NGENE+1],lgene[NGENE],*pose,npatt, readpattern*/;
       int /*np, ntime, ncode,fix_kappa,fix_rgene,fix_alpha, clock, model,*/ ncatG/*, cleandata*/;
       int /*print, */nhomo;
//       double *fpatt, *conP;
       /* not used */
//       double lmax,pi[NCODE], kappa,alpha,rou, rgene[NGENE],piG[NGENE][NCODE];
    }  com;

    QString seqfile;
    QString outfile;
    QString treefile;

private:
    PampEngine* engine;
};

#endif // CSTYLEPAMPCTLLOADER_H
