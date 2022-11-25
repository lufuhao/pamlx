//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "evolverengine.h"
#include "def.h"
#include "cstyleevolverctlloader.h"
#include <QDir>
#include <QTextStream>
#include <QMessageBox>

EvolverEngine::EvolverEngine(const QString &pamlPath, EvolverEngine::Type type, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent) :
    Engine(aOutputWindow, msec1, msec2, parent),
    mFileType(type)
{
    setPath(pamlPath);

    if(type != Nucleotide)
        setDefaultData(Nucleotide);
    if(type != Codon)
        setDefaultData(Codon);
    if(type != AminoAcid)
        setDefaultData(AminoAcid);

    setDefaultData(type);
}

bool EvolverEngine::setPath(const QString& pamlPath)
{
#ifdef Q_OS_WIN32
    mPath = pamlPath + "/bin/evolver.exe";
#else
    mPath = pamlPath + "/bin/evolver";
#endif

    if( QFileInfo(mPath).isFile() )
        statusOk = true;
    else
        statusOk = false;

    return statusOk;
}

void EvolverEngine::refreshFileContent()
{
    if(!modified && !mFileName.isEmpty()) return;

    QString tmpContent = mFileContent;
    mFileContent.clear();

    QTextStream in(&tmpContent, QIODevice::ReadOnly);
    QTextStream out(&mFileContent, QIODevice::WriteOnly);

    QString line;
    QString comment;

    int n = (mFileType == Nucleotide)? 9 : 8;

    for(int i=0; i<n; i++) {
        while(!in.atEnd()) {
            line = in.readLine();
            if( !line.trimmed().isEmpty() ) break;
            out << line << "\n";
        }

        switch(i)
        {
        case 0:
            out << nComb_outFormat;
            break;
        case 1:
            out << nEdit_seed;
            break;
        case 2:
            out << nEdit_seqs << ' ' << nEdit_sites << ' ' << nEdit_replicates;
            break;
        case 3:
            out << nEdit_treeLength;
            break;
        case 4:
            if(!strEdit_tree.isEmpty())
                out << strEdit_tree << ';';
            else
                out << "<tree>";
            break;
        case 5:
            if(mFileType == Nucleotide)
                out << nComb_model;
            else if(mFileType==Codon)
                out << nEdit_omega;
            else if(mFileType==AminoAcid)
                out << nEdit_alpha << ' ' << nEdit_ncatG;
            break;
        case 6:
            if(mFileType == Nucleotide) {
                if(!strEdit_parameters.isEmpty())
                    out << strEdit_parameters;
                else
                    out << "<parameters>";
            }
            else if(mFileType==Codon)
                out << nEdit_kappa;
            else if(mFileType==AminoAcid)
                out << nComb_model << ' ' << strEdit_aminoAcidSbstRtFile;
            break;
        case 7:
            if(mFileType == Nucleotide)
                out << nEdit_alpha << ' ' << nEdit_ncatG;
            else if(mFileType==Codon) {
                for(int i =0; i<64; i++) {
                    out << qSetFieldWidth(12) << qSetRealNumberPrecision(8) << right << nEdit_codonFrequencies[i];
                    if(i%4 == 3 && i != 63) {
                        out << qSetFieldWidth(0) << "\n";
                        in.readLine();
                    }
                }
                out << qSetFieldWidth(0);
            }
            else if(mFileType==AminoAcid) {
                for(int i =0; i<20; i++) {
                    out << nEdit_aminoAcidFrequencies[i] << ' ';
                    if(i%10 == 9 && i != 19) {
                        out << "\n";
                        in.readLine();
                    }
                }
            }
            break;
        case 8:
            if(mFileType == Nucleotide) {
                out << nEdit_baseFrequencies[0] << ' ' << nEdit_baseFrequencies[1] << ' ' << nEdit_baseFrequencies[2] << ' ' << nEdit_baseFrequencies[3];
            }
            break;
        default:;
        }

        comment = line.section('*', 1);
        if(!comment.isEmpty())
            out << "   *" << comment << "\n";
        else
            out << "\n";
    }

    if(in.atEnd()) {
        if(mFileType == Nucleotide)
            out << "  T        C        A        G\n\n";
        else if(mFileType==Codon)
            out << "\n// end of file.\n";
        else if(mFileType==AminoAcid)
            out << "\n A R N D C Q E G H I\n L K M F P S T W Y V\n"
                << "\n// end of file\n";
    }

    while(!in.atEnd()) {
        line = in.readLine();
        out << line << "\n";
    }
}

