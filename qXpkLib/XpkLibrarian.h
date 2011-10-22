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
#include "xfdLibraryBase.h"
#include "xadLibraryBase.h"
#include "XpkProgress.h"


// container/loader of derunching libraries?
//
class CXpkLibrarian : public QObject
{
protected:
	typedef xpkLibraryBase *GetXpkInstance();
	typedef xfdLibraryBase *GetXfdInstance();
	typedef xadLibraryBase *GetXadInstance();
	
	static QString getLibPath();
	static QString getLibName(QString &szLib, QString &szPath);
	
public:
	static QList<QString> availableLibraries();
	
	static xpkLibraryBase *getXpkInstance(QString &szLib, QLibrary &lib);
	static xfdLibraryBase *getXfdInstance(QString &szLib, QLibrary &lib);
	static xadLibraryBase *getXadInstance(QString &szLib, QLibrary &lib);
};


#endif // XPKLIBRARIAN_H
