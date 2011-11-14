///////////////////////////////////
//
// XPK-decrunching support:
// should load additional decoders as necessary.
//
// Support for decrunching "native" chunk-based crunched files
// via loadable libraries for given compression method.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XPKMASTER_H
#define XPKMASTER_H

#include <QObject>
#include <QString>
#include <QList>

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

// use typedefs from parent
#include "qxpklib.h"

#include "XpkLibrarian.h"

#include "AnsiFile.h"
#include "FileType.h"

// pure virtual interface
// for lower-level library (level 0)
//
#include "xpkLibraryBase.h"

// status information (caller and decruncher)
#include "XpkProgress.h"

// temp.. move this here?
#include "XpkTags.h"


class CXpkMaster : public QObject //, protected XpkTags 
{
	Q_OBJECT

protected:

	// wrapper for loading/unloading
	CXpkLibrarian m_SubLib; 

	// base-pointer (virtual) 
	// -> load proper by file/packer type
	xpkLibraryBase *m_pSubLibrary;
	
	// temp for testing
	XpkTags m_Tags;
	
	void release();

public:
    CXpkMaster(QObject *parent = 0);
	virtual ~CXpkMaster(void);

    bool isSupported(CIOBuffer *pInputBuffer, CFileType &type);
    
	// list compressed file information..
	bool archiveInfo(QXpkLib::CArchiveInfo &info);
    
	bool decrunch(XpkProgress *pProgress);

	
public slots:
	
signals:
	// errors with exceptions, other with messages
	void message(QString);
	void warning(QString);
	
};

#endif // XPKMASTER_H
