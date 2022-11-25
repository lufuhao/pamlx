//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "codemlengine.h"
#include "def.h"
#include "cstylecodemlctlloader.h"
#include <QDir>
#include <QTextStream>
#include <QMessageBox>

CodemlEngine::CodemlEngine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent) :
    Engine(aOutputWindow, msec1, msec2, parent)
{
    setPath(pamlPath);

    for(int i=0; i<maxNSsitesModels; i++)
        if((i >= 0 && i <= 13) || i == 22)
            triList_nsmodels[i] = False;
        else
            triList_nsmodels[i] = Null;

    setDefaultData();
}

bool CodemlEngine::setPath(const QString& pamlPath)
{
#ifdef Q_OS_WIN32
    mPath = pamlPath + "/bin/codeml.exe";
#else
    mPath = pamlPath + "/bin/codeml";
#endif

    if( QFileInfo(mPath).isFile() )
        statusOk = true;
    else
        statusOk = false;

    return statusOk;
}

void CodemlEngine::refreshFileContent()
{
    if(!modified && !mFileName.isEmpty()) return;

    QString tmpContent = mFileContent;
    mFileContent.clear();

    QTextStream in(&tmpContent, QIODevice::ReadOnly);
    QTextStream out(&mFileContent, QIODevice::WriteOnly);

    QStringList optionList;
    optionList << "seqfile"
               << "treefile"
               << "outfile"
               << "seqtype"
//               << "noisy"
               << "cleandata"
               << "runmode"
               << "method"
               << "clock"
//               << "TipDate"
               << "getSE"
               << "RateAncestor"
               << "CodonFreq"
               << "estFreq"
//               << "verbose"
               << "model"
//               << "hkyREV"
               << "aaDist"
               << "aaRatefile"
               << "NSsites"
//               << "NShmm"
               << "icode"
               << "Mgene"
               << "fix_kappa"
               << "kappa"
               << "fix_omega"
               << "omega"
               << "fix_alpha"
               << "alpha"
               << "Malpha"
               << "ncatG"
//               << "fix_rho"
//               << "rho"
               << "ndata"
//               << "bootstrap"
               << "Small_Diff"
               << "fix_blength";

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
                    strEdit_seqfile = strEdit_seqfile.trimmed();
                    out << strEdit_seqfile;
                    break;
                case 1:
                    strEdit_treefile = strEdit_treefile.trimmed();
                    out << strEdit_treefile;
                    break;
                case 2:
                    strEdit_outfile_Location = strEdit_outfile_Location.trimmed();
                    strEdit_outfile_Name = strEdit_outfile_Name.trimmed();
                    if(strEdit_outfile_Location.isEmpty())
                        out << strEdit_outfile_Name;
                    else if(strEdit_outfile_Location == "/" || strEdit_outfile_Location == "\\")
                        out << QDir::fromNativeSeparators(QDir::cleanPath( "/" + strEdit_outfile_Name ));
                    else
                        out << QDir::fromNativeSeparators(QDir::cleanPath( strEdit_outfile_Location + "/" + strEdit_outfile_Name ));
                    break;
                case 3:
                    if(nComb_seqtype >= 1)
                        out << nComb_seqtype;
                    break;
                case 4:
                    out << char('0' + bButt_cleandata);
                    break;
                case 5:
                    if(nComb_runmode >= 0 || nComb_runmode == -2)
                        out << nComb_runmode;
                    else if(nComb_runmode == -3)
                        out << nComb_runmode << ' ' << nEdit_t_gamma[0] << ' ' << nEdit_t_gamma[1] << ' ' << nEdit_w_gamma[0] << ' ' << nEdit_w_gamma[1];
                    break;
                case 6:
                    if(nComb_method >= 0)
                        out << nComb_method;
                    break;
                case 7:
                    if(nComb_clock >= 0)
                        out << nComb_clock;
                    break;
                case 8:
                    out << char('0' + bButt_getSE);
                    break;
                case 9:
                    out << char('0' + bButt_RateAncestor);
                    break;
                case 10:
                    if(nComb_CodonFreq >= 0)
                        out << nComb_CodonFreq;
                    break;
                case 11:
                    if(nComb_estFreq >= 0)
                        out << nComb_estFreq;
                    break;
                case 12:
                    if(nComb_model >= 0)
                        out << nComb_model;
                    break;
                case 13:
                    if(nComb_aaDist >= -6)
                        out << nComb_aaDist;
                    break;
                case 14:
                    strEdit_aaRatefile = strEdit_aaRatefile.trimmed();
                    out << strEdit_aaRatefile;
                    break;
                case 15:
                    {
                    bool isNSsitesEmpty = true;
                    for(int i=0; i<maxNSsitesModels; i++)
                        if(triList_nsmodels[i] == True) {
                            out << i << ' ';
                            isNSsitesEmpty = false;
                        }
                    if(isNSsitesEmpty && nComb_seqtype != 1)
                        out << '0' << ' ';
                    }
                    break;
                case 16:
                    if(nComb_icode >= 0)
                        out << nComb_icode;
                    break;
                case 17:
                    if(nComb_Mgene >= 0)
                        out << nComb_Mgene;
                    break;
                case 18:
                    out << char('0' + bButt_fix_kappa);
                    break;
                case 19:
                    out << nEdit_kappa;
                    break;
                case 20:
                    out << char('0' + bButt_fix_omega);
                    break;
                case 21:
                    out << nEdit_omega;
                    break;
                case 22:
                    out << char('0' + bButt_fix_alpha);
                    break;
                case 23:
                    out << nEdit_alpha;
                    break;
                case 24:
                    out << char('0' + bButt_Malpha);
                    break;
                case 25:
                    out << nEdit_ncatG;
                    break;
                case 26:
                    out << nEdit_ndata;
                    break;
                case 27:
                    out << strEdit_Small_Diff;
                    break;
                case 28:
                    if(nComb_fix_blength >= -1)
                        out << nComb_fix_blength;
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
            if(nComb_seqtype == 1 && i == 14) continue;
            if((nComb_seqtype == 2 ||nComb_seqtype == 3) && (i == 10 || i == 11 || i == 15 || i == 16 || (i >= 18 && i <= 21))) continue;

            switch(i)
            {
            case 0:
                strEdit_seqfile = strEdit_seqfile.trimmed();
                out << "      seqfile = " << strEdit_seqfile << "\n";
                break;
            case 1:
                strEdit_treefile = strEdit_treefile.trimmed();
                out << "     treefile = " << strEdit_treefile << "\n";
                break;
            case 2:
                strEdit_outfile_Location = strEdit_outfile_Location.trimmed();
                strEdit_outfile_Name = strEdit_outfile_Name.trimmed();
                if(strEdit_outfile_Location.isEmpty())
                    out << "      outfile = " << strEdit_outfile_Name << "\n\n";
                else if(strEdit_outfile_Location == "/" || strEdit_outfile_Location == "\\")
                    out << "      outfile = " << QDir::fromNativeSeparators(QDir::cleanPath( "/" + strEdit_outfile_Name )) << "\n\n";
                else
                    out << "      outfile = " << QDir::fromNativeSeparators(QDir::cleanPath( strEdit_outfile_Location + "/" + strEdit_outfile_Name )) << "\n\n";
                break;
            case 3:
                if(nComb_seqtype >= 1)
                    out << "      seqtype = " << nComb_seqtype << "\n";
                break;
            case 4:
                out << "    cleandata = " << char('0' + bButt_cleandata) << "\n\n";
                break;
            case 5:
                if(nComb_runmode >= 0 || nComb_runmode == -2)
                    out << "      runmode = " << nComb_runmode << "\n\n";
                else if(nComb_runmode == -3)
                    out << "      runmode = " << nComb_runmode << ' ' << nEdit_t_gamma[0] << ' ' << nEdit_t_gamma[1] << ' ' << nEdit_w_gamma[0] << ' ' << nEdit_w_gamma[1] << "\n\n";
                break;
            case 6:
                if(nComb_method >= 0)
                    out << "       method = " << nComb_method << "\n";
                break;
            case 7:
                if(nComb_clock >= 0)
                    out << "        clock = " << nComb_clock << "\n\n";
                break;
            case 8:
                out << "        getSE = " << char('0' + bButt_getSE) << "\n";
                break;
            case 9:
                out << " RateAncestor = " << char('0' + bButt_RateAncestor) << "\n\n";
                break;
            case 10:
                if(nComb_CodonFreq >= 0)
                    out << "    CodonFreq = " << nComb_CodonFreq << "\n";
                break;
            case 11:
                if(nComb_estFreq >= 0)
                    out << "      estFreq = " << nComb_estFreq << "\n\n";
                break;
            case 12:
                if(nComb_model >= 0)
                    out << "        model = " << nComb_model << "\n\n";
                break;
            case 13:
                if(nComb_aaDist >= -6)
                    out << "       aaDist = " << nComb_aaDist << "\n";
                break;
            case 14:
                strEdit_aaRatefile = strEdit_aaRatefile.trimmed();
                out << "   aaRatefile = " << strEdit_aaRatefile << "\n\n";
                break;
            case 15:
                out << "      NSsites = ";
                for(int i=0; i<maxNSsitesModels; i++)
                    if(triList_nsmodels[i] == True) out << i << ' ';
                out << "\n\n";
                break;
            case 16:
                if(nComb_icode >= 0)
                    out << "        icode = " << nComb_icode << "\n";
                break;
            case 17:
                if(nComb_Mgene >= 0)
                    out << "        Mgene = " << nComb_Mgene << "\n\n";
                break;
            case 18:
                out << "    fix_kappa = " << char('0' + bButt_fix_kappa) << "\n";
                break;
            case 19:
                out << "        kappa = " << nEdit_kappa << "\n";
                break;
            case 20:
                out << "    fix_omega = " << char('0' + bButt_fix_omega) << "\n";
                break;
            case 21:
                out << "        omega = " << nEdit_omega << "\n\n";
                break;
            case 22:
                out << "    fix_alpha = " << char('0' + bButt_fix_alpha) << "\n";
                break;
            case 23:
                out << "        alpha = " << nEdit_alpha << "\n";
                break;
            case 24:
                out << "       Malpha = " << char('0' + bButt_Malpha) << "\n";
                break;
            case 25:
                out << "        ncatG = " << nEdit_ncatG << "\n\n";
                break;
            case 26:
                out << "        ndata = " << nEdit_ndata << "\n";
                break;
            case 27:
                out << "   Small_Diff = " << strEdit_Small_Diff << "\n";
                break;
            case 28:
                if(nComb_fix_blength >= -1)
                    out << "  fix_blength = " << nComb_fix_blength << "\n";
                break;
            default:;
            }
        }
    }

    delete used;
    used = NULL;
}

