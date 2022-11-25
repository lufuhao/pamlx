#include "cstyleevolverctlloader.h"
#include "evolverengine.h"
#include <cstdio>

#define FOR(i,n) for(i=0; i<n; i++)

int CStyleEvolverCtlLoader::GetOptions(char *ctlf)
{
    FILE *fin;
    char line[32000];
    int lline=32000, i,/*j,*/k, n,nr, fixtree=1;
    int format=0/*, nrate=1*/;
    double /*birth=0, death=0, sample=1, mut=1,*/ tlength/*, *blengthBS*/;
    char ch;

    noisy = 1;
    com.ncode = n = (com.seqtype==0 ? 4 : (com.seqtype==1?64:20));
    fin = (FILE*)fopen(ctlf,"r");
    if(!fin) return(-1);
    if(fscanf(fin, "%d", &format) <= 0) return(-1);
    fgets(line, lline, fin);
    outFormat = format;

    if(fscanf (fin, "%d", &i) <= 0) return(-1);
    fgets(line, lline, fin);
    seed = i;//SetSeed(i, 1);
    if(fscanf (fin, "%d%d%d", &com.ns, &com.ls, &nr) <= 0) return(-1);
    replicates = nr;
    fgets(line, lline, fin);

    if(com.ns>NS) error2("too many seqs?");

    if(fixtree) {
       if(fscanf(fin,"%lf",&tlength) <= 0) return(-1);
       fgets(line, lline, fin);
       treeLength = tlength;

       do {
           ch=fgetc(fin);  /* skip spaces */
       }while(isspace(ch));
       ungetc(ch,fin);
       fgets(line, lline, fin);
       treeString = QString::fromLocal8Bit(line);
    }
    else {   /* random trees, broken or need testing? */ return(-1);
    }

    if(com.seqtype==BASEseq) {
       if(fscanf(fin,"%d",&com.model) <= 0) return(-1);
       fgets(line, lline, fin);
       if(com.model<0 || com.model>REV) error2("model err");
       if(com.model==T92) error2("T92: please use HKY85 with T=A and C=G.");

       do {
           ch=fgetc(fin);  /* skip spaces */
       }while(isspace(ch));
       ungetc(ch,fin);
       fgets(line, lline, fin);
       parameters = QString::fromLocal8Bit(line);
    }
    else if(com.seqtype==CODONseq) {
       if(com.model==0 && com.NSsites) {  /* site model */ return(-1);
       }
       else if(com.model && com.NSsites) {  /* branchsite model */ return(-1);
       }
       else if(!com.model) {  /* M0 */
          if(fscanf(fin,"%lf",&com.omega) <= 0) return(-1);
          fgets(line, lline, fin);
       }

       if(fscanf(fin,"%lf",&com.kappa) <= 0) return(-1);
       fgets(line, lline, fin);
    }

    if(com.seqtype==BASEseq || com.seqtype==AAseq) {
       if(fscanf(fin,"%lf%d", &com.alpha, &com.ncatG) <= 0) return(-1);
       fgets(line, lline, fin);
    }

    if(com.seqtype==AAseq) { /* get aa substitution model and rate matrix */
       if(fscanf(fin,"%d",&com.model) <= 0) return(-1);
       if(fscanf(fin,"%s",com.daafile) <= 0) return(-1);
       fgets(line, lline, fin);
    }
    /* get freqs com.pi[] */
          FOR(k,com.ncode) { if(fscanf(fin,"%lf",&com.pi[k]) <= 0) return(-1); }

    return(0);
}

void CStyleEvolverCtlLoader::error2(const char *message)
{
    QString msg(message);
    engine->output("\nError: " + msg + ".\n", QProcess::StandardError);
}
