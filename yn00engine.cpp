//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "yn00engine.h"
#include "def.h"
#include "cstyleyn00ctlloader.h"
#include <QDir>
#include <QTextStream>
#include <QMessageBox>

YN00Engine::YN00Engine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent) :
    Engine(aOutputWindow, msec1, msec2, parent)
{
    setPath(pamlPath);
    setDefaultData();
}

bool YN00Engine::setPath(const QString& pamlPath)
{
#ifdef Q_OS_WIN32
    mPath = pamlPath + "/bin/yn00.exe";
#else
    mPath = pamlPath + "/bin/yn00";
#endif

    if( QFileInfo(mPath).isFile() )
        statusOk = true;
    else
        statusOk = false;

    return statusOk;
}

void YN00Engine::refreshFileContent()
{
    if(!modified && !mFileName.isEmpty()) return;

    QString tmpContent = mFileContent;
    mFileContent.clear();

    QTextStream in(&tmpContent, QIODevice::ReadOnly);
    QTextStream out(&mFileContent, QIODevice::WriteOnly);

    QStringList optionList;
    optionList << "seqfile"
               << "outfile"
               << "verbose"
               << "noisy"
               << "icode"
               << "weighting"
               << "commonkappa"
               << "commonf3x4"
               << "ndata";

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
                    strEdit_outfile_Location = strEdit_outfile_Location.trimmed();
                    strEdit_outfile_Name = strEdit_outfile_Name.trimmed();
                    if(strEdit_outfile_Location.isEmpty())
                        out << strEdit_outfile_Name;
                    else if(strEdit_outfile_Location == "/" || strEdit_outfile_Location == "\\")
                        out << QDir::fromNativeSeparators(QDir::cleanPath( "/" + strEdit_outfile_Name ));
                    else
                        out << QDir::fromNativeSeparators(QDir::cleanPath( strEdit_outfile_Location + "/" + strEdit_outfile_Name ));
                    break;
                case 2:
                    if(nComb_verbose >= 0)
                        out << nComb_verbose;
                    break;
                case 3:
                    if(nComb_noisy >= 0)
                        out << nComb_noisy;
                    break;
                case 4:
                    if(nComb_icode >= 0)
                        out << nComb_icode;
                    break;
                case 5:
                    out << char('0' + bButt_weighting);
                    break;
                case 6:
                    out << char('0' + bButt_commonkappa);
                    break;
                case 7:
                    out << char('0' + bButt_commonf3x4);
                    break;
                case 8:
                    out << nEdit_ndata;
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
                strEdit_outfile_Location = strEdit_outfile_Location.trimmed();
                strEdit_outfile_Name = strEdit_outfile_Name.trimmed();
                if(strEdit_outfile_Location.isEmpty())
                    out << "      outfile = " << strEdit_outfile_Name << "\n";
                else if(strEdit_outfile_Location == "/" || strEdit_outfile_Location == "\\")
                    out << "      outfile = " << QDir::fromNativeSeparators(QDir::cleanPath( "/" + strEdit_outfile_Name )) << "\n";
                else
                    out << "      outfile = " << QDir::fromNativeSeparators(QDir::cleanPath( strEdit_outfile_Location + "/" + strEdit_outfile_Name )) << "\n";
                break;
            case 2:
                if(nComb_verbose >= 0)
                    out << "      verbose = " << nComb_verbose << "\n";
                break;
            case 3:
                if(nComb_noisy >= 0)
                    out << "        noisy = " << nComb_noisy << "\n\n";
                break;
            case 4:
                if(nComb_icode >= 0)
                    out << "        icode = " << nComb_icode << "\n\n";
                break;
            case 5:
                out << "    weighting = " << char('0' + bButt_weighting) << "\n";
                break;
            case 6:
                out << "  commonkappa = " << char('0' + bButt_commonkappa) << "\n";
                break;
            case 7:
                out << "   commonf3x4 = " << char('0' + bButt_commonf3x4) << "\n";
                break;
            case 8:
                out << "        ndata = " << nEdit_ndata << "\n";
                break;
            default:;
            }
        }
    }

    delete used;
    used = NULL;
}

