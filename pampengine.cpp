//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "pampengine.h"
#include "def.h"
#include "cstylepampctlloader.h"
#include <QDir>
#include <QTextStream>
#include <QMessageBox>

PampEngine::PampEngine(const QString &pamlPath, QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent) :
    Engine(aOutputWindow, msec1, msec2, parent)
{
    setPath(pamlPath);
    setDefaultData();
}

bool PampEngine::setPath(const QString& pamlPath)
{
#ifdef Q_OS_WIN32
    mPath = pamlPath + "/bin/pamp.exe";
#else
    mPath = pamlPath + "/bin/pamp";
#endif

    if( QFileInfo(mPath).isFile() )
        statusOk = true;
    else
        statusOk = false;

    return statusOk;
}

void PampEngine::refreshFileContent()
{
    if(!modified && !mFileName.isEmpty()) return;

    QString tmpContent = mFileContent;
    mFileContent.clear();

    QTextStream in(&tmpContent, QIODevice::ReadOnly);
    QTextStream out(&mFileContent, QIODevice::WriteOnly);

    QStringList optionList;
    optionList << "seqfile"
               << "outfile"
               << "treefile"
               << "seqtype"
               << "ncatG"
               << "nhomo";

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
                    strEdit_treefile = strEdit_treefile.trimmed();
                    out << strEdit_treefile;
                    break;
                case 3:
                    if(nComb_seqtype >= 0)
                        out << nComb_seqtype;
                    break;
                case 4:
                    out << nEdit_ncatG;
                    break;
                case 5:
                    if(nComb_nhomo >= 0)
                        out << nComb_nhomo;
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
                strEdit_treefile = strEdit_treefile.trimmed();
                out << "     treefile = " << strEdit_treefile << "\n\n";
                break;
            case 3:
                if(nComb_seqtype >= 0)
                    out << "      seqtype = " << nComb_seqtype << "\n";
                break;
            case 4:
                out << "        ncatG = " << nEdit_ncatG << "\n";
                break;
            case 5:
                if(nComb_nhomo >= 0)
                    out << "        nhomo = " << nComb_nhomo << "\n";
                break;
            default:;
            }
        }
    }

    delete used;
    used = NULL;
}

void PampEngine::setDefaultData()
{
    strEdit_seqfile = "";
    strEdit_outfile_Name = "mp";
    strEdit_treefile = "";
    strEdit_outfile_Location = "";
    nComb_seqtype = 0;
    nEdit_ncatG = 8;
    nComb_nhomo = 0;
}

bool PampEngine::readFile(const QString& fileName)
{
    CStylePampCtlLoader* ctlLoader = new CStylePampCtlLoader(this);
    QFile ctlFile(fileName);

    ctlLoader->com.seqtype = 0;
    ctlLoader->com.ncatG = 8;
    ctlLoader->com.nhomo = 1;

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
    strEdit_treefile = ctlLoader->treefile.section('*', 0, 0).trimmed();
    nComb_seqtype = ctlLoader->com.seqtype;
    nEdit_ncatG = ctlLoader->com.ncatG;
    nComb_nhomo = ctlLoader->com.nhomo;

    delete ctlLoader;

    return true;
}

bool PampEngine::writeFile(const QString& fileName)
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

bool PampEngine::createTmpCtlFile(const QString& fileName)
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

    if(nComb_seqtype < 0) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("seqtype option can not be empty."));
        msgBox.setInformativeText(tr("Please select <i>seqtype</i>."));
        msgBox.exec();
        return false;
    }
    ctlOut << "      seqtype = " << nComb_seqtype << "\n";
    ctlOut << "        ncatG = " << nEdit_ncatG << "\n";
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

    ctlOut.flush();

    return true;
}

bool PampEngine::onNewDocument()
{
    if( !Engine::onNewDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool PampEngine::onCloseDocument()
{
    if( !Engine::onCloseDocument() )
        return false;

    mFileContent.clear();
    setDefaultData();

    return true;
}

bool PampEngine::doOpen(const QString& fileName)
{
    return readFile(fileName);
}

bool PampEngine::doSave(const QString& fileName)
{
    return writeFile(fileName);
}

bool PampEngine::run()
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

    QString ctlFileName = (workingDirectory == "/")? "/pamp.ctl.tmp" : QDir::cleanPath( workingDirectory + "/pamp.ctl.tmp" );
    if( !createTmpCtlFile(ctlFileName) ) return false;

    if( !workingDirectory.isEmpty() ) //always true
        process->setWorkingDirectory(workingDirectory);

    QStringList arguments;
    arguments << "pamp.ctl.tmp";
    process->start(mPath, arguments);

    return true;
}
