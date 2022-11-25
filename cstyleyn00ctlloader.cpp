#include "cstyleyn00ctlloader.h"
#include "yn00engine.h"
#include <cstdio>

int CStyleYN00CtlLoader::GetOptions(char *ctlf)
{
    int i, nopt=9, lline=4096;
    char line[4096], *pline, opt[20], comment='*';
    const char *optstr[]={"seqfile","outfile", "verbose", "noisy", "icode",
         "weighting","commonkappa", "commonf3x4", "ndata"};
    double t;
    FILE *fctl;

    if((fctl=fopen(ctlf,"r"))==NULL) { error2("\nctl file open error.\n"); return(-1); }
//    printf ("\nReading options from %s..\n", ctlf);
    for (;;) {
       if (fgets (line, lline, fctl) == NULL) break;
       for (i=0,t=0,pline=line; i<lline&&line[i]; i++)
          if (isalnum(line[i]))  { t=1; break; }
          else if (line[i]==comment) break;
       if (t==0) continue;
       sscanf (line, "%s%*s%lf", opt, &t);
       if ((pline=strstr(line, "="))==NULL) { error2("option file."); return(-1); }

       for (i=0; i<nopt; i++) {
          if (strncmp(opt, optstr[i], 8)==0)  {
//             if (noisy>2)
//                printf ("\n%3d %15s | %-20s %6.2f", i+1,optstr[i],opt,t);
             switch (i) {
                case (0): seqfile = QString::fromLocal8Bit(pline+2);
//                          sscanf(pline+2, "%s", com.seqf);
                          break;
                case (1): outfile = QString::fromLocal8Bit(pline+2);
//                          sscanf(pline+2, "%s", com.outf);
                          break;
                case (2): com.verbose=(int)t;     break;
                case (3): noisy=(int)t;           break;
                case (4): com.icode=(int)t;       break;
                case (5): com.weighting=(int)t;   break;
                case (6): com.kcommon=(int)t;     break;
                case (7): com.fcommon=(int)t;     break;
                case (8): com.ndata=(int)t;       break;
             }
             break;
          }
       }
//       if (i==nopt)
//          { printf ("\noption %s in %s\n", opt, ctlf);  exit (-1); }
    }

   fclose (fctl);

   return (0);
}

void CStyleYN00CtlLoader::error2(const char *message)
{
    QString msg(message);
    engine->output("\nError: " + msg + ".\n", QProcess::StandardError);
}
