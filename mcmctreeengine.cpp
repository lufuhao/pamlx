//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "mcmctreeengine.h"
#include "def.h"
#include "cstylemcmctreectlloader.h"
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include "mydebug.h"

MCMCTreeEngine::MCMCTreeEngine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent) :
    Engine(aOutputWindow, msec1, msec2, parent)
{
    setPath(pamlPath);
    setDefaultData();
}

bool MCMCTreeEngine::setPath(const QString& pamlPath)
{
#ifdef Q_OS_WIN32
    mPath = pamlPath + "/bin/mcmctree.exe";
#else
    mPath = pamlPath + "/bin/mcmctree";
#endif

    if( QFileInfo(mPath).isFile() )
        statusOk = true;
    else
        statusOk = false;

    mVersion.clear();
    if(statusOk) {
        QString pamlhFileName = pamlPath + "/src/paml.h";
        QFile pamlhFile(pamlhFileName);
        if( pamlhFile.open(QIODevice::ReadOnly) ) {
            QTextStream pamlhIn(&pamlhFile);
            while(!pamlhIn.atEnd()) {
                QString line = pamlhIn.readLine();
                if( line.startsWith("#define pamlVerStr") ) {
                    mVersion = line.section(' ', 4, 4).section(',', 0, 0);
                    break;
                }
            }
        }
    }

    return statusOk;
}