void CodemlEngine::setDefaultData()
{
    strEdit_seqfile = "";
    strEdit_treefile = "";
    strEdit_outfile_Name = "mlc";
    strEdit_outfile_Location = "";
    nComb_seqtype = 2;
    bButt_cleandata = false;
    nComb_runmode = 0;
    nEdit_t_gamma[0] = 1.1;
    nEdit_t_gamma[1] = 1.1;
    nEdit_w_gamma[0] = 1.1;
    nEdit_w_gamma[1] = 2.2;
    nComb_method = 0;
    nComb_clock = 0;
    bButt_getSE = true;
    bButt_RateAncestor = false;
    nComb_CodonFreq = 0;///1
    nComb_estFreq = 0;
    nComb_model = 0;
    nComb_aaDist = 0;
    strEdit_aaRatefile = "";///2
    triList_nsmodels[0] = True;
    for(int i=1; i<maxNSsitesModels; i++)
        if(triList_nsmodels[i] != Null)
            triList_nsmodels[i] = False;
    nComb_icode = 0;///1
    nComb_Mgene = 0;
    bButt_fix_kappa = false;///1
    nEdit_kappa = 2;///1
    bButt_fix_omega = false;///1
    nEdit_omega = 0.4;///1
    bButt_fix_alpha = true;
    nEdit_alpha = 0;
    bButt_Malpha = false;
    nEdit_ncatG = 5;
    nEdit_ndata = 1;
    strEdit_Small_Diff = "5e-7";
    nComb_fix_blength = 0;
}

