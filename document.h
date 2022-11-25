//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QString>

class Document
{
public:
    explicit Document();

public:
    const QString& fileName() const { return mFileName; }
    const QString& title() const { return mTitle; }
    bool isModified() const { return modified; }
    void setFileName(const QString& fileName) { mFileName = fileName; }
    void setTitle(const QString& title) { mTitle = title; }
    void setModified(bool b) { modified = b; }
    const QString& fileContent() { return mFileContent; }
    virtual void refreshFileContent() = 0;

public:
    virtual bool onNewDocument();
    virtual bool onOpenDocument();
    virtual bool onOpenDocument(const QString& fileName);
    virtual bool onSaveDocument();
    virtual bool onSaveDocument(const QString& fileName);
    virtual bool onCloseDocument();

protected:
    virtual bool doOpen(const QString& fileName) = 0;
    virtual bool doSave(const QString& fileName) = 0;

protected:
    QString mFileName;
    QString mTitle;
    bool modified;
    QString mFileContent;
};

#endif // DOCUMENT_H