void MCMCTreeEngine::refreshFileContent()
{
    if(!modified && !mFileName.isEmpty()) return;

    QString tmpContent = mFileContent;
    mFileContent.clear();

    QTextStream in(&tmpContent, QIODevice::ReadOnly);
    QTextStream out(&mFileContent, QIODevice::WriteOnly);

    QStringList optionList;
    optionList << "seed"
               << "seqfile"
               << "treefile"
               << "outfile"
//               << "mcmcfile"
//               << "seqtype"
//               << "aaRatefile"
//               << "icode"
//               << "noisy"
               << "usedata"
               << "ndata"
               << "model"
               << "clock"
               << "TipDate"
               << "RootAge"
//               << "fossilerror"
               << "alpha"
               << "ncatG"
               << "cleandata"
               << "BDparas"
               << "kappa_gamma"
               << "alpha_gamma"
               << "rgene_gamma"
               << "sigma2_gamma"
               << "print"
               << "burnin"
               << "sampfreq"
               << "nsample"
               << "finetune";

    int size= optionList.size();
    bool* used = new bool[size];
    for(int i=0; i<size; i++) used[i] = false;

    while(!in.atEnd()) {
        QString line = in.readLine();
        QString optionString = line.section('*', 0, 0).section('=', 0, 0);
        QString comment = line.section('*', 1);
        QString option = optionString.trimmed();

        if(option.isEmpty()) {
            out << line << "\n";
            continue;
        }

        int i=0;
        for(; i<size; i++) {
            if(option == optionList[i]) {
                if(!optionString.endsWith(' ')) optionString.append(' ');

                out << optionString << "= ";

                switch(i)
                {
                case 0:
                    out << nEdit_seed;
                    break;
                case 1:
                    strEdit_seqfile = strEdit_seqfile.trimmed();
                    out << strEdit_seqfile;
                    break;
                case 2:
                    strEdit_treefile = strEdit_treefile.trimmed();
                    out << strEdit_treefile;
                    break;
                case 3:
                    strEdit_outfile_Location = strEdit_outfile_Location.trimmed();
                    strEdit_outfile_Name = strEdit_outfile_Name.trimmed();
                    if(strEdit_outfile_Location.isEmpty())
                        out << strEdit_outfile_Name;
                    else if(strEdit_outfile_Location == "/" || strEdit_outfile_Location == "\\")
                        out << QDir::fromNativeSeparators(QDir::cleanPath( "/" + strEdit_outfile_Name ));
                    else
                        out << QDir::fromNativeSeparators(QDir::cleanPath( strEdit_outfile_Location + "/" + strEdit_outfile_Name ));
                    break;
                case 4:
                    if(nComb_usedata >= 0)
                        out << nComb_usedata;
                    break;
                case 5:
                    out << nEdit_ndata;
                    break;
                case 6:
                    if(nComb_model >= 0)
                        out << nComb_model;
                    break;
                case 7:
                    if(nComb_clock >= 1)
                        out << nComb_clock;
                    break;
                case 8:
                    out << nEdit_TipDate << ' ' << nEdit_TipDate_TimeUnit;
                    break;
                case 9:
                    strEdit_RootAge = strEdit_RootAge.trimmed();
                    out << strEdit_RootAge;
                    break;
                case 10:
                    out << nEdit_alpha;
                    break;
                case 11:
                    out << nEdit_ncatG;
                    break;
                case 12:
                    out << char('0' + bButt_cleandata);
                    break;
                case 13:
                    out << nEdit_BDparas[0] << ' ' << nEdit_BDparas[1] << ' ' << nEdit_BDparas[2] << ' ' << nEdit_BDparas[3];
                    break;
                case 14:
                    out << nEdit_kappa_gamma[0] << ' ' << nEdit_kappa_gamma[1];
                    break;
                case 15:
                    out << nEdit_alpha_gamma[0] << ' ' << nEdit_alpha_gamma[1];
                    break;
                case 16:
                    out << nEdit_rgene_para[0] << ' ' << nEdit_rgene_para[1] << ' ' << nEdit_rgene_para[2];
                    if(nComb_rgeneprior >= 0)
                        out << ' ' << nComb_rgeneprior;
                    break;
                case 17:
                    out << nEdit_sigma2_para[0] << ' ' << nEdit_sigma2_para[1] << ' ' << nEdit_sigma2_para[2];
                    break;
                case 18:
                    out << char('0' + bButt_print);
                    break;
                case 19:
                    out << nEdit_burnin;
                    break;
                case 20:
                    out << nEdit_sampfreq;
                    break;
                case 21:
                    out << nEdit_nsample;
                    break;
                case 22:
                    if( compareVersion("4.9") < 0 )
                        out << char('0' + bButt_autoAdjustFinetune) << ": " << nEdit_finetune[0] << ' ' << nEdit_finetune[1] << ' ' << nEdit_finetune[2] << ' ' << nEdit_finetune[3] << ' ' << nEdit_finetune[4] << ' ';
                    break;
                default:;
                }

                if(!comment.isEmpty())
                    out << "   *" << comment << "\n";
                else
                    out << "\n";

                used[i] = true;
                break;
            }
        }

        if(i == size) {
            out << line << "\n";
        }
    }

    for(int i=0; i<size; i++) {
        if(!used[i]) {
            switch(i)
            {
            case 0:
                out << "          seed = " << nEdit_seed << "\n";
                break;
            case 1:
                strEdit_seqfile = strEdit_seqfile.trimmed();
                out << "       seqfile = " << strEdit_seqfile << "\n";
                break;
            case 2:
                strEdit_treefile = strEdit_treefile.trimmed();
                out << "      treefile = " << strEdit_treefile << "\n";
                break;
            case 3:
                strEdit_outfile_Location = strEdit_outfile_Location.trimmed();
                strEdit_outfile_Name = strEdit_outfile_Name.trimmed();
                if(strEdit_outfile_Location.isEmpty())
                    out << "       outfile = " << strEdit_outfile_Name << "\n\n";
                else if(strEdit_outfile_Location == "/" || strEdit_outfile_Location == "\\")
                    out << "       outfile = " << QDir::fromNativeSeparators(QDir::cleanPath( "/" + strEdit_outfile_Name )) << "\n\n";
                else
                    out << "       outfile = " << QDir::fromNativeSeparators(QDir::cleanPath( strEdit_outfile_Location + "/" + strEdit_outfile_Name )) << "\n\n";
                break;
            case 4:
                if(nComb_usedata >= 0)
                    out << "       usedata = " << nComb_usedata << "\n";
                else
                    out << "       usedata = \n";
                break;
            case 5:
                out << "         ndata = " << nEdit_ndata << "\n";
                break;
            case 6:
                if(nComb_model >= 0)
                    out << "         model = " << nComb_model << "\n";
                else
                    out << "         model = \n";
                break;
            case 7:
                if(nComb_clock >= 1)
                    out << "         clock = " << nComb_clock << "\n";
                else
                    out << "         clock = \n";
                break;
            case 8:
                out << "       TipDate = " << nEdit_TipDate << ' ' << nEdit_TipDate_TimeUnit << "\n";
                break;
            case 9:
                strEdit_RootAge = strEdit_RootAge.trimmed();
                out << "       RootAge = " << strEdit_RootAge << "\n\n";
                break;
            case 10:
                out << "         alpha = " << nEdit_alpha << "\n";
                break;
            case 11:
                out << "         ncatG = " << nEdit_ncatG << "\n\n";
                break;
            case 12:
                out << "     cleandata = " << char('0' + bButt_cleandata) << "\n\n";
                break;
            case 13:
                out << "       BDparas = " << nEdit_BDparas[0] << ' ' << nEdit_BDparas[1] << ' ' << nEdit_BDparas[2] << ' ' << nEdit_BDparas[3] << "\n";
                break;
            case 14:
                out << "   kappa_gamma = " << nEdit_kappa_gamma[0] << ' ' << nEdit_kappa_gamma[1] << "\n";
                break;
            case 15:
                out << "   alpha_gamma = " << nEdit_alpha_gamma[0] << ' ' << nEdit_alpha_gamma[1] << "\n\n";
                break;
            case 16:
                out << "   rgene_gamma = " << nEdit_rgene_para[0] << ' ' << nEdit_rgene_para[1] << ' ' << nEdit_rgene_para[2];
                if(nComb_rgeneprior >= 0)
                    out << ' ' << nComb_rgeneprior;
                out << "\n";
                break;
            case 17:
                out << "  sigma2_gamma = " << nEdit_sigma2_para[0] << ' ' << nEdit_sigma2_para[1] << ' ' << nEdit_sigma2_para[2] << "\n\n";
                break;
            case 18:
                out << "         print = " << char('0' + bButt_print) << "\n";
                break;
            case 19:
                out << "        burnin = " << nEdit_burnin << "\n";
                break;
            case 20:
                out << "      sampfreq = " << nEdit_sampfreq << "\n";
                break;
            case 21:
                out << "       nsample = " << nEdit_nsample << "\n\n";
                break;
            case 22:
                if( compareVersion("4.9") < 0 )
                    out << "      finetune = " << char('0' + bButt_autoAdjustFinetune) << ": " << nEdit_finetune[0] << ' ' << nEdit_finetune[1] << ' ' << nEdit_finetune[2] << ' ' << nEdit_finetune[3] << ' ' << nEdit_finetune[4] << ' ' << "\n\n";
                break;
            default:;
            }
        }
    }

    delete used;
    used = NULL;
}

