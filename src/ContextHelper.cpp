#include "ContextHelper.h"

#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qapplication.h>

const QString CContextHelper::htmlExtension(".html");
const QString CContextHelper::helpDir("help");


CContextHelper::CContextHelper() 
{
}

CContextHelper::CContextHelper(QString a_dialogId)
{
    setDialogId(a_dialogId);
}

CContextHelper::~CContextHelper(void)
{
}

void CContextHelper::setDialogId(QString a_dialogId)
{
    dialogId = a_dialogId;
}

QString CContextHelper::getHelpFileName(bool existing, bool compressedHelp) const {
    QString retVal;
	const QString hlpExtension = htmlExtension;

    if (! helpDir.isEmpty()) retVal += helpDir + "/";
	retVal += dialogId;

    QString defaultName =  retVal + hlpExtension;

    retVal = defaultName;

    const QString exePath = qApp->argv()[0];
    QFileInfo fi(exePath);

    retVal = fi.dirPath(true) + "/" + retVal;
    defaultName = fi.dirPath(true) + "/" + defaultName;

    if (existing) 
    {
        if (QFile::exists(retVal)) return retVal;
        if (QFile::exists(defaultName)) return defaultName;

        return QString::null;
    }
    else return defaultName;
}

