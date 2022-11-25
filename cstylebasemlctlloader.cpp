#include "cstylebasemlctlloader.h"
#include "basemlengine.h"
#include <cstdio>

int CStyleBasemlCtlLoader::GetOptions(char *ctlf)
{
    int iopt, i, /*j,*/ nopt=31, lline=4096;
    char line[4096], *pline, opt[32]; const char *comment="*#";
    const char *optstr[]={"seqfile","outfile","treefile","noisy", "cleandata",
         "verbose","runmode", "method", "clock", "TipDate", "fix_rgene","Mgene","nhomo",
         "getSE","RateAncestor", "model","fix_kappa","kappa",
         "fix_alpha","alpha","Malpha","ncatG", "fix_rho","rho",
         "nparK", "ndata", "bootstrap", "Small_Diff","icode", "fix_blength", "seqtype"};
    double t;
    FILE *fctl;
 //   int ng=-1, markgenes[NGENE];

 //   com.nalpha = 0;
    fctl = fopen(ctlf,"r");
    if(!fctl) return(-1);
 //   if(noisy) printf ("Reading options from %s..\n", ctlf);
    for ( ; ; ) {
       if (fgets(line, lline, fctl) == NULL) break;
       for (i=0,t=0,pline=line; i<lline&&line[i]; i++)
          if (isalnum(line[i]))  { t=1; break; }
          else if (strchr(comment,line[i])) break;
       if (t==0) continue;
       sscanf (line, "%s%*s%lf", opt, &t);
       if ((pline=strstr(line, "="))==NULL)
       { error2("option file."); return(-1); }

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
                case ( 3): noisy=(int)t;           break;
                case ( 4): com.cleandata=(char)t;  break;
                case ( 5): com.verbose=(int)t;     break;
                case ( 6): com.runmode=(int)t;     break;
                case ( 7): com.method=(int)t;      break;
                case ( 8): com.clock=(int)t;       break;
                case ( 9):
                   sscanf(pline+1, "%lf%lf", &com.TipDate, &com.TipDate_TimeUnit);
                   break;
                case (10): com.fix_rgene=(int)t;   break;
                case (11): com.Mgene=(int)t;       break;
                case (12): com.nhomo=(int)t;       break;
                case (13): com.getSE=(int)t;       break;
                case (14): com.print=(int)t;       break;
                case (15): com.model=(int)t;
 //                  if(com.model>UNREST) GetStepMatrix(line);
                   break;
                case (16): com.fix_kappa=(int)t;   break;
                case (17):
                   com.kappa=t;
 //                  if(com.fix_kappa && (com.clock==5 || com.clock==6)
 //                     && com.model!=0 && com.model!=2) {
 //                     ng = splitline (++pline, markgenes);
 //                     for(j=0; j<min2(ng,com.ndata); j++)
 //                        if(!sscanf(pline+markgenes[j], "%lf", &data.kappa[j])) break;
 //                        /*
 //                        matout(F0, data.kappa, 1, min2(ng,com.ndata));
 //                        */
 //                  }
                   break;
                case (18): com.fix_alpha=(int)t;   break;
                case (19):
                   com.alpha=t;
 //                  if(com.fix_alpha && t && (com.clock==5 || com.clock==6)) {
 //                     ng = splitline (++pline, markgenes);
 //                     for(j=0; j<min2(ng,com.ndata); j++)
 //                        if(!sscanf(pline+markgenes[j], "%lf", &data.alpha[j])) break;
 //                        /*
 //                        matout(F0, data.alpha, 1, min2(ng,com.ndata));
 //                        */
 //                  }
                   break;
                case (20): com.nalpha=(int)t;      break;
                case (21): com.ncatG=(int)t;       break;
                case (22): com.fix_rho=(int)t;     break;
                case (23): com.rho=t;              break;
                case (24): com.nparK=(int)t;       break;
                case (25): com.ndata=(int)t;       break;
                case (26): com.bootstrap=(int)t;   break;
                case (27): Small_Diff=t;           break;
                case (28): com.icode=(int)t; com.coding=1; break;
                case (29): com.fix_blength=(int)t; break;
                case (30):
                   com.seqtype=(int)t;
 //                  if(com.seqtype==2)      com.ncode = 2;
 //                  else if(com.seqtype==5) com.ncode = 5;
                   break;
             }
             break;
          }
       }
 //      if (iopt==nopt)
 //        { printf ("\noption %s in %s not recognised\n", opt,ctlf); exit(-1); }
    }
    fclose (fctl);

    return (0);
}

void CStyleBasemlCtlLoader::error2(const char *message)
{
    QString msg(message);
    engine->output("\nError: " + msg + ".\n", QProcess::StandardError);
}
