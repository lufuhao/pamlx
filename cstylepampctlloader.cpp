#include "cstylepampctlloader.h"
#include "pampengine.h"
#include <cstdio>

int CStylePampCtlLoader::GetOptions(char *ctlf)
{
    int iopt, nopt=6, i, lline=4096, t;
    char line[4096], *pline, opt[32]; const char *comment="*#";
    const char *optstr[] = {"seqfile","outfile","treefile", "seqtype", "ncatG", "nhomo"};
    FILE  *fctl=fopen (ctlf, "r");

    if (fctl) {
       for (;;) {
          if (fgets (line, lline, fctl) == NULL) break;
          for (i=0,t=0; i<lline&&line[i]; i++)
             if (isalnum(line[i]))  { t=1; break; }
             else if (strchr(comment,line[i])) break;
          if (t==0) continue;
          sscanf (line, "%s%*s%d", opt, &t);
          if ((pline=strstr(line, "="))==NULL) { error2 ("option file."); return(-1); }

          for (iopt=0; iopt<nopt; iopt++) {
             if (strncmp(opt, optstr[iopt], 8)==0)  {
//                if (noisy>2)
//                   printf ("\n%3d %15s | %-20s %6d", iopt+1,optstr[iopt],opt,t);
                switch (iopt) {
                   case ( 0): seqfile = QString::fromLocal8Bit(pline+2);
//                              sscanf(pline+2, "%s", com.seqf);
                              break;
                   case ( 1): outfile = QString::fromLocal8Bit(pline+2);
//                              sscanf(pline+2, "%s", com.outf);
                              break;
                   case ( 2): treefile = QString::fromLocal8Bit(pline+2);
//                              sscanf(pline+2, "%s", com.treef);
                              break;
                   case  (3): com.seqtype=t;   break;
                   case  (4): com.ncatG=t;     break;
                   case  (5): com.nhomo=t;     break;
                }
                break;
             }
          }
//          if (iopt==nopt)
//             { printf ("\nopt %s in %s\n", opt, ctlf);  exit (-1); }
       }
       fclose (fctl);
    }
    else
//       if (noisy) printf ("\nno ctl file..");
        { error2("\nno ctl file.."); return(-1); }

//    if (com.seqtype==0)       com.ncode=4;
//    else if (com.seqtype==2)  com.ncode=20;
//    else if (com.seqtype==3)  com.ncode=2;
//    else                      error2("seqtype");
//    if (com.ncatG>NCATG) error2 ("raise NCATG?");

    fclose (fctl);

    return (0);
}

void CStylePampCtlLoader::error2(const char *message)
{
    QString msg(message);
    engine->output("\nError: " + msg + ".\n", QProcess::StandardError);
}