bool CodemlEngine::readFile(const QString& fileName)
{
    CStyleCodemlCtlLoader* ctlLoader = new CStyleCodemlCtlLoader(this);
    QFile ctlFile(fileName);

    ctlLoader->com.seqtype = 2;
    ctlLoader->com.cleandata = 0;
    ctlLoader->com.runmode = 0;
    ctlLoader->com.hyperpar[0] = 1.1;
    ctlLoader->com.hyperpar[1] = 1.1;
    ctlLoader->com.hyperpar[2] = 1.1;
    ctlLoader->com.hyperpar[3] = 2.2;
    ctlLoader->com.method = 0;
    ctlLoader->com.clock = 0;
    ctlLoader->com.getSE = 0;
    ctlLoader->com.print = 0;
    ctlLoader->com.codonf = 0;
    ctlLoader->com.npi = 0;
    ctlLoader->com.model = 3;
    ctlLoader->com.aaDist = 0;
    ctlLoader->nnsmodels = 1;
    ctlLoader->nsmodels[0] = 0;
    ctlLoader->com.icode = 0;
    ctlLoader->com.Mgene = 0;
    ctlLoader->com.fix_kappa = 0;
    ctlLoader->com.kappa = 1;
    ctlLoader->com.fix_omega = 0;
    ctlLoader->com.omega = 2.1;
    ctlLoader->com.fix_alpha = 1;
    ctlLoader->com.alpha = 0;
    ctlLoader->com.nalpha = 0;
    ctlLoader->com.ncatG = 4;
    ctlLoader->com.ndata = 1;
    ctlLoader->Small_Diff = 5e-7;
    ctlLoader->com.fix_blength = 0;

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

    strEdit_seqfile = ctlLoader->seqfile.section('*', 0, 0).trimmed();
    strEdit_treefile = ctlLoader->treefile.section('*', 0, 0).trimmed();
    QString outfile = ctlLoader->outfile.section('*', 0, 0).trimmed();
    strEdit_outfile_Name = QFileInfo(outfile).fileName();
    strEdit_outfile_Location = QDir::toNativeSeparators(QFileInfo(outfile).path());
    if(strEdit_outfile_Location == ".") strEdit_outfile_Location = QDir::toNativeSeparators("./");
    if(strEdit_outfile_Location == "..") strEdit_outfile_Location = QDir::toNativeSeparators("../");
    nComb_seqtype = ctlLoader->com.seqtype;
    bButt_cleandata = ctlLoader->com.cleandata;
    nComb_runmode = ctlLoader->com.runmode;
    nEdit_t_gamma[0] = ctlLoader->com.hyperpar[0];
    nEdit_t_gamma[1] = ctlLoader->com.hyperpar[1];
    nEdit_w_gamma[0] = ctlLoader->com.hyperpar[2];
    nEdit_w_gamma[1] = ctlLoader->com.hyperpar[3];
    nComb_method = ctlLoader->com.method;
    nComb_clock = ctlLoader->com.clock;
    bButt_getSE = ctlLoader->com.getSE;
    bButt_RateAncestor = ctlLoader->com.print;
    nComb_CodonFreq = ctlLoader->com.codonf;
    nComb_estFreq = ctlLoader->com.npi;
    nComb_model = ctlLoader->com.model;
    nComb_aaDist = ctlLoader->com.aaDist;
    strEdit_aaRatefile = ctlLoader->aaRatefile.section('*', 0, 0).trimmed();
    for(int i=0; i<maxNSsitesModels; i++)
        if(triList_nsmodels[i] != Null)
            triList_nsmodels[i] = False;
    for(int i=0; i<ctlLoader->nnsmodels; i++) {
        int index = ctlLoader->nsmodels[i];
        if( index >= 0 && index < maxNSsitesModels && triList_nsmodels[index] != Null )
            triList_nsmodels[index] = True;
    }
    nComb_icode = ctlLoader->com.icode;
    nComb_Mgene = ctlLoader->com.Mgene;
    bButt_fix_kappa = ctlLoader->com.fix_kappa;
    nEdit_kappa = ctlLoader->com.kappa;
    bButt_fix_omega = ctlLoader->com.fix_omega;
    nEdit_omega = ctlLoader->com.omega;
    bButt_fix_alpha = ctlLoader->com.fix_alpha;
    nEdit_alpha = ctlLoader->com.alpha;
    bButt_Malpha = ctlLoader->com.nalpha;
    nEdit_ncatG = ctlLoader->com.ncatG;
    nEdit_ndata = ctlLoader->com.ndata;
    strEdit_Small_Diff = QString::number(ctlLoader->Small_Diff);
    nComb_fix_blength = ctlLoader->com.fix_blength;

    delete ctlLoader;

    return true;
}

