//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "basemlengine.h"
#include "def.h"
#include "cstylebasemlctlloader.h"
#include <QDir>
#include <QTextStream>
#include <QtWidgets/QMessageBox>

BasemlEngine::BasemlEngine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent) :
    Engine(aOutputWindow, msec1, msec2, parent)
{
    setPath(pamlPath);
    setDefaultData();
}

bool BasemlEngine::setPath(const QString& pamlPath)
{
#ifdef Q_OS_WIN32
    mPath = pamlPath + "/bin/baseml.exe";
#else
    mPath = pamlPath + "/bin/baseml";
#endif

    if( QFileInfo(mPath).isFile() )
        statusOk = true;
    else
        statusOk = false;

    return statusOk;
}

void BasemlEngine::refreshFileContent()
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
//               << "noisy"
               << "cleandata"
//               << "verbose"
               << "runmode"
               << "method"
               << "clock"
               << "TipDate"
//               << "fix_rgene"
               << "Mgene"
               << "nhomo"
               << "getSE"
               << "RateAncestor"
               << "model"
               << "fix_kappa"
               << "kappa"
               << "fix_alpha"
               << "alpha"
               << "Malpha"
               << "ncatG"
               << "fix_rho"
               << "rho"
               << "nparK"
               << "ndata"
//               << "bootstrap"
               << "Small_Diff"
               << "icode"
               << "fix_blength";