void EvolverEngine::setFileType(Type type)
{
    mFileType = type;
    setDefaultData(mFileType);
}

void EvolverEngine::setDefaultData(Type type)
{
    switch(type)
    {
    case Nucleotide:
        {
        strEdit_Output_Location = "";
        nComb_outFormat = 0;
        nEdit_seed = -1;
        nEdit_seqs = 9;
        nEdit_sites = 1;
        nEdit_replicates = 1;
        nEdit_treeLength = -1;
        strEdit_tree = "(((a:0.2, b:0.2):0.5, ((c:0.1, d:0.1):0.3, e:0.4):0.3):0.3, (f:0.8, (g:0.3, (h:0.05, i:0.05):0.25):0.5):0.2)";

        nComb_model = 0;
        strEdit_parameters = "";
        nEdit_alpha = 0.25;
        nEdit_ncatG = 5;
        nEdit_baseFrequencies[0] = 0.25;
        nEdit_baseFrequencies[1] = 0.25;
        nEdit_baseFrequencies[2] = 0.25;
        nEdit_baseFrequencies[3] = 0.25;
        }
        break;

    case Codon:
        {
        strEdit_Output_Location = "";
        nComb_outFormat = 0;
        nEdit_seed = -1;
        nEdit_seqs = 9;
        nEdit_sites = 1;
        nEdit_replicates = 1;
        nEdit_treeLength = -1;
        strEdit_tree = "(((a:0.2, b:0.2):0.5, ((c:0.1, d:0.1):0.3, e:0.4):0.3):0.3, (f:0.8, (g:0.3, (h:0.05, i:0.05):0.25):0.5):0.2)";

        nEdit_omega = 0.3;
        nEdit_kappa = 5;

        nEdit_codonFrequencies[0] = 0.00983798;
        nEdit_codonFrequencies[1] = 0.01745548;
        nEdit_codonFrequencies[2] = 0.00222048;
        nEdit_codonFrequencies[3] = 0.01443315;
        nEdit_codonFrequencies[4] = 0.00844604;
        nEdit_codonFrequencies[5] = 0.01498576;
        nEdit_codonFrequencies[6] = 0.00190632;
        nEdit_codonFrequencies[7] = 0.01239105;
        nEdit_codonFrequencies[8] = 0.01064012;
        nEdit_codonFrequencies[9] = 0.01887870;
        nEdit_codonFrequencies[10] = 0;
        nEdit_codonFrequencies[11] = 0;
        nEdit_codonFrequencies[12] = 0.00469486;
        nEdit_codonFrequencies[13] = 0.00833007;
        nEdit_codonFrequencies[14] = 0;
        nEdit_codonFrequencies[15] = 0.00688776;
        nEdit_codonFrequencies[16] = 0.01592816;
        nEdit_codonFrequencies[17] = 0.02826125;
        nEdit_codonFrequencies[18] = 0.00359507;
        nEdit_codonFrequencies[19] = 0.02336796;
        nEdit_codonFrequencies[20] = 0.01367453;
        nEdit_codonFrequencies[21] = 0.02426265;
        nEdit_codonFrequencies[22] = 0.00308642;
        nEdit_codonFrequencies[23] = 0.02006170;
        nEdit_codonFrequencies[24] = 0.01722686;
        nEdit_codonFrequencies[25] = 0.03056552;
        nEdit_codonFrequencies[26] = 0.00388819;
        nEdit_codonFrequencies[27] = 0.02527326;
        nEdit_codonFrequencies[28] = 0.00760121;
        nEdit_codonFrequencies[29] = 0.01348678;
        nEdit_codonFrequencies[30] = 0.00171563;
        nEdit_codonFrequencies[31] = 0.01115161;
        nEdit_codonFrequencies[32] = 0.01574077;
        nEdit_codonFrequencies[33] = 0.02792876;
        nEdit_codonFrequencies[34] = 0.00355278;
        nEdit_codonFrequencies[35] = 0.02309304;
        nEdit_codonFrequencies[36] = 0.01351366;
        nEdit_codonFrequencies[37] = 0.02397721;
        nEdit_codonFrequencies[38] = 0.00305010;
        nEdit_codonFrequencies[39] = 0.01982568;
        nEdit_codonFrequencies[40] = 0.01702419;
        nEdit_codonFrequencies[41] = 0.03020593;
        nEdit_codonFrequencies[42] = 0.00384245;
        nEdit_codonFrequencies[43] = 0.02497593;
        nEdit_codonFrequencies[44] = 0.00751178;
        nEdit_codonFrequencies[45] = 0.01332811;
        nEdit_codonFrequencies[46] = 0.00169545;
        nEdit_codonFrequencies[47] = 0.01102042;
        nEdit_codonFrequencies[48] = 0.02525082;
        nEdit_codonFrequencies[49] = 0.04480239;
        nEdit_codonFrequencies[50] = 0.00569924;
        nEdit_codonFrequencies[51] = 0.03704508;
        nEdit_codonFrequencies[52] = 0.02167816;
        nEdit_codonFrequencies[53] = 0.03846344;
        nEdit_codonFrequencies[54] = 0.00489288;
        nEdit_codonFrequencies[55] = 0.03180369;
        nEdit_codonFrequencies[56] = 0.02730964;
        nEdit_codonFrequencies[57] = 0.04845534;
        nEdit_codonFrequencies[58] = 0.00616393;
        nEdit_codonFrequencies[59] = 0.04006555;
        nEdit_codonFrequencies[60] = 0.01205015;
        nEdit_codonFrequencies[61] = 0.02138052;
        nEdit_codonFrequencies[62] = 0.00271978;
        nEdit_codonFrequencies[63] = 0.01767859;
        }
        break;

    case AminoAcid:
        {
        strEdit_Output_Location = "";
        nComb_outFormat = 0;
        nEdit_seed = -1;
        nEdit_seqs = 9;
        nEdit_sites = 1;
        nEdit_replicates = 1;
        nEdit_treeLength = -1;
        strEdit_tree = "(((a:0.2, b:0.2):0.5, ((c:0.1, d:0.1):0.3, e:0.4):0.3):0.3, (f:0.8, (g:0.3, (h:0.05, i:0.05):0.25):0.5):0.2)";

        nEdit_alpha = 0.5;
        nEdit_ncatG = 8;
        nComb_model = 0;
        strEdit_aminoAcidSbstRtFile = "";

        for(int i=0; i<20; i++)
            nEdit_aminoAcidFrequencies[i] = 0.05;
        }
        break;

    default:
        return;
    }
}