void YN00Engine::setDefaultData()
{
    strEdit_seqfile = "";
    strEdit_outfile_Name = "yn";
    strEdit_outfile_Location = "";
    nComb_verbose = 0;
    nComb_noisy = 1;
    nComb_icode = 0;
    bButt_weighting = 0;
    bButt_commonkappa = 1;
    bButt_commonf3x4 = 0;
    nEdit_ndata = 1;
}

bool YN00Engine::readFile(const QString& fileName)
{
    CStyleYN00CtlLoader* ctlLoader = new CStyleYN00CtlLoader(this);
    QFile ctlFile(fileName);

    ctlLoader->com.verbose = 0;
    ctlLoader->noisy = 1;
    ctlLoader->com.icode = 0;
    ctlLoader->com.weighting = 0;
    ctlLoader->com.kcommon = 1;
    ctlLoader->com.fcommon = 0;
    ctlLoader->com.ndata = 1;

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
    QString outfile = ctlLoader->outfile.section('*', 0, 0).trimmed();
    strEdit_outfile_Name = QFileInfo(outfile).fileName();
    strEdit_outfile_Location = QDir::toNativeSeparators(QFileInfo(outfile).path());
    if(strEdit_outfile_Location == ".") strEdit_outfile_Location = QDir::toNativeSeparators("./");
    if(strEdit_outfile_Location == "..") strEdit_outfile_Location = QDir::toNativeSeparators("../");
    nComb_verbose = ctlLoader->com.verbose;
    nComb_noisy = ctlLoader->noisy;
    nComb_icode = ctlLoader->com.icode;
    bButt_weighting = ctlLoader->com.weighting;
    bButt_commonkappa = ctlLoader->com.kcommon;
    bButt_commonf3x4 = ctlLoader->com.fcommon;
    nEdit_ndata = ctlLoader->com.ndata;

    delete ctlLoader;

    return true;
}

bool YN00Engine::writeFile(const QString& fileName)
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

bool YN00Engine::createTmpCtlFile(const QString& fileName)
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

    if(nComb_verbose < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("verbose option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>verbose</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "      verbose = " << nComb_verbose << "\n";
    if(nComb_noisy < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("noisy option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>noisy</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "        noisy = " << nComb_noisy << "\n\n";

    if(nComb_icode < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("icode option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>icode</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "        icode = " << nComb_icode << "\n\n";

    ctlOut << "    weighting = " << char('0' + bButt_weighting) << "\n";
    ctlOut << "  commonkappa = " << char('0' + bButt_commonkappa) << "\n";
    ctlOut << "   commonf3x4 = " << char('0' + bButt_commonf3x4) << "\n";
    ctlOut << "        ndata = " << nEdit_ndata << "\n";

    ctlOut.flush();

    return true;
}

bool YN00Engine::onNewDocument()
{
    if( !Engine::onNewDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool YN00Engine::onCloseDocument()
{
    if( !Engine::onCloseDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool YN00Engine::doOpen(const QString& fileName)
{
    return readFile(fileName);
}

bool YN00Engine::doSave(const QString& fileName)
{
    return writeFile(fileName);
}

bool YN00Engine::run()
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

    QString ctlFileName = (workingDirectory == "/")? "/yn00.ctl.tmp" : QDir::cleanPath( workingDirectory + "/yn00.ctl.tmp" );
    if( !createTmpCtlFile(ctlFileName) ) return false;

    if( !workingDirectory.isEmpty() ) //always true
        process->setWorkingDirectory(workingDirectory);

    QStringList arguments;
    arguments << "yn00.ctl.tmp";
    process->start(mPath, arguments);

    return true;
}
