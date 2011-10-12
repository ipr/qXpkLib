///////////////////////////////////
//
// CXpkLibrarian : sub-library loader&handler
// can load additional decoders as necessary,
// see xpkLibraryBase
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//


#ifndef XPKLIBRARIAN_H
#define XPKLIBRARIAN_H

#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QLibrary>

#include "FileType.h"

// pure virtual interface
// for lower-level library (level 0)
//
#include "xpkLibraryBase.h"
#include "XpkProgress.h"


// container/loader of derunching libraries?
//
class CXpkLibrarian : public QObject
{
protected:
	typedef xpkLibraryBase *GetXpkInstance();
	
public:
	static QList<QString> availableLibraries();
	
	static QString libNameFromType(tHeaderType enType);
	
	//static xpkLibraryBase *getDecruncher(QString szType, QLibrary &lib);
	static xpkLibraryBase *getDecruncher(QString &szLib, QLibrary &lib);
};


#endif // XPKLIBRARIAN_H
