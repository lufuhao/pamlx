//------------------------------------------------
// Copyright (C) 2012 Bo Xu <xuxbob@gmail.com>
//------------------------------------------------

#include "document.h"
#include <QFileInfo>

Document::Document() :
    modified(false)
{
}

bool Document::onNewDocument()
{
    static int count = 0;
    count++;

    mFileName.clear();
    mTitle = QObject::tr("%1%2").arg(QObject::tr("Untitled-")).arg(count);
    modified = false;

    return true;
}

bool Document::onOpenDocument()
{
    return onOpenDocument(mFileName);
}

bool Document::onOpenDocument(const QString& fileName)
{
    bool ret = doOpen(fileName);

    if(ret) {
        if( mFileName != fileName ) {
            mFileName = fileName;
            mTitle = QFileInfo(mFileName).fileName();
        }
        modified = false;
    }

    return ret;
}

bool Document::onSaveDocument()
{
    return onSaveDocument(mFileName);
}

bool Document::onSaveDocument(const QString& fileName)
{
    bool ret = doSave(fileName);

    if(ret) {
        if( mFileName != fileName ) {
            mFileName = fileName;
            mTitle = QFileInfo(mFileName).fileName();
        }
        modified = false;
    }

    return ret;
}

bool Document::onCloseDocument()
{
    mFileName.clear();
    mTitle.clear();
    modified = false;

    return true;
}
