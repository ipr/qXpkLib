///////////////////////////////////
//
// XFD-decrunching support:
// should load additional decoders as necessary.
//
// May be called from XpkMaster when XFD-decruncher is needed.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XFDMASTER_H
#define XFDMASTER_H

#include <QObject>
#include <QString>

// use typedefs from parent
#include "qxpklib.h"

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

#include "AnsiFile.h"
#include "FileType.h"

#include "XfdSlave.h"

// external XFD-decrunching library interface
#include "xfdLibraryBase.h"

// status information (caller and decruncher)
#include "XpkProgress.h"


class CXfdMaster : public QObject
{
	Q_OBJECT

protected:

	// need better way of sharing code..
	/*
	uint32_t MakeTag(const unsigned char *buf) const
    {
        uint32_t tmp = 0;
        tmp |= (((uint32_t)(buf[3])) << 24);
        tmp |= (((uint32_t)(buf[2])) << 16);
        tmp |= (((uint32_t)(buf[1])) << 8);
        tmp |= ((uint32_t)(buf[0]));
        return tmp;
    }
    */

	// TODO: determine inheritance to use here..
    XfdSlave *m_pXfdSlave; // TODO: move to a library?
    xfdLibraryBase *m_pSubLibrary;
    
    XfdSlave *loadDecruncher(CReadBuffer *pInputBuffer);

	void release();
	
public:
    CXfdMaster(QObject *parent = 0);
    virtual ~CXfdMaster(void);
    
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

#endif // XFDMASTER_H
