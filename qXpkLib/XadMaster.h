///////////////////////////////////
//
// XAD-library support:
// should load additional decoders as necessary.
//
// Support multi-file archives unpacking via loadable libraries.
//
// May be called from XpkMaster when needed.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//


#ifndef XADMASTER_H
#define XADMASTER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QLibrary>

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

// use typedefs from parent
#include "qxpklib.h"

#include "AnsiFile.h"
#include "FileType.h"

#include "XpkProgress.h"
#include "xadLibraryBase.h"


class CXadMaster : public QObject
{
	Q_OBJECT
	
protected:
	// wrapper for loading/unloading
	QLibrary m_SubLib; 

	// base-pointer (virtual) 
	// -> load proper by archive type
	xadLibraryBase *m_pSubLibrary;
	
	//QString m_archive;

	void release();
	
public:
    CXadMaster(QObject *parent = 0);
    virtual ~CXadMaster(void);

	bool isSupported(CIOBuffer *pInputBuffer, CFileType &type);
    
	// list compressed file information..
	bool archiveInfo(QXpkLib::CArchiveInfo &info);
    
	//void setArchive(QString &szArchive);
	void setExtractPath(QString &szPath);
	bool decrunch(XpkProgress *pProgress);
	
public slots:
	
signals:
	// errors with exceptions, other with messages
	void message(QString);
	void warning(QString);
	
};

#endif // XADMASTER_H
