///////////////////////////////////
//
// XAD-library support:
// should load additional decoders as necessary.
//
// Support for unpacking multi-file archives via loadable libraries.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XADMASTER_H
#define XADMASTER_H

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

#include "XpkProgress.h"
#include "xadLibraryBase.h"


class CXadMaster : public QObject
{
	Q_OBJECT
	
protected:
	// wrapper for loading/unloading
	CXpkLibrarian m_SubLib; 

	// base-pointer (virtual) 
	// -> load proper by archive type
	xadLibraryBase *m_pSubLibrary;
	
	void release();
	
public:
    CXadMaster(QObject *parent = 0);
    virtual ~CXadMaster(void);

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

#endif // XADMASTER_H