int MCMCTreeEngine::compareVersion(const QString& versionStr) const
{
    unsigned int mMajor, mMinor, major, minor;
    QString mRevision, revision;
    char c;

    QTextStream(mVersion.toAscii()) >> mMajor >> c >> mMinor >> mRevision;
    QTextStream(versionStr.toAscii()) >> major >> c >> minor >> revision;

    int ret;

    if(mMajor > major)
        ret = 1;
    else if(mMajor < major)
        ret = -1;
    else {
        if(mMinor > minor)
            ret = 1;
        else if(mMinor < minor)
            ret = -1;
        else
            ret = QString::compare(mRevision, revision, Qt::CaseInsensitive);
    }

    return ret;
}

void MCMCTreeEngine::setDefaultData()
{
    nEdit_seed = -1;
    strEdit_seqfile = "";
    strEdit_treefile = "";
    strEdit_outfile_Name = "out";
    strEdit_outfile_Location = "";
    nComb_usedata = 1;
    nEdit_ndata = 1;
    nComb_model = 4;
    nComb_clock = 0;
    nEdit_TipDate = 0;
    nEdit_TipDate_TimeUnit = 100.0;
    strEdit_RootAge = "'<1.0'";
    nEdit_alpha = 0;
    nEdit_ncatG = 5;
    bButt_cleandata = false;
    nEdit_BDparas[0] = 0.2;
    nEdit_BDparas[1] = 0.1;
    nEdit_BDparas[2] = 0;
    nEdit_BDparas[3] = 0.18;
    nEdit_kappa_gamma[0] = 6;
    nEdit_kappa_gamma[1] = 2;
    nEdit_alpha_gamma[0] = 1;
    nEdit_alpha_gamma[1] = 1;
    nComb_rgeneprior = 0;
    nEdit_rgene_para[0] = 2;
    nEdit_rgene_para[1] = 2;
    nEdit_rgene_para[2] = 1;
    nEdit_sigma2_para[0] = 1;
    nEdit_sigma2_para[1] = 10;
    nEdit_sigma2_para[2] = 1;
    bButt_print = true;
    nEdit_burnin = 2000;
    nEdit_sampfreq = 2;
    nEdit_nsample = 20000;
    if( compareVersion("4.9") < 0 ) {
        bButt_autoAdjustFinetune = true;
        nEdit_finetune[0] = 0.04;
        nEdit_finetune[1] = 0.25;
        nEdit_finetune[2] = 0.1;
        nEdit_finetune[3] = 0.1;
        nEdit_finetune[4] = 0.5;
    }
}