bool EvolverEngine::readFile(const QString& fileName, Type type)
{
    CStyleEvolverCtlLoader* ctlLoader = new CStyleEvolverCtlLoader(this);
    QFile ctlFile(fileName);

    ctlLoader->com.seqtype = type;
    ctlLoader->outFormat = 0;
    ctlLoader->seed = -1;
    ctlLoader->com.ns = 0;
    ctlLoader->com.ls = 0;
    ctlLoader->replicates = 0;
    ctlLoader->treeLength = -1;
    ctlLoader->com.model = 0;
    ctlLoader->com.alpha = 0;
    ctlLoader->com.ncatG = 5;
    ctlLoader->com.omega = 0.3;
    ctlLoader->com.kappa = 5;
    for(int i=0; i<64; i++)
        ctlLoader->com.pi[i] = 0;

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

    strEdit_Output_Location = "";
    nComb_outFormat = ctlLoader->outFormat;
    nEdit_seed = ctlLoader->seed;
    nEdit_seqs = ctlLoader->com.ns;
    nEdit_sites = ctlLoader->com.ls;
    nEdit_replicates = ctlLoader->replicates;
    nEdit_treeLength = ctlLoader->treeLength;
    strEdit_tree = ctlLoader->treeString.section('*', 0, 0).trimmed();
    while(strEdit_tree.endsWith(';')) strEdit_tree.remove(-1, 1);

    switch(type)
    {
    case Nucleotide:
        {
        nComb_model = ctlLoader->com.model;
        strEdit_parameters = ctlLoader->parameters.section('*', 0, 0).trimmed();
        nEdit_alpha = ctlLoader->com.alpha;
        nEdit_ncatG = ctlLoader->com.ncatG;
        nEdit_baseFrequencies[0] = ctlLoader->com.pi[0];
        nEdit_baseFrequencies[1] = ctlLoader->com.pi[1];
        nEdit_baseFrequencies[2] = ctlLoader->com.pi[2];
        nEdit_baseFrequencies[3] = ctlLoader->com.pi[3];
        }
        break;

    case Codon:
        {
        nEdit_omega = ctlLoader->com.omega;
        nEdit_kappa = ctlLoader->com.kappa;

        for(int i=0; i<64; i++)
            nEdit_codonFrequencies[i] = ctlLoader->com.pi[i];
        }
        break;

    case AminoAcid:
        {
        nEdit_alpha = ctlLoader->com.alpha;
        nEdit_ncatG = ctlLoader->com.ncatG;
        nComb_model = ctlLoader->com.alpha;
        strEdit_aminoAcidSbstRtFile = ctlLoader->com.daafile;

        for(int i=0; i<20; i++)
            nEdit_aminoAcidFrequencies[i] = ctlLoader->com.pi[i];
        }
        break;

    default:
        delete ctlLoader;
        return false;
    }

    delete ctlLoader;

    return true;
}

