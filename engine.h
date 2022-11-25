//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef ENGINE_H
#define ENGINE_H

#include "document.h"
#include <QObject>
#include <QString>
#include <QProcess>
#include <QTimer>

class QTextEdit;

class Engine : public QObject, public Document
{
    Q_OBJECT

public:
    explicit Engine(QTextEdit *aOutputWindow = 0, int msec1 = 0, int msec2 = 0, QObject *parent = 0);
    virtual ~Engine();

public:
    enum EngineState { NotRunning, Starting, Running };

public:
    virtual bool setPath(const QString& aPath) = 0;
    const QString& path() const { return mPath; }
    bool isStatusOk() const { return statusOk; }
    EngineState state() const { return engineState; }
    void setOutputOk(bool b) { outputOk = b; }
    bool isOutputOk() const { return outputOk; }

public:
    bool onNewDocument();
    bool onOpenDocument(const QString &fileName);
    bool onSaveDocument(const QString& fileName);
    bool onCloseDocument();

protected:
    void initBuffer() { buffer.clear(); }

signals:
    void stateChanged(Engine::EngineState newState);

public slots:
    virtual bool run();
    virtual bool terminate();

    void setOutputWindow(QTextEdit* aOutputWindow);
    void removeOutputWindow();
    void output(const QString& data, QProcess::ProcessChannel processChannel = QProcess::StandardOutput);

    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onProcessStateChanged(QProcess::ProcessState newState);
    virtual void onReadyReadStandardOutput();
    virtual void onReadyReadStandardError();

    void onTimer1Timeout();
    void onTimer2Timeout();

protected:
    QString                 mPath;
    QString                 mSecondaryPath;
    bool                    statusOk;
    EngineState             engineState;
    QProcess*               process;
    QByteArray              buffer;
    QTextEdit*              outputWindow;
    bool                    outputOk;
    QTimer                  timer1;
    QTimer                  timer2;
};

#endif // ENGINE_H