bool MCMCTreeEngine::readFile(const QString& fileName)
{
    CStyleMCMCTreeCtlLoader* ctlLoader = new CStyleMCMCTreeCtlLoader(this);
    QFile ctlFile(fileName);

    ctlLoader->seed = -1;
    ctlLoader->mcmc.usedata = 2;
    ctlLoader->com.ndata = 1;
    ctlLoader->com.model = 4;
    ctlLoader->com.clock = 1;
    ctlLoader->com.TipDate = 0;
    ctlLoader->com.TipDate_TimeUnit = 100.0;
    ctlLoader->com.alpha = 0;
    ctlLoader->com.ncatG = 1;
    ctlLoader->com.cleandata = 0;
    ctlLoader->data.BDS[0] = 1;
    ctlLoader->data.BDS[1] = 1;
    ctlLoader->data.BDS[2] = 0;
    ctlLoader->data.BDS[3] = 0;
    ctlLoader->data.kappagamma[0] = 6;
    ctlLoader->data.kappagamma[1] = 2;
    ctlLoader->data.alphagamma[0] = 1;
    ctlLoader->data.alphagamma[1] = 1;
    ctlLoader->data.rgeneprior = 0;
    ctlLoader->data.rgenepara[0] = 2;
    ctlLoader->data.rgenepara[1] = 2;
    ctlLoader->data.rgenepara[2] = 1;
    ctlLoader->data.sigma2para[0] = 1;
    ctlLoader->data.sigma2para[1] = 10;
    ctlLoader->data.sigma2para[2] = 1;
    ctlLoader->mcmc.print = 1;
    ctlLoader->mcmc.burnin = 2000;
    ctlLoader->mcmc.sampfreq = 2;
    ctlLoader->mcmc.nsample = 20000;
    if( compareVersion("4.9") < 0 ) {
        ctlLoader->mcmc.resetFinetune = 1;
        ctlLoader->mcmc.finetune[0] = 0.1;
        ctlLoader->mcmc.finetune[1] = 0.1;
        ctlLoader->mcmc.finetune[2] = 0.1;
        ctlLoader->mcmc.finetune[3] = 0.1;
        ctlLoader->mcmc.finetune[4] = 0.1;
    }

    if( ctlLoader->GetOptions(fileName.toLocal8Bit().data()) != 0 || !ctlFile.open(QIODevice::ReadOnly) ) {
        delete ctlLoader;

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Error !"));
        msgBox.setText(tr("Failed to read file:"));
        msgBox.setInformativeText(QDir::toNativeSeparators(fileName));
        msgBox.exec();
        return false;
    }

    QTextStream ctlIn(&ctlFile);
    mFileContent = ctlIn.readAll();

    nEdit_seed = ctlLoader->seed;
    strEdit_seqfile = ctlLoader->seqfile.section('*', 0, 0).trimmed();
    strEdit_treefile = ctlLoader->treefile.section('*', 0, 0).trimmed();
    QString outfile = ctlLoader->outfile.section('*', 0, 0).trimmed();
    strEdit_outfile_Name = QFileInfo(outfile).fileName();
    strEdit_outfile_Location = QDir::toNativeSeparators(QFileInfo(outfile).path());
    if(strEdit_outfile_Location == ".") strEdit_outfile_Location = QDir::toNativeSeparators("./");
    if(strEdit_outfile_Location == "..") strEdit_outfile_Location = QDir::toNativeSeparators("../");
    nComb_usedata = ctlLoader->mcmc.usedata;
    nEdit_ndata = ctlLoader->com.ndata;
    nComb_model = ctlLoader->com.model;
    nComb_clock = ctlLoader->com.clock;
    nEdit_TipDate = ctlLoader->com.TipDate;
    nEdit_TipDate_TimeUnit = ctlLoader->com.TipDate_TimeUnit;
    strEdit_RootAge = ctlLoader->RootAge.section('*', 0, 0).trimmed();
    nEdit_alpha = ctlLoader->com.alpha;
    nEdit_ncatG = ctlLoader->com.ncatG;
    bButt_cleandata = ctlLoader->com.cleandata;
    nEdit_BDparas[0] = ctlLoader->data.BDS[0];
    nEdit_BDparas[1] = ctlLoader->data.BDS[1];
    nEdit_BDparas[2] = ctlLoader->data.BDS[2];
    nEdit_BDparas[3] = ctlLoader->data.BDS[3];
    nEdit_kappa_gamma[0] = ctlLoader->data.kappagamma[0];
    nEdit_kappa_gamma[1] = ctlLoader->data.kappagamma[1];
    nEdit_alpha_gamma[0] = ctlLoader->data.alphagamma[0];
    nEdit_alpha_gamma[1] = ctlLoader->data.alphagamma[1];
    nComb_rgeneprior = ctlLoader->data.rgeneprior;
    nEdit_rgene_para[0] = ctlLoader->data.rgenepara[0];
    nEdit_rgene_para[1] = ctlLoader->data.rgenepara[1];
    nEdit_rgene_para[2] = ctlLoader->data.rgenepara[2];
    nEdit_sigma2_para[0] = ctlLoader->data.sigma2para[0];
    nEdit_sigma2_para[1] = ctlLoader->data.sigma2para[1];
    nEdit_sigma2_para[2] = ctlLoader->data.sigma2para[2];
    bButt_print = ctlLoader->mcmc.print;
    nEdit_burnin = ctlLoader->mcmc.burnin;
    nEdit_sampfreq = ctlLoader->mcmc.sampfreq;
    nEdit_nsample = ctlLoader->mcmc.nsample;
    if( compareVersion("4.9") < 0 ) {
        bButt_autoAdjustFinetune = ctlLoader->mcmc.resetFinetune;
        nEdit_finetune[0] = ctlLoader->mcmc.finetune[0];
        nEdit_finetune[1] = ctlLoader->mcmc.finetune[1];
        nEdit_finetune[2] = ctlLoader->mcmc.finetune[2];
        nEdit_finetune[3] = ctlLoader->mcmc.finetune[3];
        nEdit_finetune[4] = ctlLoader->mcmc.finetune[4];
    }

    delete ctlLoader;

    return true;
}