bool EvolverEngine::writeFile(const QString& fileName, Type type)
{
    mFileType = type;

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

bool EvolverEngine::createTmpCtlFile(const QString& fileName, Type type)
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

    switch(type)
    {
    case Nucleotide:
        {
        if(nComb_outFormat < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Format option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>format</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << nComb_outFormat << "\n";
        ctlOut << nEdit_seed << "\n\n";

        ctlOut << nEdit_seqs << ' ' << nEdit_sites << ' ' << nEdit_replicates << "\n";
        ctlOut << nEdit_treeLength << "\n\n";

        if( strEdit_tree.isEmpty() ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Tree can not be empty."));
            msgBox.setInformativeText(tr("Please enter <i>tree</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << strEdit_tree << ";\n\n";

        if(nComb_model < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Model option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>model</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << nComb_model << "\n";
        if( strEdit_parameters.isEmpty() ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Parameters can not be empty."));
            msgBox.setInformativeText(tr("Please enter <i>parameters</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << strEdit_parameters << "\n";
        ctlOut << nEdit_alpha << ' ' << nEdit_ncatG << "\n\n";

        ctlOut << nEdit_baseFrequencies[0] << ' ' << nEdit_baseFrequencies[1] << ' ' << nEdit_baseFrequencies[2] << ' ' << nEdit_baseFrequencies[3] << "\n";
        ctlOut << "  T        C        A        G\n\n";
        }
        break;

    case Codon:
        {
        if(nComb_outFormat < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Format option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>format</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << nComb_outFormat << "\n";
        ctlOut << nEdit_seed << "\n";
        ctlOut << nEdit_seqs << ' ' << nEdit_sites << ' ' << nEdit_replicates << "\n\n";

        ctlOut << nEdit_treeLength << "\n";
        if( strEdit_tree.isEmpty() ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Tree can not be empty."));
            msgBox.setInformativeText(tr("Please enter <i>tree</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << strEdit_tree << "\n\n";

        ctlOut << nEdit_omega << "\n";
        ctlOut << nEdit_kappa << "\n\n";

        for(int i =0; i<64; i++) {
            ctlOut << qSetFieldWidth(12) << qSetRealNumberPrecision(8) << right << nEdit_codonFrequencies[i];
            if(i%4 == 3) ctlOut << qSetFieldWidth(0) << "\n";
        }

        ctlOut << "\n// end of file.\n";
        }
        break;

    case AminoAcid:
        {
        if(nComb_outFormat < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Format option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>format</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << nComb_outFormat << "\n";
        ctlOut << nEdit_seed << "\n\n";

        ctlOut << nEdit_seqs << ' ' << nEdit_sites << ' ' << nEdit_replicates << "\n\n";

        ctlOut << nEdit_treeLength << "\n\n";

        if( strEdit_tree.isEmpty() ) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Tree can not be empty."));
            msgBox.setInformativeText(tr("Please enter <i>tree</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << strEdit_tree << "\n\n";

        ctlOut << nEdit_alpha << ' ' << nEdit_ncatG << "\n";
        if(nComb_model < 0) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("Model option can not be empty."));
            msgBox.setInformativeText(tr("Please select <i>model</i>."));
            msgBox.exec();
            return false;
        }
        ctlOut << nComb_model << ' ';
        strEdit_aminoAcidSbstRtFile = strEdit_aminoAcidSbstRtFile.trimmed();
        if(nComb_model >= 2) {
            if( strEdit_aminoAcidSbstRtFile.isEmpty() ) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Warning"));
                msgBox.setText(tr("Amino acid substitution rate file's path can not be empty."));
                msgBox.setInformativeText(tr("Please enter <i>substitution rate file</i>."));
                msgBox.exec();
                return false;
            }
            if( strEdit_aminoAcidSbstRtFile.length() >= 90/*not bigger than 96 in evolver.c*/ ) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Warning"));
                msgBox.setText(tr("Full name of amino acid substitution rate file is too long."));
                msgBox.setInformativeText(tr("Please check <i>substitution rate file</i>."));
                msgBox.exec();
                return false;
            }
            if( strEdit_aminoAcidSbstRtFile.contains(' ') ) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(tr("Warning"));
                msgBox.setText(tr("There should be no space characters in amino acid substitution rate file's path."));
                msgBox.setInformativeText(tr("Please check <i>substitution rate file</i>."));
                msgBox.exec();
                return false;
            }
            if( QDir::isAbsolutePath(strEdit_aminoAcidSbstRtFile) || mFileName.isEmpty() ) {
                if( !QDir::isAbsolutePath(strEdit_aminoAcidSbstRtFile) || !QFileInfo(strEdit_aminoAcidSbstRtFile).isFile() ) {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle(tr("Warning"));
                    msgBox.setText(tr("Failed to open amino acid substitution rate file."));
                    msgBox.setInformativeText(tr("Please check <i>substitution rate file</i>."));
                    msgBox.exec();
                    return false;
                }
                ctlOut << strEdit_aminoAcidSbstRtFile << "\n\n";
            }
            else {
                QString ratefilePath = QDir::cleanPath( QFileInfo(mFileName).absoluteDir().absoluteFilePath(strEdit_aminoAcidSbstRtFile) );
                if( !QFileInfo(ratefilePath).isFile() ) {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle(tr("Warning"));
                    msgBox.setText(tr("Failed to open amino acid substitution rate file."));
                    msgBox.setInformativeText(tr("Please check <i>substitution rate file</i>."));
                    msgBox.exec();
                    return false;
                }
                if( ratefilePath.length() >= 90/*not bigger than 96 in evolver.c*/ ) {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle(tr("Warning"));
                    msgBox.setText(tr("Absolute path of amino acid substitution rate file is too long."));
                    msgBox.setInformativeText(QDir::toNativeSeparators(ratefilePath));
                    msgBox.exec();
                    return false;
                }
                if( ratefilePath.contains(' ') ) {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.setWindowTitle(tr("Warning"));
                    msgBox.setText(tr("Absolute path of amino acid substitution rate file can not have space characters."));
                    msgBox.setInformativeText(QDir::toNativeSeparators(ratefilePath));
                    msgBox.exec();
                    return false;
                }
                ctlOut << ratefilePath << "\n\n";
            }
        }
        else {
            ctlOut << strEdit_aminoAcidSbstRtFile << "\n\n";
        }

        for(int i =0; i<20; i++) {
            ctlOut << nEdit_aminoAcidFrequencies[i] << ' ';
            if(i%10 == 9) ctlOut << "\n";
        }

        ctlOut << "\n A R N D C Q E G H I\n L K M F P S T W Y V\n";

        ctlOut << "\n// end of file\n";
        }
        break;

    default:
        return false;
    }

    ctlOut.flush();

    return true;
}

bool EvolverEngine::onNewDocument()
{
    if( !Engine::onNewDocument() )
        return false;

    mFileContent.clear();
    setDefaultData(mFileType);

    return true;
}

bool EvolverEngine::onCloseDocument()
{
    if( !Engine::onCloseDocument() )
        return false;

    mFileContent.clear();
    setDefaultData(mFileType);

    return true;
}

bool EvolverEngine::doOpen(const QString& fileName)
{
    return readFile(fileName, mFileType);
}

bool EvolverEngine::doSave(const QString& fileName)
{
    return writeFile(fileName, mFileType);
}

bool EvolverEngine::run()
{
    if( !Engine::run() )
        return false;

    strEdit_Output_Location = strEdit_Output_Location.trimmed();
    if( strEdit_Output_Location.isEmpty() ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Output file's path can not be empty."));
        msgBox.setInformativeText(tr("Please enter <i>location</i>."));
        msgBox.exec();
        return false;
    }
    if( strEdit_Output_Location.length() >= MAX_PATH ) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Path of output file is too long."));
        msgBox.setInformativeText(tr("Please check <i>location</i>."));
        msgBox.exec();
        return false;
    }

    QString workingDirectory = strEdit_Output_Location;

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

    switch(mFileType)
    {
    case Nucleotide:
        {
        QString ctlFileName = (workingDirectory == "/")? "/MCbase.dat.tmp" : QDir::cleanPath( workingDirectory + "/MCbase.dat.tmp" );
        if( !createTmpCtlFile(ctlFileName, mFileType) ) return false;

        if( !workingDirectory.isEmpty() ) //always true
            process->setWorkingDirectory(workingDirectory);

        QStringList arguments;
        arguments << "5" << "MCbase.dat.tmp";
        process->start(mPath, arguments);
        }
        break;

    case Codon:
        {
        QString ctlFileName = (workingDirectory == "/")? "/MCcodon.dat.tmp" : QDir::cleanPath( workingDirectory + "/MCcodon.dat.tmp" );
        if( !createTmpCtlFile(ctlFileName, mFileType) ) return false;

        if( !workingDirectory.isEmpty() ) //always true
            process->setWorkingDirectory(workingDirectory);

        QStringList arguments;
        arguments << "6" << "MCcodon.dat.tmp";
        process->start(mPath, arguments);
        }
        break;

    case AminoAcid:
        {
        QString ctlFileName = (workingDirectory == "/")? "/MCaa.dat.tmp" : QDir::cleanPath( workingDirectory + "/MCaa.dat.tmp" );
        if( !createTmpCtlFile(ctlFileName, mFileType) ) return false;

        if( !workingDirectory.isEmpty() ) //always true
            process->setWorkingDirectory(workingDirectory);

        QStringList arguments;
        arguments << "7" << "MCaa.dat.tmp";
        process->start(mPath, arguments);
        }
        break;

    default:
        return false;
    }

    return true;
}
