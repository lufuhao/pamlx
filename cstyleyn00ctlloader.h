#ifndef CSTYLEYN00CTLLOADER_H
#define CSTYLEYN00CTLLOADER_H

//----- C Style code for loading baseml ctl file in paml:  yn00.h -----

#include <QString>

#define NS            1000
//#define LSPNAME       30
#define NCODE         64
//#define NGENE         2000

class YN00Engine;

class CStyleYN00CtlLoader
{
public:
    CStyleYN00CtlLoader(YN00Engine* engine) : engine(engine) {}
    int GetOptions(char *ctlf);
    void error2(const char * message);

public:
    struct common_info {
//       char *z[NS], *spname[NS], seqf[256],outf[256];
       int ns,ls,npatt,codonf,icode,ncode,getSE,*pose,verbose, seqtype, readpattern;
       int cleandata, fcommon,kcommon, weighting, ndata, print;
       double *fpatt, pi[NCODE], f3x4s[NS][12], kappa, omega;
//       int ngene,posG[NGENE+1],lgene[NGENE],fix_rgene, model;
//       double rgene[NGENE],piG[NGENE][NCODE], alpha;
    }  com;

    int noisy;

    QString seqfile;
    QString outfile;

private:
    YN00Engine* engine;
};

#endif // CSTYLEYN00CTLLOADER_H
