///////////////////////////////////
//
// Actual decoder handling:
// can load additional decoders as necessary,
// see xpkLibraryBase
//
// CXpkMaster : main decrunch handler
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XPKMASTER_H
#define XPKMASTER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QLibrary>

// use typedefs from parent
#include "qxpklib.h"

#include "AnsiFile.h"
#include "FileType.h"

// pure virtual interface
// for lower-level library (level 0)
//
#include "xpkLibraryBase.h"
#include "XpkProgress.h"

// temp.. move here?
#include "XpkTags.h"



class CXpkMaster : public QObject
{
	Q_OBJECT

protected:

	// wrapper for loading/unloading
	QLibrary m_SubLib; 

	// base-pointer (virtual) 
	// -> load proper by file/packer type
	xpkLibraryBase *m_pSubLibrary;
	
	// temp for testing
	XpkTags m_Tags;
	
	void release();

public:
    CXpkMaster(QObject *parent = 0);
	virtual ~CXpkMaster(void);

    bool isSupported(CReadBuffer *pInputBuffer, CFileType &type);
    
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
