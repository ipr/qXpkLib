///////////////////////////////////
//
// XFD-decrunching support:
// should load additional decoders as necessary.
//
// Support for decrunching "foreign"/"alien" whole-file compressions
// via loadable libraries.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XFDMASTER_H
#define XFDMASTER_H

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

// external XFD-decrunching library interface
#include "xfdLibraryBase.h"

// status information (caller and decruncher)
#include "XpkProgress.h"


class CXfdMaster : public QObject
{
	Q_OBJECT

protected:

	// wrapper for loading/unloading
	CXpkLibrarian m_SubLib; 

	// base-pointer (virtual) 
	// -> load proper by archive type
    xfdLibraryBase *m_pSubLibrary;
    
	void release();
	
public:
    CXfdMaster(QObject *parent = 0);
    virtual ~CXfdMaster(void);
    
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

#endif // XFDMASTER_H
