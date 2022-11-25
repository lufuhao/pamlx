#include "cstylemcmctreectlloader.h"
#include "mcmctreeengine.h"
#include <cstdio>

int CStyleMCMCTreeCtlLoader::GetOptions(char *ctlf)
{
    int  transform0=ARCSIN_B; /* default transform: SQRT_B, LOG_B, ARCSIN_B */
    int  iopt, i, /*j,*/ nopt=29, lline=4096;
    char line[4096],*pline, /**peq,*/ opt[32]; const char *comment="*#";
    const char *optstr[] = {"seed", "seqfile","treefile", "outfile", "mcmcfile",
         "seqtype", "aaRatefile", "icode", "noisy", "usedata", "ndata", "model", "clock",
         "TipDate", "RootAge", "fossilerror", "alpha", "ncatG", "cleandata",
         "BDparas", "kappa_gamma", "alpha_gamma",
         "rgene_gamma", "sigma2_gamma", "print", "burnin", "sampfreq",
         "nsample", "finetune"};
    double t=1, *eps=mcmc.finetune;
    FILE  *fctl=fopen (ctlf, "r");

    data.transform = transform0;
    if (fctl) {
//        if (noisy) printf ("\nReading options from %s..\n", ctlf);
        for (;;) {
            if (fgets (line, lline, fctl) == NULL) break;
            for (i=0,t=0,pline=line; i<lline&&line[i]; i++)
                if (isalnum(line[i]))  { t=1; break; }
                else if (strchr(comment,line[i])) break;
            if (t==0) continue;
            if ((pline=strstr(line, "="))==NULL)
                { error2 ("option file."); fclose(fctl); return(-1); }
            *pline='\0';   sscanf(line, "%s", opt);  *pline='=';   
            sscanf(pline+1, "%lf", &t);

            for (iopt=0; iopt<nopt; iopt++) {
                if (strncmp(opt, optstr[iopt], 8)==0)  {
//                    if (noisy>=9)
//                        printf ("\n%3d %15s | %-20s %6.2f", iopt+1,optstr[iopt],opt,t);
                    switch (iopt) {
                    case ( 0): seed = (int)t;/*SetSeed((int)t, 1);*/  break;
                    case ( 1):
                        seqfile = QString::fromLocal8Bit(pline+1);
//                        sscanf(pline+1, "%s", com.seqf);
                        break;
                    case ( 2):
                        treefile = QString::fromLocal8Bit(pline+1);
//                        sscanf(pline+1, "%s", com.treef);
                        break;
                    case ( 3):
                        outfile = QString::fromLocal8Bit(pline+1);
//                        sscanf(pline+1, "%s", com.outf);
                        break;
                    case ( 4): sscanf(pline+1, "%s", com.mcmcf);   break;
                    case ( 5): com.seqtype=(int)t;    break;
                    case ( 6): sscanf(pline+2,"%s", com.daafile);  break;
                    case ( 7): com.icode=(int)t;      break;
                    case ( 8): noisy=(int)t;          break;
                    case ( 9):
                        /*j=*/sscanf(pline+1, "%d %s%d", &mcmc.usedata, com.inBVf, &data.transform);
//                        if(mcmc.usedata==2)
//                           if(strchr(com.inBVf, '*')) { strcpy(com.inBVf, "in.BV"); data.transform=transform0; }
//                           else if(j==2)              data.transform=transform0;
                        break;
                    case (10): com.ndata=(int)t;      break;
                    case (11): com.model=(int)t;      break;
                    case (12): com.clock=(int)t;      break;
                    case (13):
                        sscanf(pline+2,"%lf%lf", &com.TipDate, &com.TipDate_TimeUnit);
                        if(com.TipDate && com.TipDate_TimeUnit==0) error2("should set com.TipDate_TimeUnit");
                        data.transform = SQRT_B;  /* SQRT_B, LOG_B, ARCSIN_B */
                        break;
                    case (14):
                        RootAge = QString::fromLocal8Bit(pline+1);
//                        sptree.RootAge[2] = sptree.RootAge[3] = 0.025;  /* default tail probs */
//                        if((strchr(line, '>') || strchr(line, '<')) && (strstr(line, "U(") || strstr(line, "B(")))
//                           error2("don't mix < U B on the RootAge line");

//                        if((pline=strchr(line, '>')))
//                            sscanf(pline+1, "%lf", &sptree.RootAge[0]);
//                        if((pline=strchr(line,'<'))) {  /* RootAge[0]=0 */
//                            sscanf(pline+1, "%lf", &sptree.RootAge[1]);
//                        }
//                        if((pline=strstr(line, "U(")))
//                            sscanf(pline+2, "%lf,%lf", &sptree.RootAge[1], &sptree.RootAge[2]);
//                        else if((pline=strstr(line, "B(")))
//                            sscanf(pline+2, "%lf,%lf,%lf,%lf", &sptree.RootAge[0], &sptree.RootAge[1], &sptree.RootAge[2], &sptree.RootAge[3]);
                        break;
                    case (15):
                        data.pfossilerror[0] = 0.0;
                        data.pfossilerror[2] = 1;  /* default: minimum 2 good fossils */
                        sscanf(pline+1, "%lf%lf%lf", data.pfossilerror, data.pfossilerror+1, data.pfossilerror+2);
                        break;
                    case (16): com.alpha=t;           break;
                    case (17): com.ncatG=(int)t;      break;
                    case (18): com.cleandata=(int)t;  break;
                    case (19):
                        sscanf(pline+1,"%lf%lf%lf%lf", &data.BDS[0],&data.BDS[1],&data.BDS[2],&data.BDS[3]);
                        break;
                    case (20):
                        sscanf(pline+1,"%lf%lf", data.kappagamma, data.kappagamma+1); break;
                    case (21):
                        sscanf(pline+1,"%lf%lf", data.alphagamma, data.alphagamma+1); break;
                    case (22):
                        sscanf(pline+1,"%lf%lf%lf%d", &data.rgenepara[0], &data.rgenepara[1], &data.rgenepara[2], &data.rgeneprior); 
//                        if(data.rgenegD[2]<=0) data.rgenegD[2]=1;
//                        if(data.rgeneprior<0)  data.rgeneprior=0;
                        break;
                    case (23):
                        sscanf(pline+1,"%lf%lf%lf", data.sigma2para, data.sigma2para+1, data.sigma2para+2); 
//                        if(data.sigma2gD[2]<=0) data.sigma2gD[2]=1;
                        break;
                    case (24): mcmc.print=(int)t;     break;
                    case (25): mcmc.burnin=(int)t;    break;
                    case (26): mcmc.sampfreq=(int)t;  break;
                    case (27): mcmc.nsample=(int)t;   break;
                    case (28):
                        if( engine->compareVersion("4.9") < 0 )
                            sscanf(pline+1,"%d:%lf%lf%lf%lf%lf%lf", &mcmc.resetFinetune, eps,eps+1,eps+2,eps+3,eps+4,eps+5);
                        else
                            warning("finetune is deprecated now.");
                        break;
//                        sscanf(pline + 1, "%d:%lf%lf%lf%lf%lf%lf", &j, eps, eps + 1, eps + 2, eps + 3, eps + 4, eps + 5);
//                        break;
                    }
                    break;
                }
            }
//            if (iopt==nopt)
//               { printf ("\noption %s in %s\n", opt, ctlf);  exit (-1); }
        }
        fclose(fctl);
    }
    else
       /*if (noisy)*/ { error2("\nno ctl file.."); return(-1); }

    if(com.ndata>NGENE) error2("raise NGENE?");
//    else if(com.ndata<=0) com.ndata=1;

//    if(com.seqtype==2)
//        com.ncode = 20;

//    if(com.alpha==0)  { com.fix_alpha=1; com.nalpha=0; }
    if(com.clock<1 || com.clock>3) error2("clock should be 1, 2, 3?");
    return(0);
}

void CStyleMCMCTreeCtlLoader::error2(const char *message)
{
    QString msg(message);
    engine->output("\nError: " + msg + ".\n", QProcess::StandardError);
}

void CStyleMCMCTreeCtlLoader::warning(const char *message)
{
    QString msg(message);
    engine->output("\nWarning: " + msg + ".\n", QProcess::StandardError);
}
