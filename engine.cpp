//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "engine.h"
#include <QDir>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMessageBox>

Engine::Engine(QTextEdit *aOutputWindow, int msec1, int msec2, QObject *parent) :
    QObject(parent),
    statusOk(false),
    engineState(NotRunning),
    process(new QProcess),
    timer1(this),
    timer2(this)
{
    setOutputWindow(aOutputWindow);
    timer1.setInterval(msec1);
    timer1.setSingleShot(true);
    timer2.setInterval(msec2);
    timer2.setSingleShot(true);

    QObject::connect( process, SIGNAL(started()), this, SLOT(onProcessStarted()) );
    QObject::connect( process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onProcessFinished(int, QProcess::ExitStatus)) );
    QObject::connect( process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onProcessError(QProcess::ProcessError)) );
    QObject::connect( process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onProcessStateChanged(QProcess::ProcessState)) );
    QObject::connect( process, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadyReadStandardOutput()) );
    QObject::connect( process, SIGNAL(readyReadStandardError()), this, SLOT(onReadyReadStandardError()) );

    QObject::connect( process, SIGNAL(readyReadStandardOutput()), &timer1, SLOT(start()) );
    QObject::connect( process, SIGNAL(readyReadStandardError()), &timer1, SLOT(start()) );
    QObject::connect( &timer1, SIGNAL(timeout()), this, SLOT(onTimer1Timeout()) );

    QObject::connect( process, SIGNAL(readyReadStandardOutput()), &timer2, SLOT(start()) );
    QObject::connect( process, SIGNAL(readyReadStandardError()), &timer2, SLOT(start()) );
    QObject::connect( &timer2, SIGNAL(timeout()), this, SLOT(onTimer2Timeout()) );
}

Engine::~Engine()
{
    delete process;
    process = NULL;
}

bool Engine::onNewDocument()
{
    if(engineState != NotRunning) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Current control file is in use."));
        msgBox.setInformativeText(tr("Please press <i>stop</i> button first."));
        msgBox.exec();

        return false;
    }

    return Document::onNewDocument();
}

bool Engine::onOpenDocument(const QString &fileName)
{
    if(engineState != NotRunning) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Current control file is in use."));
        msgBox.setInformativeText(tr("Please press <i>stop</i> button first."));
        msgBox.exec();

        return false;
    }

    return Document::onOpenDocument(fileName);
}

bool Engine::onSaveDocument(const QString &fileName)
{
    if(engineState != NotRunning && mFileName != fileName) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Current control file is in use."));
        msgBox.setInformativeText(tr("Please press <i>stop</i> button first."));
        msgBox.exec();

        return false;
    }

    return Document::onSaveDocument(fileName);
}

bool Engine::onCloseDocument()
{
    if(engineState != NotRunning) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("Current control file is in use."));
        msgBox.setInformativeText(tr("Please press <i>stop</i> button first."));
        msgBox.exec();

        return false;
    }

    return Document::onCloseDocument();
}

bool Engine::run()
{
    if(!statusOk) return false;
    if(engineState != NotRunning) return false;

    mSecondaryPath = mPath;
    initBuffer();
    return true;
}

bool Engine::terminate()
{
    process->kill();
    return true;
}

void Engine::setOutputWindow(QTextEdit* aOutputWindow)
{
    outputWindow = aOutputWindow;
    outputOk = (outputWindow != NULL);
}

void Engine::removeOutputWindow()
{
    outputOk = false;
    outputWindow = NULL;
}

void Engine::output(const QString& data, QProcess::ProcessChannel processChannel)
{
    if(!outputOk) return;

    switch(processChannel)
    {
    case QProcess::StandardOutput:
        {
        outputWindow->setTextColor(QColor(Qt::black));
        outputWindow->append(data);
        }
        break;
    case QProcess::StandardError:
        {
        outputWindow->setTextColor(QColor(Qt::red));
        outputWindow->append(data);
        }
        break;
    default:;
    }
}

void Engine::onProcessStarted()
{
    if(!outputOk) return;

    outputWindow->setTextColor(QColor(Qt::blue));
    outputWindow->append( QDir::toNativeSeparators(mSecondaryPath) + "   " + tr("running...") + "\n" );

}

void Engine::onProcessFinished(int /*exitCode*/, QProcess::ExitStatus exitStatus)
{
    if(!outputOk) return;

    if(!buffer.isEmpty()) output(buffer);

    if(exitStatus == QProcess::NormalExit) {
        outputWindow->setTextColor(QColor(Qt::blue));
        outputWindow->append( "\n" + QDir::toNativeSeparators(mSecondaryPath) + "   " + tr("finished") + "\n" );
    }
    else {
        outputWindow->setTextColor(QColor(Qt::red));
        outputWindow->append( "\n" + QDir::toNativeSeparators(mSecondaryPath) + "   " + tr("-- killed") + "\n" );
    }
}

void Engine::onProcessError(QProcess::ProcessError error)
{
    switch(error)
    {
    case QProcess::FailedToStart:
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(tr("Error !"));
            msgBox.setText(tr("Failed to launch process:"));
            msgBox.setInformativeText(QDir::toNativeSeparators(mSecondaryPath));
            msgBox.exec();
        }
        break;
    default:;
    }
}

void Engine::onProcessStateChanged(QProcess::ProcessState newState)
{
    switch(newState)
    {
    case QProcess::NotRunning:
        timer1.stop();
        timer2.stop();
        break;
    case QProcess::Running:
        timer1.start();
        timer2.start();
        break;
    default:;
    }

    engineState = (EngineState)newState;
    emit stateChanged(engineState);
}

void Engine::onReadyReadStandardOutput()
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

#if defined(Q_OS_WIN32)
    if(newData.endsWith("\r\n"))
        newData.chop(1);
#endif
    newData.chop(1);

    output(newData);
}

void Engine::onReadyReadStandardError()
{
    QByteArray newData = process->readAllStandardError();
    if(newData.isEmpty()) return;

#if defined(Q_OS_WIN32)
    if(newData.endsWith("\r\n"))
        newData.chop(1);
#endif
    if(newData.endsWith('\r') || newData.endsWith('\n'))
        newData.chop(1);

    output(newData, QProcess::StandardError);
}

void Engine::onTimer1Timeout()
{
    process->write("\n\n\n");//process->putChar('\n');
}

void Engine::onTimer2Timeout()
{
    output(tr("Timeout"), QProcess::StandardError);
    terminate();
}