bool CodemlEngine::writeFile(const QString& fileName)
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

bool CodemlEngine::createTmpCtlFile(const QString& fileName)
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

    if(nComb_seqtype == 1) {
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
            ctlOut << "      seqfile = " << strEdit_seqfile << "\n";
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
            ctlOut << "      seqfile = " << seqfilePath << "\n";
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
            ctlOut << "     treefile = " << strEdit_treefile << "\n";
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
            ctlOut << "     treefile = " << treefilePath << "\n";
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
        ctlOut << "      outfile = " << strEdit_outfile_Name << "\n\n";

        ctlOut << "        noisy = " << '4'/*nComb_noisy*/ << "\n";
        ctlOut << "      verbose = " << '1'/*nComb_verbose*/ << "\n";
        if(nComb_runmode < 0 && nComb_runmode != -2 && nComb_runmode != -3) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("runmode option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>runmode</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "      runmode = " << nComb_runmode;
        if(nComb_runmode == -3)
            ctlOut << ' ' << nEdit_t_gamma[0] << ' ' << nEdit_t_gamma[1] << ' ' << nEdit_w_gamma[0] << ' ' << nEdit_w_gamma[1];
        ctlOut << "\n\n\n";

        ctlOut << "      seqtype = " << nComb_seqtype << "\n";
        if(nComb_CodonFreq < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("CodonFreq option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>CodonFreq</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "    CodonFreq = " << nComb_CodonFreq << "\n";
        if(nComb_estFreq < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("estFreq option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>estFreq</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "      estFreq = " << nComb_estFreq << "\n\n";

        ctlOut << "        ndata = " << nEdit_ndata << "\n";
        if(nComb_clock < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("clock option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>clock</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "        clock = " << nComb_clock << "\n";
        if(nComb_aaDist < -6) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("aaDist option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>aaDist</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "       aaDist = " << nComb_aaDist << "\n\n\n\n";

        if(nComb_model < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("model option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>model</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "        model = " << nComb_model << "\n\n\n\n\n\n\n";

        ctlOut << "      NSsites = ";
        bool isNSsitesEmpty = true;
        for(int i=0; i<maxNSsitesModels; i++)
            if(triList_nsmodels[i] == True) {
                ctlOut << i << ' ';
                isNSsitesEmpty = false;
            }
        if(isNSsitesEmpty) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("NSsites option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>NSsites</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "\n\n\n\n\n";

        if(nComb_icode < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("icode option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>icode</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "        icode = " << nComb_icode << "\n";
        if(nComb_Mgene < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Mgene option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>Mgene</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "        Mgene = " << nComb_Mgene << "\n\n\n\n";

        ctlOut << "    fix_kappa = " << char('0' + bButt_fix_kappa) << "\n";
        ctlOut << "        kappa = " << nEdit_kappa << "\n";
        ctlOut << "    fix_omega = " << char('0' + bButt_fix_omega) << "\n";
        ctlOut << "        omega = " << nEdit_omega << "\n\n";

        ctlOut << "    fix_alpha = " << char('0' + bButt_fix_alpha) << "\n";
        ctlOut << "        alpha = " << nEdit_alpha << "\n";
        ctlOut << "       Malpha = " << char('0' + bButt_Malpha) << "\n";
        ctlOut << "        ncatG = " << nEdit_ncatG << "\n\n";

        ctlOut << "        getSE = " << char('0' + bButt_getSE) << "\n";
        ctlOut << " RateAncestor = " << char('0' + bButt_RateAncestor) << "\n\n";

        ctlOut << "   Small_Diff = " << strEdit_Small_Diff << "\n";
        ctlOut << "    cleandata = " << char('0' + bButt_cleandata) << "\n";
        if(nComb_fix_blength < -1) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("fix_blength option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>fix_blength</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "  fix_blength = " << nComb_fix_blength << "\n";
        if(nComb_method < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("method option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>optimization method</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "       method = " << nComb_method << "\n";
    }

    else if(nComb_seqtype == 2 || nComb_seqtype == 3) {
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
            ctlOut << "      seqfile = " << strEdit_seqfile << "\n";
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
            ctlOut << "      seqfile = " << seqfilePath << "\n";
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
            ctlOut << "     treefile = " << strEdit_treefile << "\n\n";
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
            ctlOut << "     treefile = " << treefilePath << "\n\n";
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
        ctlOut << "      outfile = " << strEdit_outfile_Name << "\n";

        ctlOut << "        noisy = " << '4'/*nComb_noisy*/ << "\n";
        ctlOut << "      verbose = " << '1'/*nComb_verbose*/ << "\n";
        if(nComb_runmode < 0 && nComb_runmode != -2 && nComb_runmode != -3) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("runmode option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>runmode</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "      runmode = " << nComb_runmode;
        if(nComb_runmode == -3)
            ctlOut << ' ' << nEdit_t_gamma[0] << ' ' << nEdit_t_gamma[1] << ' ' << nEdit_w_gamma[0] << ' ' << nEdit_w_gamma[1];
        ctlOut << "\n\n\n";

        ctlOut << "      seqtype = " << nComb_seqtype << "\n";
        if(nComb_aaDist < -6) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("aaDist option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>aaDist</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "       aaDist = " << nComb_aaDist << "\n";

        strEdit_aaRatefile = strEdit_aaRatefile.trimmed();
        if( nComb_model == 2 || nComb_model == 3 ) {
            if( strEdit_aaRatefile.isEmpty() ) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Warning"));
                msgBox.setText(tr("Amino acid rate file's path can not be empty."));
                msgBox.setInformativeText(tr("Please enter <i>amino acid rate file</i>."));
                msgBox.exec();
                return false;
            }
            if( strEdit_aaRatefile.length() >= 90/*not bigger than 96 in codeml.c*/ ) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Warning"));
                msgBox.setText(tr("Full name of amino acid rate file is too long."));
                msgBox.setInformativeText(tr("Please check <i>amino acid rate file</i>."));
                msgBox.exec();
                return false;
            }
            if( strEdit_aaRatefile.contains(' ') ) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Warning"));
                msgBox.setText(tr("There should be no space characters in amino acid rate file's path."));
                msgBox.setInformativeText(tr("Please check <i>amino acid rate file</i>."));
                msgBox.exec();
                return false;
            }
            if( QDir::isAbsolutePath(strEdit_aaRatefile) || mFileName.isEmpty() ) {
                if( !QDir::isAbsolutePath(strEdit_aaRatefile) || !QFileInfo(strEdit_aaRatefile).isFile() ) {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle(tr("Warning"));
                    msgBox.setText(tr("Failed to open amino acid rate file."));
                    msgBox.setInformativeText(tr("Please check <i>amino acid rate file</i>."));
                    msgBox.exec();
                    return false;
                }
                ctlOut << "   aaRatefile = " << strEdit_aaRatefile << "\n\n\n";
            }
            else {
                QString ratefilePath = QDir::cleanPath( QFileInfo(mFileName).absoluteDir().absoluteFilePath(strEdit_aaRatefile) );
                if( !QFileInfo(ratefilePath).isFile() ) {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle(tr("Warning"));
                    msgBox.setText(tr("Failed to open amino acid rate file."));
                    msgBox.setInformativeText(tr("Please check <i>amino acid rate file</i>."));
                    msgBox.exec();
                    return false;
                }
                if( ratefilePath.length() >= 90/*not bigger than 96 in codeml.c*/ ) {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle(tr("Warning"));
                    msgBox.setText(tr("Absolute path of amino acid rate file is too long."));
                    msgBox.setInformativeText(QDir::toNativeSeparators(ratefilePath));
                    msgBox.exec();
                    return false;
                }
                if( ratefilePath.contains(' ') ) {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle(tr("Warning"));
                    msgBox.setText(tr("Absolute path of amino acid rate file can not have space characters."));
                    msgBox.setInformativeText(QDir::toNativeSeparators(ratefilePath));
                    msgBox.exec();
                    return false;
                }
                ctlOut << "   aaRatefile = " << ratefilePath << "\n\n\n";
            }
        }
        else {
            ctlOut << "   aaRatefile = " << strEdit_aaRatefile << "\n\n\n";
        }

        if(nComb_model < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("model option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>model</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "        model = " << nComb_model << "\n\n";

        if(nComb_Mgene < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Mgene option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>Mgene</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "        Mgene = " << nComb_Mgene << "\n\n";

        ctlOut << "    fix_alpha = " << char('0' + bButt_fix_alpha) << "\n";
        ctlOut << "        alpha = " << nEdit_alpha << "\n";
        ctlOut << "       Malpha = " << char('0' + bButt_Malpha) << "\n";
        ctlOut << "        ncatG = " << nEdit_ncatG << "\n\n";

        if(nComb_clock < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("clock option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>clock</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "        clock = " << nComb_clock << "\n";
        ctlOut << "        getSE = " << char('0' + bButt_getSE) << "\n";
        ctlOut << " RateAncestor = " << char('0' + bButt_RateAncestor) << "\n\n\n\n\n\n\n\n";

        ctlOut << "   Small_Diff = " << strEdit_Small_Diff << "\n";
        ctlOut << "    cleandata = " << char('0' + bButt_cleandata) << "\n";
        if(nComb_fix_blength < -1) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("fix_blength option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>fix_blength</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "  fix_blength = " << nComb_fix_blength << "\n";
        ctlOut << "        ndata = " << nEdit_ndata << "\n";
        if(nComb_method < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("method option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>optimization method</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << "       method = " << nComb_method << "\n";
    }

    else {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("seqtype option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>seqtype</i>."));
        msgBox.exec();
        return false;
    }

    ctlOut.flush();

    return true;
}

bool CodemlEngine::onNewDocument()
{
    if( !Engine::onNewDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool CodemlEngine::onCloseDocument()
{
    if( !Engine::onCloseDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool CodemlEngine::doOpen(const QString& fileName)
{
    return readFile(fileName);
}

bool CodemlEngine::doSave(const QString& fileName)
{
    return writeFile(fileName);
}

bool CodemlEngine::run()
{
    if( !Engine::run() )
        return false;

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

    QString ctlFileName = (workingDirectory == "/")? "/codeml.ctl.tmp" : QDir::cleanPath( workingDirectory + "/codeml.ctl.tmp" );
    if( !createTmpCtlFile(ctlFileName) ) return false;

    if( !workingDirectory.isEmpty() ) //always true
        process->setWorkingDirectory(workingDirectory);

    QStringList arguments;
    arguments << "codeml.ctl.tmp";
    process->start(mPath, arguments);

    return true;
}