bool MCMCTreeEngine::writeFile(const QString& fileName)
{
    refreshFileContent();

    QFile ctlFile(fileName);
    if( !ctlFile.open(QIODevice::WriteOnly) ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Error !"));
        msgBox.setText(tr("Failed to write to file:"));
        msgBox.setInformativeText(QDir::toNativeSeparators(fileName));
        msgBox.exec();
        return false;
    }
    QTextStream ctlOut(&ctlFile);

    ctlOut << mFileContent;
    ctlOut.flush();

    return true;
}

bool MCMCTreeEngine::createTmpCtlFile(const QString& fileName)
{
    QFile ctlFile(fileName);
    if( !ctlFile.open(QIODevice::WriteOnly) ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Error !"));
        msgBox.setText(tr("Failed to create or open file:"));
        msgBox.setInformativeText(QDir::toNativeSeparators(fileName));
        msgBox.exec();
        return false;
    }
    QTextStream ctlOut(&ctlFile);

    ctlOut << "          seed = " << nEdit_seed << "\n";

    strEdit_seqfile = strEdit_seqfile.trimmed();
    if( strEdit_seqfile.isEmpty() ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Sequence file's path can not be empty."));
        msgBox.setInformativeText(tr("Please enter <i>seqfile</i>."));
        msgBox.exec();
        return false;
    }
    if( strEdit_seqfile.length() >= MAX_PATH ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Full name of sequence file is too long."));
        msgBox.setInformativeText(tr("Please check <i>seqfile</i>."));
        msgBox.exec();
        return false;
    }
    if( strEdit_seqfile.contains(' ') ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("There should be no space characters in sequence file's path."));
        msgBox.setInformativeText(tr("Please check <i>seqfile</i>."));
        msgBox.exec();
        return false;
    }
    if( QDir::isAbsolutePath(strEdit_seqfile) || mFileName.isEmpty() ) {
        if( !QDir::isAbsolutePath(strEdit_seqfile) || !QFileInfo(strEdit_seqfile).isFile() ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Failed to open sequence file."));
            msgBox.setInformativeText(tr("Please check <i>seqfile</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "       seqfile = " << strEdit_seqfile << "\n";
    }
    else {
        QString seqfilePath = QDir::cleanPath( QFileInfo(mFileName).absoluteDir().absoluteFilePath(strEdit_seqfile) );
        if( !QFileInfo(seqfilePath).isFile() ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Failed to open sequence file."));
            msgBox.setInformativeText(tr("Please check <i>seqfile</i>."));
            msgBox.exec();
            return false;
        }
        if( seqfilePath.length() >= MAX_PATH ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Absolute path of sequence file is too long."));
            msgBox.setInformativeText(QDir::toNativeSeparators(seqfilePath));
            msgBox.exec();
            return false;
        }
        if( seqfilePath.contains(' ') ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Absolute path of sequence file can not have space characters."));
            msgBox.setInformativeText(QDir::toNativeSeparators(seqfilePath));
            msgBox.exec();
            return false;
        }
        ctlOut << "       seqfile = " << seqfilePath << "\n";
    }

    strEdit_treefile = strEdit_treefile.trimmed();
    if( strEdit_treefile.isEmpty() ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Tree file's path can not be empty."));
        msgBox.setInformativeText(tr("Please enter <i>treefile</i>."));
        msgBox.exec();
        return false;
    }
    if( strEdit_treefile.length() >= MAX_PATH ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Full name of tree file is too long."));
        msgBox.setInformativeText(tr("Please check <i>treefile</i>."));
        msgBox.exec();
        return false;
    }
    if( strEdit_treefile.contains(' ') ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("There should be no space characters in tree file's path."));
        msgBox.setInformativeText(tr("Please check <i>treefile</i>."));
        msgBox.exec();
        return false;
    }
    if( QDir::isAbsolutePath(strEdit_treefile) || mFileName.isEmpty() ) {
        if( !QDir::isAbsolutePath(strEdit_treefile) || !QFileInfo(strEdit_treefile).isFile() ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Failed to open tree file."));
            msgBox.setInformativeText(tr("Please check <i>treefile</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "      treefile = " << strEdit_treefile << "\n";
    }
    else {
        QString treefilePath = QDir::cleanPath( QFileInfo(mFileName).absoluteDir().absoluteFilePath(strEdit_treefile) );
        if( !QFileInfo(treefilePath).isFile() ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Failed to open tree file."));
            msgBox.setInformativeText(tr("Please check <i>treefile</i>."));
            msgBox.exec();
            return false;
        }
        if( treefilePath.length() >= MAX_PATH ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Absolute path of tree file is too long."));
            msgBox.setInformativeText(QDir::toNativeSeparators(treefilePath));
            msgBox.exec();
            return false;
        }
        if( treefilePath.contains(' ') ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Absolute path of tree file can not have space characters."));
            msgBox.setInformativeText(QDir::toNativeSeparators(treefilePath));
            msgBox.exec();
            return false;
        }
        ctlOut << "      treefile = " << treefilePath << "\n";
    }

    strEdit_outfile_Name = strEdit_outfile_Name.trimmed();
    if( strEdit_outfile_Name.isEmpty() ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Output filename can not be empty."));
        msgBox.setInformativeText(tr("Please enter <i>outfile name</i>."));
        msgBox.exec();
        return false;
    }
    if( strEdit_outfile_Name.length() >= MAX_PATH ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Name of output file is too long."));
        msgBox.setInformativeText(tr("Please check <i>outfile name</i>."));
        msgBox.exec();
        return false;
    }
    if( strEdit_outfile_Name.contains(' ') ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("There should be no space characters in output filename."));
        msgBox.setInformativeText(tr("Please check <i>outfile name</i>."));
        msgBox.exec();
        return false;
    }
//    if( !strEdit_outfile_Name.contains(QRegExp("^ *[^ ]+ *$")) ) {
//        return false;
//    }
    ctlOut << "       outfile = " << strEdit_outfile_Name << "\n\n";

    ctlOut << "         noisy = " << '3'/*nComb_noisy*/ << "\n";
    ctlOut << "         ndata = " << nEdit_ndata << "\n";
    if(nComb_usedata < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("usedata option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>usedata</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "       usedata = " << nComb_usedata << "\n";
    if(nComb_clock < 1) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("clock option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>clock</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "         clock = " << nComb_clock << "\n";

    ctlOut << "       TipDate = " << nEdit_TipDate << ' ' << nEdit_TipDate_TimeUnit << "\n";

    strEdit_RootAge = strEdit_RootAge.trimmed();
    if( strEdit_RootAge.isEmpty() ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("RootAge can not be empty."));
        msgBox.setInformativeText(tr("Please enter <i>RootAge</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "       RootAge = " << strEdit_RootAge << "\n\n";

    if(nComb_model < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("model option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>model</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "         model = " << nComb_model << "\n";
    ctlOut << "         alpha = " << nEdit_alpha << "\n";
    ctlOut << "         ncatG = " << nEdit_ncatG << "\n\n";

    ctlOut << "     cleandata = " << char('0' + bButt_cleandata) << "\n\n";

    ctlOut << "       BDparas = " << nEdit_BDparas[0] << ' ' << nEdit_BDparas[1] << ' ' << nEdit_BDparas[2] << ' ' << nEdit_BDparas[3] << "\n";
    ctlOut << "   kappa_gamma = " << nEdit_kappa_gamma[0] << ' ' << nEdit_kappa_gamma[1] << "\n";
    ctlOut << "   alpha_gamma = " << nEdit_alpha_gamma[0] << ' ' << nEdit_alpha_gamma[1] << "\n\n";

    if(nComb_rgeneprior < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("rgene prior option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>rgene prior</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "   rgene_gamma = " << nEdit_rgene_para[0] << ' ' << nEdit_rgene_para[1] << ' ' << nEdit_rgene_para[2] << ' ' << nComb_rgeneprior << "\n";
    ctlOut << "  sigma2_gamma = " << nEdit_sigma2_para[0] << ' ' << nEdit_sigma2_para[1] << ' ' << nEdit_sigma2_para[2] << "\n\n";

    if( compareVersion("4.9") < 0 )
        ctlOut << "      finetune = " << char('0' + bButt_autoAdjustFinetune) << ": " << nEdit_finetune[0] << ' ' << nEdit_finetune[1] << ' ' << nEdit_finetune[2] << ' ' << nEdit_finetune[3] << ' ' << nEdit_finetune[4] << ' ' << "\n\n";

    ctlOut << "         print = " << char('0' + bButt_print) << "\n";
    ctlOut << "        burnin = " << nEdit_burnin << "\n";
    ctlOut << "      sampfreq = " << nEdit_sampfreq << "\n";
    ctlOut << "       nsample = " << nEdit_nsample << "\n\n";

    ctlOut.flush();

    return true;
}

bool MCMCTreeEngine::onNewDocument()
{
    if( !Engine::onNewDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool MCMCTreeEngine::onCloseDocument()
{
    if( !Engine::onCloseDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool MCMCTreeEngine::doOpen(const QString& fileName)
{
    return readFile(fileName);
}

bool MCMCTreeEngine::doSave(const QString& fileName)
{
    return writeFile(fileName);
}

bool MCMCTreeEngine::run()
{
    if( !Engine::run() )
        return false;

#ifdef STREAMLINED_OUTPUT
    streamlineFlag = 0;
#endif

    strEdit_outfile_Location = strEdit_outfile_Location.trimmed();
    if( strEdit_outfile_Location.isEmpty() ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Output path can not be empty."));
        msgBox.setInformativeText(tr("Please enter <i>location</i>."));
        msgBox.exec();
        return false;
    }
    if( strEdit_outfile_Location.length() >= MAX_PATH ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Output path is too long."));
        msgBox.setInformativeText(tr("Please check <i>location</i>."));
        msgBox.exec();
        return false;
    }

    QString workingDirectory = strEdit_outfile_Location;

    if( !QDir::isAbsolutePath(workingDirectory) && !mFileName.isEmpty() ) {
        workingDirectory = QFileInfo(mFileName).absolutePath() + "/" + workingDirectory;
    }

    workingDirectory = QDir::cleanPath(workingDirectory);

    if( !QDir::isAbsolutePath(workingDirectory) || !QDir(workingDirectory).exists() ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Output path does not exist."));
        msgBox.setInformativeText(tr("Please check <i>location</i>."));
        msgBox.exec();
        return false;
    }

    if( workingDirectory.length() >= MAX_PATH ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Absolute path of output is too long."));
        msgBox.setInformativeText(QDir::toNativeSeparators(workingDirectory));
        msgBox.exec();
        return false;
    }

    QString ctlFileName = (workingDirectory == "/")? "/mcmctree.ctl.tmp" : QDir::cleanPath( workingDirectory + "/mcmctree.ctl.tmp" );
    if( !createTmpCtlFile(ctlFileName)) return false;

    if( !workingDirectory.isEmpty() ) //always true
        process->setWorkingDirectory(workingDirectory);

    QStringList arguments;
    arguments << "mcmctree.ctl.tmp";
    process->start(mPath, arguments);

    return true;
}

#ifdef STREAMLINED_OUTPUT
void MCMCTreeEngine::onReadyReadStandardOutput()
{
    QByteArray newData = process->readAllStandardOutput();
    newData.prepend(buffer);
    int newDataSize = newData.size();

    int pos1 = newData.lastIndexOf('\r');
    int pos2 = newData.lastIndexOf('\n');
    int pos = (pos1 > pos2)? pos1 : pos2;

    buffer = newData.right(newDataSize - (pos + 1));
    newData.remove(pos + 1, newDataSize);
    if(newData.isEmpty()) return;

    if(streamlineFlag == 2) {
#ifdef MYDEBUG
        fwrite(newData.data(), newData.size(), 1, mydebugfp);
        fflush(mydebugfp);
#endif
#if defined(Q_OS_WIN32)
        if(newData.endsWith("\r\n"))
            newData.chop(1);
#endif
        newData.chop(1);
        output(newData);
    }

    if(streamlineFlag == 0) {
        const char marker[] = "alpha)";
        pos = newData.indexOf(marker);
        if(pos != -1) {
            //split newData = prefixData + newData
            int i = pos + sizeof(marker)/sizeof(char) - 1;
            for(; i<newData.size(); i++)
                if(newData[i] != '\r' && newData[i] != '\n') break;
            QByteArray prefixData = newData.left(i);
            newData.remove(0, i);
#ifdef MYDEBUG
            fwrite(prefixData.data(), prefixData.size(), 1, mydebugfp);
            fflush(mydebugfp);
#endif
#if defined(Q_OS_WIN32)
            if(prefixData.endsWith("\r\n"))
                prefixData.chop(1);
#endif
            prefixData.chop(1);
            output(prefixData);
            streamlineFlag = 1;
        }
        else {
#ifdef MYDEBUG
            fwrite(newData.data(), newData.size(), 1, mydebugfp);
            fflush(mydebugfp);
#endif
#if defined(Q_OS_WIN32)
            if(newData.endsWith("\r\n"))
                newData.chop(1);
#endif
            newData.chop(1);
            output(newData);
        }
    }

    if(streamlineFlag == 1) {
        QByteArray suffixData;
        pos = newData.indexOf("Time used");
        if(pos != -1) {
            //split newData = newData + suffixData
            newDataSize = newData.size();
            suffixData = newData.right(newDataSize - pos);
            newData.remove(pos, newDataSize);
            streamlineFlag = 2;
        }

        //streamline newData
#ifdef MYDEBUG
        fwrite(newData.data(), newData.size(), 1, mydebugfp);
        fflush(mydebugfp);
#endif
        char* string = newData.data();

#if defined(Q_OS_WIN32)
        for(char* index = string; *index != '\0'; index++) {
            if( *index == '\r' ) {
                if( *(index + 1) == '\n' ) {
                    *index = '\0';
                    output(string);
                    index++;
                }
                string = index + 1;
            }
        }
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
        for(char* index = string; *index != '\0'; index++) {
            if( *index == '\r' ) {
                string = index + 1;
            }
            else if( *index == '\n' ) {
                *index = '\0';
                output(string);
                string = index + 1;
            }
        }
#else
#warning Nothing to do on current platform
#endif

        if(streamlineFlag == 2) {
#ifdef MYDEBUG
            fwrite(suffixData.data(), suffixData.size(), 1, mydebugfp);
            fflush(mydebugfp);
#endif
#if defined(Q_OS_WIN32)
            if(suffixData.endsWith("\r\n"))
                suffixData.chop(1);
#endif
            suffixData.chop(1);
            output(suffixData);
        }
    }
}
#endif // STREAMLINED_OUTPUT
