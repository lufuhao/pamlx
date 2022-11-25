#include "cstylecodemlctlloader.h"
#include "codemlengine.h"
#include <cstdio>

int CStyleCodemlCtlLoader::GetOptions(char *ctlf)
{
    int iopt, i,/*j,*/ nopt=37, lline=255;
    char line[255], *pline, opt[99]; const char *comment="*#";
    const char *optstr[] = {"seqfile", "outfile", "treefile", "seqtype", "noisy",
         "cleandata", "runmode", "method",
         "clock", "TipDate", "getSE", "RateAncestor", "CodonFreq", "estFreq", "verbose",
         "model", "hkyREV", "aaDist","aaRatefile",
         "NSsites", "NShmm", "icode", "Mgene", "fix_kappa", "kappa",
         "fix_omega", "omega", "fix_alpha", "alpha","Malpha", "ncatG",
         "fix_rho", "rho", "ndata", "bootstrap", "Small_Diff", "fix_blength"};
    double t;
    FILE  *fctl;
 //   int ng=-1, markgenes[NGENE+99];
 //   char *daafiles[]={"", "grantham.dat", "miyata.dat",
 //                     "g1974c.dat","g1974p.dat","g1974v.dat","g1974a.dat"};

    /* kostas, default prior for t & w */
 //   com.hyperpar[0]=1.1; com.hyperpar[1]=1.1; com.hyperpar[2]=1.1; com.hyperpar[3]=2.2;
    fctl=fopen(ctlf,"r");
    if(!fctl) return(-1);
 //   if (noisy) printf ("\n\nReading options from %s..\n", ctlf);
    for (;;) {
       if (fgets(line, lline, fctl) == NULL) break;
       for (i=0,t=0,pline=line; i<lline&&line[i]; i++)
          if (isalnum(line[i]))  { t=1; break; }
          else if (strchr(comment,line[i])) break;
       if (t==0) continue;
       sscanf (line, "%s%*s%lf", opt,&t);
       if ((pline=strstr(line, "="))==NULL)
          { error2("err: option file. add space around the equal sign?"); return(-1); }
       for (iopt=0; iopt<nopt; iopt++) {
          if (strncmp(opt, optstr[iopt], 8)==0)  {
 //            if (noisy>=9)
 //               printf ("\n%3d %15s | %-20s %6.2f", iopt+1,optstr[iopt],opt,t);
             switch (iopt) {
                case ( 0):
                   seqfile = QString::fromLocal8Bit(pline+1);
 //                sscanf(pline+1, "%s", com.seqf);
                   break;
                case ( 1):
                   outfile = QString::fromLocal8Bit(pline+1);
 //                sscanf(pline+1, "%s", com.outf);
                   break;
                case ( 2):
                   treefile = QString::fromLocal8Bit(pline+1);
 //                sscanf(pline+1, "%s", com.treef);
                   break;
                case ( 3): com.seqtype=(int)t;     break;
                case ( 4): noisy=(int)t;           break;
                case ( 5): com.cleandata=(char)t;  break;
                case ( 6):
                   sscanf(pline+1, "%d%lf%lf%lf%lf", &com.runmode, com.hyperpar, com.hyperpar+1, com.hyperpar+2, com.hyperpar+3);            
                   break;
                case ( 7): com.method=(int)t;      break;
                case ( 8): com.clock=(int)t;       break;
                case ( 9):
                   sscanf(pline+1, "%lf%lf", &com.TipDate, &com.TipDate_TimeUnit);
                   break;
                case (10): com.getSE=(int)t;       break;
                case (11): com.print=(int)t;       break;
                case (12): com.codonf=(int)t;      break;
                case (13): com.npi=(int)t;         break;
                case (14): com.verbose=(int)t;     break;
                case (15): com.model=(int)t;       break;
                case (16): com.hkyREV=(int)t;      break;
                case (17): com.aaDist=(int)t;      break;
                case (18):
                   aaRatefile = QString::fromLocal8Bit(pline+2);
 //                  sscanf(pline+2,"%s",com.daafile);
 //                  if(com.seqtype==2 && com.ndata>1 && (com.clock==5 || com.clock==6)) {
 //                     ReadDaafiles(pline+2);
 //                     break;
 //                  }
                   break;
                case (19): { if(ReadNSsitesModels(line)) return(-1); } break;
                case (20): com.nparK=(int)t;       break;
                case (21):
                   com.icode=(int)t;
 //                  if(com.seqtype==1 && (com.clock==5 || com.clock==6)) {
 //                     ng = splitline (++pline, markgenes);
 //                     for(j=0; j<min2(ng,com.ndata); j++)
 //                        if(!sscanf(pline+markgenes[j],"%d",&data.icode[j])) break;

 //                     for(j=0; j<min2(ng,com.ndata); j++) printf("%4d", data.icode[j]);  FPN(F0);

 //                  }
                   break;
                case (22): com.Mgene=(int)t;       break;
                case (23): com.fix_kappa=(int)t;   break;
                case (24):
                   com.kappa=t;
 //                  if(com.seqtype==1 && com.fix_kappa && (com.clock==5 || com.clock==6)) {
 //                     ng = splitline (++pline, markgenes);
 //                     for(j=0; j<min2(ng,com.ndata); j++)
 //                        if(!sscanf(pline+markgenes[j],"%lf",&data.kappa[j])) break;

 //                     matout(F0, data.kappa, 1, min2(ng,com.ndata));
 //                  }
                   break;
                case (25): com.fix_omega=(int)t;   break;
                case (26):
                   com.omega=t;
 //                  if(com.seqtype==1 && com.fix_omega && (com.clock==5 || com.clock==6)) {
 //                     ng = splitline (++pline, markgenes);
 //                     for(j=0; j<min2(ng,com.ndata); j++)
 //                        if(!sscanf(pline+markgenes[j],"%lf",&data.omega[j])) break;

 //                     matout(F0, data.omega, 1, min2(ng,com.ndata));
 //                  }

                   break;
                case (27): com.fix_alpha=(int)t;   break;
                case (28):
                   com.alpha=t;
 //                  if(com.fix_alpha && t && (com.clock==5 || com.clock==6)) {
 //                     ng = splitline (++pline, markgenes);
 //                     for(j=0; j<min2(ng,com.ndata); j++)
 //                        if(!sscanf(pline+markgenes[j], "%lf", &data.alpha[j])) break;

 //                     matout(F0, data.alpha, 1, min2(ng,com.ndata));
 //                  }
                   break;
                case (29): com.nalpha=(int)t;      break;
                case (30): com.ncatG=(int)t;       break;
                case (31): com.fix_rho=(int)t;     break;
                case (32): com.rho=t;              break;
                case (33): com.ndata=(int)t;       break;
                case (34): com.bootstrap=(int)t;   break;
                case (35): Small_Diff=t;           break;
                case (36): com.fix_blength=(int)t; break;
            }
            break;
          }
       }
 //      if (iopt==nopt)
 //        { printf ("\noption %s in %s not recognised\n", opt,ctlf); exit(-1); }
    }
    fclose (fctl);

    return(0);
}

void CStyleCodemlCtlLoader::error2(const char *message)
{
    QString msg(message);
    engine->output("\nError: " + msg + ".\n", QProcess::StandardError);
}

int CStyleCodemlCtlLoader::ReadNSsitesModels(char *line)
{
/* This reads the line  NSsites = 0 1 2 3 7 8  in codeml.ctl.
*/
    char *pline;
    int pop_digit;

    if ((pline=strstr(line, "="))==NULL) { error2(".ctl file error NSsites"); return(-1); }
    pline++;
    for (nnsmodels=0; nnsmodels<int(sizeof(nsmodels)/sizeof(int)) && nnsmodels<maxNSsitesModels; nnsmodels++) {
       if(sscanf(pline, "%d", &nsmodels[nnsmodels]) != 1) break;
       for(pop_digit=0; ; ) {
          if(isdigit(*pline)) { pline++; pop_digit=1; }
          else if(isspace(*pline)) {
             pline++;
             if(pop_digit) break;
          }
          else  error2(".ctl file NSsites line strange.");
       }
       if(nsmodels[nnsmodels]<0 || nsmodels[nnsmodels]>=maxNSsitesModels)
          error2("NSsites model");
    }
//    com.NSsites=nsmodels[0];

    return(0);
}