//               << "seqtype";

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
                if(i == 24 && nComb_icode == -1) {
                    out << "* " << line << "\n";
                    used[i] = true;
                    break;
                }

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
                    out << char('0' + bButt_cleandata);
                    break;
                case 4:
                    if(nComb_runmode >= 0)
                        out << nComb_runmode;
                    break;
                case 5:
                    if(nComb_method >= 0)
                        out << nComb_method;
                    break;
                case 6:
                    if(nComb_clock >= 0)
                        out << nComb_clock;
                    break;
                case 7:
                    out << nEdit_TipDate << ' ' << nEdit_TipDate_TimeUnit;
                    break;
                case 8:
                    if(nComb_Mgene >= 0)
                        out << nComb_Mgene;
                    break;
                case 9:
                    if(nComb_nhomo >= 0)
                        out << nComb_nhomo;
                    break;
                case 10:
                    out << char('0' + bButt_getSE);
                    break;
                case 11:
                    out << char('0' + bButt_RateAncestor);
                    break;
                case 12:
                    if(nComb_model >= 0)
                        out << nComb_model;
                    break;
                case 13:
                    out << char('0' + bButt_fix_kappa);
                    break;
                case 14:
                    out << nEdit_kappa;
                    break;
                case 15:
                    out << char('0' + bButt_fix_alpha);
                    break;
                case 16:
                    out << nEdit_alpha;
                    break;
                case 17:
                    out << char('0' + bButt_Malpha);
                    break;
                case 18:
                    out << nEdit_ncatG;
                    break;
                case 19:
                    out << char('0' + bButt_fix_rho);
                    break;
                case 20:
                    out << nEdit_rho;
                    break;
                case 21:
                    if(nComb_nparK >= 0)
                        out << nComb_nparK;
                    break;
                case 22:
                    out << nEdit_ndata;
                    break;
                case 23:
                    out << strEdit_Small_Diff;
                    break;
                case 24:
                    if(nComb_icode >= 0)
                        out << nComb_icode;
                    break;
                case 25:
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
            switch(i)
            {
            case 0:
                strEdit_seqfile = strEdit_seqfile.trimmed();
                out << "      seqfile = " << strEdit_seqfile << "\n";
                break;
            case 1:
                strEdit_treefile = strEdit_treefile.trimmed();
                out << "     treefile = " << strEdit_treefile << "\n\n";
                break;
            case 2:
                strEdit_outfile_Location = strEdit_outfile_Location.trimmed();
                strEdit_outfile_Name = strEdit_outfile_Name.trimmed();
                if(strEdit_outfile_Location.isEmpty())
                    out << "      outfile = " << strEdit_outfile_Name << "\n";
                else if(strEdit_outfile_Location == "/" || strEdit_outfile_Location == "\\")
                    out << "      outfile = " << QDir::fromNativeSeparators(QDir::cleanPath( "/" + strEdit_outfile_Name )) << "\n";
                else
                    out << "      outfile = " << QDir::fromNativeSeparators(QDir::cleanPath( strEdit_outfile_Location + "/" + strEdit_outfile_Name )) << "\n";
                break;
            case 3:
                out << "    cleandata = " << char('0' + bButt_cleandata) << "\n";
                break;
            case 4:
                if(nComb_runmode >= 0)
                    out << "      runmode = " << nComb_runmode << "\n\n\n";
                break;
            case 5:
                if(nComb_method >= 0)
                    out << "       method = " << nComb_method << "\n";
                break;
            case 6:
                if(nComb_clock >= 0)
                    out << "        clock = " << nComb_clock << "\n";
                break;
            case 7:
                out << "      TipDate = " << nEdit_TipDate << ' ' << nEdit_TipDate_TimeUnit << "\n";
                break;
            case 8:
                if(nComb_Mgene >= 0)
                    out << "        Mgene = " << nComb_Mgene << "\n\n";
                break;
            case 9:
                if(nComb_nhomo >= 0)
                    out << "        nhomo = " << nComb_nhomo << "\n";
                break;
            case 10:
                out << "        getSE = " << char('0' + bButt_getSE) << "\n";
                break;
            case 11:
                out << " RateAncestor = " << char('0' + bButt_RateAncestor) << "\n\n";
                break;
            case 12:
                if(nComb_model >= 0)
                    out << "        model = " << nComb_model << "\n\n\n";
                break;
            case 13:
                out << "    fix_kappa = " << char('0' + bButt_fix_kappa) << "\n";

                break;
            case 14:
                out << "        kappa = " << nEdit_kappa << "\n\n";
                break;
            case 15:
                out << "    fix_alpha = " << char('0' + bButt_fix_alpha) << "\n";
                break;
            case 16:
                out << "        alpha = " << nEdit_alpha << "\n";
                break;
            case 17:
                out << "       Malpha = " << char('0' + bButt_Malpha) << "\n";
                break;
            case 18:
                out << "        ncatG = " << nEdit_ncatG << "\n";
                break;
            case 19:
                out << "      fix_rho = " << char('0' + bButt_fix_rho) << "\n";
                break;
            case 20:
                out << "          rho = " << nEdit_rho << "\n";
                break;
            case 21:
                if(nComb_nparK >= 0)
                    out << "        nparK = " << nComb_nparK << "\n\n";
                break;
            case 22:
                out << "        ndata = " << nEdit_ndata << "\n";
                break;
            case 23:
                out << "   Small_Diff = " << strEdit_Small_Diff << "\n";
                break;
            case 24:
                if(nComb_icode >= 0)
                    out << "        icode = " << nComb_icode << "\n";
                break;
            case 25:
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

void BasemlEngine::setDefaultData()
{
    strEdit_seqfile = "";
    strEdit_treefile = "";
    strEdit_outfile_Name = "mlb";
    strEdit_outfile_Location = "";
    bButt_cleandata = false;
    nComb_runmode = 0;
    nComb_method = 0;
    nComb_clock = 0;
    nEdit_TipDate = 0;
    nEdit_TipDate_TimeUnit = 100.0;
    nComb_Mgene = 0;
    nComb_nhomo = 0;
    bButt_getSE = false;
    bButt_RateAncestor = false;
    nComb_model = 7;
    bButt_fix_kappa = false;
    nEdit_kappa = 2;
    bButt_fix_alpha = true;
    nEdit_alpha = 0;
    bButt_Malpha = false;
    nEdit_ncatG = 5;
    bButt_fix_rho = true;
    nEdit_rho = 0;
    nComb_nparK = 0;
    nEdit_ndata = 1;
    strEdit_Small_Diff = "7e-6";
    nComb_icode = -1;
    nComb_fix_blength = 0;
}

bool BasemlEngine::readFile(const QString& fileName)
{
    CStyleBasemlCtlLoader* ctlLoader = new CStyleBasemlCtlLoader(this);
    QFile ctlFile(fileName);

    ctlLoader->com.cleandata = 0;
    ctlLoader->com.runmode = 0;
    ctlLoader->com.method = 0;
    ctlLoader->com.clock = 0;
    ctlLoader->com.TipDate = 0;
    ctlLoader->com.TipDate_TimeUnit = 100.0;
    ctlLoader->com.Mgene = 0;
    ctlLoader->com.nhomo = 0;
    ctlLoader->com.getSE = 0;
    ctlLoader->com.print = 0;
    ctlLoader->com.model = 0;
    ctlLoader->com.fix_kappa = 0;
    ctlLoader->com.kappa = 5;
    ctlLoader->com.fix_alpha = 1;
    ctlLoader->com.alpha = 0;
    ctlLoader->com.nalpha = 0;
    ctlLoader->com.ncatG = 4;
    ctlLoader->com.fix_rho = 1;
    ctlLoader->com.rho = 0;
    ctlLoader->com.nparK = 0;
    ctlLoader->com.ndata = 1;
    ctlLoader->Small_Diff = 7e-6;
    ctlLoader->com.icode = -1;
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
    bButt_cleandata = ctlLoader->com.cleandata;
    nComb_runmode = ctlLoader->com.runmode;
    nComb_method = ctlLoader->com.method;
    nComb_clock = ctlLoader->com.clock;
    nEdit_TipDate = ctlLoader->com.TipDate;
    nEdit_TipDate_TimeUnit = ctlLoader->com.TipDate_TimeUnit;
    nComb_Mgene = ctlLoader->com.Mgene;
    nComb_nhomo = ctlLoader->com.nhomo;
    bButt_getSE = ctlLoader->com.getSE;
    bButt_RateAncestor = ctlLoader->com.print;
    nComb_model = ctlLoader->com.model;
    bButt_fix_kappa = ctlLoader->com.fix_kappa;
    nEdit_kappa = ctlLoader->com.kappa;
    bButt_fix_alpha = ctlLoader->com.fix_alpha;
    nEdit_alpha = ctlLoader->com.alpha;
    bButt_Malpha = ctlLoader->com.nalpha;
    nEdit_ncatG = ctlLoader->com.ncatG;
    bButt_fix_rho = ctlLoader->com.fix_rho;
    nEdit_rho = ctlLoader->com.rho;
    nComb_nparK = ctlLoader->com.nparK;
    nEdit_ndata = ctlLoader->com.ndata;
    strEdit_Small_Diff = QString::number(ctlLoader->Small_Diff);
    nComb_icode = ctlLoader->com.icode;
    nComb_fix_blength = ctlLoader->com.fix_blength;

    delete ctlLoader;

    return true;
}

bool BasemlEngine::writeFile(const QString& fileName)
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

bool BasemlEngine::createTmpCtlFile(const QString& fileName)
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
    if(nComb_runmode < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("runmode option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>runmode</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "      runmode = " << nComb_runmode << "\n\n\n";

    if(nComb_model < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("model option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>model</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "        model = " << nComb_model << "\n\n\n";

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
    ctlOut << "      TipDate = " << nEdit_TipDate << ' ' << nEdit_TipDate_TimeUnit << "\n";
    ctlOut << "    fix_kappa = " << char('0' + bButt_fix_kappa) << "\n";
    ctlOut << "        kappa = " << nEdit_kappa << "\n\n";

    ctlOut << "    fix_alpha = " << char('0' + bButt_fix_alpha) << "\n";
    ctlOut << "        alpha = " << nEdit_alpha << "\n";
    ctlOut << "       Malpha = " << char('0' + bButt_Malpha) << "\n";
    ctlOut << "        ncatG = " << nEdit_ncatG << "\n";
    ctlOut << "      fix_rho = " << char('0' + bButt_fix_rho) << "\n";
    ctlOut << "          rho = " << nEdit_rho << "\n";
    if(nComb_nparK < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("nparK option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>nparK</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "        nparK = " << nComb_nparK << "\n\n";

    if(nComb_nhomo < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("nhomo option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>nhomo</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "        nhomo = " << nComb_nhomo << "\n";
    ctlOut << "        getSE = " << char('0' + bButt_getSE) << "\n";
    ctlOut << " RateAncestor = " << char('0' + bButt_RateAncestor) << "\n\n";

    ctlOut << "   Small_Diff = " << strEdit_Small_Diff << "\n";
    ctlOut << "    cleandata = " << char('0' + bButt_cleandata) << "\n";
    if(nComb_icode < -1) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("icode option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>icode</i>."));
        msgBox.exec();
        return false;
    }
    if(nComb_icode >= 0)
        ctlOut << "        icode = " << nComb_icode << "\n";

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

    ctlOut.flush();

    return true;
}

bool BasemlEngine::onNewDocument()
{
    if( !Engine::onNewDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool BasemlEngine::onCloseDocument()
{
    if( !Engine::onCloseDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool BasemlEngine::doOpen(const QString& fileName)
{
    return readFile(fileName);
}

bool BasemlEngine::doSave(const QString& fileName)
{
    return writeFile(fileName);
}

bool BasemlEngine::run()
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

    QString ctlFileName = (workingDirectory == "/")? "/baseml.ctl.tmp" : QDir::cleanPath( workingDirectory + "/baseml.ctl.tmp" );
    if( !createTmpCtlFile(ctlFileName) ) return false;

    if( !workingDirectory.isEmpty() ) //always true
        process->setWorkingDirectory(workingDirectory);

    QStringList arguments;
    arguments << "baseml.ctl.tmp";
    process->start(mPath, arguments);

    return true;
}
