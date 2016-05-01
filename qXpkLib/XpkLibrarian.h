///////////////////////////////////
//
// CXpkLibrarian : sub-library loader&handler
// can load additional decoders as necessary,
// see related sub-library base for interface:
// - xpkLibraryBase, chunk-based XPK-format
// - xfdLibraryBase, single file decrunching
// - xadLibraryBase, archive unpacking
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XPKLIBRARIAN_H
#define XPKLIBRARIAN_H

#include <QObject>
#include <QString>
//#include <QList>
#include <QLibrary>

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
	
	// wrapper for loading/unloading
	QLibrary m_SubLib; 
	QString m_libName;
	
	QString getLibPath();
    QString getLibName(const QString &szLib, const QString &szPath);
	
    bool loadLib(const QString &szLib);
	
public:
	CXpkLibrarian(QObject *parent = 0);
	virtual ~CXpkLibrarian();
	
	/*
	// TODO: enumeration of available sub-libraries
	QList<QString> availableLibraries();
	*/
	
    xpkLibraryBase *getXpkInstance(const QString &szLib);
    xfdLibraryBase *getXfdInstance(const QString &szLib);
    xadLibraryBase *getXadInstance(const QString &szLib);
};


#endif // XPKLIBRARIAN_H
