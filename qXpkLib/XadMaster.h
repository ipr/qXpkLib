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

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

#include "AnsiFile.h"
#include "FileType.h"

#include "XpkProgress.h"
#include "xadLibraryBase.h"


class CXadMaster : public QObject
{
	Q_OBJECT
	
protected:

	xadLibraryBase *m_pSubLibrary;
	
	//QString m_archive;
	
public:
    CXadMaster(QObject *parent = 0);
    virtual ~CXadMaster(void);

	bool isSupported(CReadBuffer *pInputBuffer, CFileType &type);
    
	//void setArchive(QString &szArchive);
	void setExtractPath(QString &szPath);
	bool extractArchive(XpkProgress *pProgress);
	
public slots:
	
signals:
	// errors with exceptions, other with messages
	void message(QString);
	void warning(QString);
	
};

#endif // XADMASTER_H
